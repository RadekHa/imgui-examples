#include "Camera.h"
#include "Image/IImageData.h"
#include "TraceLog/Log.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

using namespace Camera;
using namespace std;
using namespace Image;

///////////////////////////////////////////////////////////////////////////////
// Factory function.

unique_ptr<ICamera> Camera::createCamera (int cameraIndex)
{
    unique_ptr<OpenCVCamera> cam = make_unique<OpenCVCamera>();

    if (!cam->open (cameraIndex))
    {
        return nullptr;
    }
    return cam;
}

///////////////////////////////////////////////////////////////////////////////
// OpenCVCamera

OpenCVCamera::OpenCVCamera ()
    : m_isNewFrame {}
{
    m_capture.set (cv::CAP_PROP_OPEN_TIMEOUT_MSEC, 5000);
    m_capture.set (cv::CAP_PROP_READ_TIMEOUT_MSEC, 2000);
}

OpenCVCamera::~OpenCVCamera () = default;

bool OpenCVCamera::open (int cameraIndex)
{
    bool result = {};

    close ();

    if (m_capture.open (cameraIndex))
    {
        APP_INFO ("Camera opened: index {}", cameraIndex);

        m_thread = jthread ( [this] (stop_token token) {
            captureLoop (token);
        });

        result = true;

        {
            m_orbDetector = cv::ORB::create (
                2000,
                1.2f,
                8,
                5, // 15
                0,
                2,
                cv::ORB::HARRIS_SCORE,
                31,
                10);

            m_matcher = cv::BFMatcher::create (cv::NORM_HAMMING);

            unique_ptr<IImageData> imgData = loadImage ("C:\\code\\imgui-examples\\src\\assets\\images\\ruka.png", 3);

            if (imgData)
            {
                cv::Mat matOrig (imgData->getHeight (),
                                 imgData->getWidth (), CV_8UC3, const_cast<unsigned char*> (imgData->getPixels ()));
                cv::Mat matGrey;
                cv::cvtColor (matOrig.clone (), matGrey, cv::COLOR_BGR2GRAY);

                cv::flip (matGrey, matGrey, 1);

                m_templateSize = matGrey.size ();

                m_orbDetector->detectAndCompute (matGrey, cv::noArray (), m_templateKeypoints, m_templateDescriptors);

                if (m_templateDescriptors.empty ())
                {
                    APP_WARN ("ORB nenašel na šablonì žádné klíèové body!");
                }
            }
        }
    }
    else
    {
        APP_ERROR ("[Camera] Can not open camera with index {}", cameraIndex);
    }
    return result;
}

void OpenCVCamera::close ()
{
    if (m_thread.joinable ())
    {
        m_thread.request_stop ();
        m_thread.join ();
    }

    if (m_capture.isOpened ())
    {
        m_capture.release ();
    }
    {
        lock_guard lock{m_mutex};
        m_backBuffer = {};
        m_frontBuffer = {};
        m_isNewFrame = false;
        m_matchResult = {};
    }
}

bool OpenCVCamera::read (CameraFrame& frame, MatchResult& matchResult)
{
    return false;
}

bool OpenCVCamera::read (CameraFrame& frame)
{
    {
        lock_guard lock{m_mutex};

        if (!m_isNewFrame)
        {
            return false;
        }
        swap (m_frontBuffer, m_backBuffer);
        m_isNewFrame = false;
    }

    cv::cvtColor (m_frontBuffer, m_rgb, cv::COLOR_BGR2RGB);

    frame.data = m_rgb.data;
    frame.width = m_rgb.cols;
    frame.height = m_rgb.rows;
    frame.channels = m_rgb.channels ();

    return true;
}

bool OpenCVCamera::isOpen () const
{
    return m_capture.isOpened ();
}

void OpenCVCamera::setTemplate (const cv::Mat& templ)
{
    {
        lock_guard lock{m_templateMutex};
        m_template = templ;
    }

    APP_INFO ("Template set: {}x{}", templ.cols, templ.rows);
}

void OpenCVCamera::clearTemplate ()
{
    lock_guard lock{m_templateMutex};
    m_template = {};

    APP_INFO ("Template cleared");
}

void OpenCVCamera::captureLoop (stop_token stopToken)
{
    try
    {
        cv::Mat frame;

        while (!stopToken.stop_requested ())
        {
            if (!m_capture.read (frame) || frame.empty ())
            {
                this_thread::sleep_for (chrono::milliseconds (10));
                continue;
            }
            runMatching (frame);

            if (!m_template.empty ())
            {
                cv::rectangle (
                    frame,
                    cv::Rect (m_matchResult.x, m_matchResult.y, m_matchResult.width, m_matchResult.height),
                    cv::Scalar (0, 0, 255),
                    1);
            }
            {
                lock_guard lock{m_mutex};
                swap (m_backBuffer, frame);
                m_isNewFrame = true;
            }
        }
    }
    catch (const cv::Exception& e)
    {
        APP_ERROR ("Capture thread cv::Exception: {}", e.what ());
    }
    catch (const std::exception& e)
    {
        APP_ERROR ("Capture thread std::exception: {}", e.what ());
    }
    catch (...)
    {
        APP_ERROR ("Capture thread unknown exception");
    }
}

void OpenCVCamera::runMatching (const cv::Mat& frame)
{
    cv::Mat gray;
    cv::cvtColor (frame, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::KeyPoint> frameKeypoints;
    cv::Mat frameDescriptors;
    m_orbDetector->detectAndCompute (gray, cv::noArray (), frameKeypoints, frameDescriptors);

    if (!frameDescriptors.empty ())
    {
        std::vector<cv::DMatch> matches;
        m_matcher->match (m_templateDescriptors, frameDescriptors, matches);

        std::vector<cv::DMatch> goodMatches;
        constexpr float distanceThreshold = 30.0f;

        for (const auto& match : matches)
        {
            if (match.distance < distanceThreshold)
            {
                goodMatches.push_back (match);
            }
        }
        constexpr size_t minRequiredMatches = 15;

        if (goodMatches.size () >= minRequiredMatches)
        {
            std::vector<cv::Point2f> srcPoints;
            std::vector<cv::Point2f> dstPoints;

            for (const auto& match : goodMatches)
            {
                srcPoints.push_back (m_templateKeypoints [match.queryIdx].pt);
                dstPoints.push_back (frameKeypoints [match.trainIdx].pt);
            }
            cv::Mat H = cv::findHomography (srcPoints, dstPoints, cv::RANSAC, 5.0);

            if (!H.empty ())
            {
                double r11 = H.at<double> (0, 0);
                double r21 = H.at<double> (1, 0);

                double angleRad = std::atan2 (r21, r11);
                double angleDeg = angleRad * (180.0 / CV_PI);

                if (angleDeg < 0)
                {
                    angleDeg += 360.0;
                }
                constexpr double maxAllowedAngle = 45.0;

                if ((angleDeg <= maxAllowedAngle) || (angleDeg >= (360.0 - maxAllowedAngle)))
                {
                    cv::drawKeypoints (frame, frameKeypoints, frame, cv::Scalar (0, 255, 0));
                }
                APP_INFO ("angleDeg: {}", angleDeg);
            }
        }
    }
}
