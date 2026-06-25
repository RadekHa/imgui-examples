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
// Matcher

Matcher::Matcher ()
    : m_orb {cv::ORB::create (2000, 1.2f, 8, 31, 0, 2, cv::ORB::HARRIS_SCORE, 31, 7)}
    , m_matcher {cv::BFMatcher::create (cv::NORM_HAMMING)}
{
}

bool Matcher::setTemplate (const cv::Mat& grayTemplate)
{
    m_templateKeypoints.clear ();
    m_templateDescriptors = {};

    m_orb->detectAndCompute (grayTemplate, cv::noArray (), m_templateKeypoints, m_templateDescriptors);

    if (m_templateDescriptors.empty ())
    {
        APP_WARN ("Matcher: ORB no key point found");
    }
    else
    {
        APP_INFO ("Matcher: template set, keypoints: {}", m_templateKeypoints.size ());
    }
    return hasTemplate ();
}

void Matcher::clearTemplate ()
{
    m_templateKeypoints.clear ();
    m_templateDescriptors = {};
    APP_INFO ("Matcher: template cleared");
}

bool Matcher::hasTemplate () const
{
    return !m_templateDescriptors.empty ();
}

MatchResult Matcher::match (const cv::Mat& frame) const
{
    MatchResult result;

    if (!hasTemplate ())
    {
        return result;
    }
    cv::Mat grayFrame;
    cv::cvtColor (frame, grayFrame, cv::COLOR_BGR2GRAY);

    vector<cv::KeyPoint> frameKeypoints;
    cv::Mat frameDescriptors;
    m_orb->detectAndCompute (grayFrame, cv::noArray (), frameKeypoints, frameDescriptors);

    if (frameDescriptors.empty ())
    {
        return result;
    }
    vector<cv::DMatch> matches;
    m_matcher->match (m_templateDescriptors, frameDescriptors, matches);

    vector<cv::DMatch> goodMatches;
    constexpr float distanceThreshold = 35.0f;

    for (const auto& m : matches)
    {
        if (m.distance < distanceThreshold)
        {
            goodMatches.push_back (m);
        }
    }
    result.goodMatches = static_cast<int> (goodMatches.size ());

    constexpr size_t minRequiredMatches = 15;

    APP_INFO ("Matcher: goodMatches: {}", result.goodMatches);
    cv::drawKeypoints (frame, frameKeypoints, frame, cv::Scalar (0, 255, 0));

    if (goodMatches.size () >= minRequiredMatches)
    {
        vector<cv::Point2f> srcPoints, dstPoints;

        for (const auto& m : goodMatches)
        {
            srcPoints.push_back (m_templateKeypoints [m.queryIdx].pt);
            dstPoints.push_back (frameKeypoints [m.trainIdx].pt);
        }
        cv::Mat H = cv::findHomography (srcPoints, dstPoints, cv::RANSAC, 5.0);

        if (!H.empty ())
        {
            double angleRad = atan2 (H.at<double> (1, 0), H.at<double> (0, 0));
            double angleDeg = angleRad * (180.0 / CV_PI);

            if (angleDeg < 0.0)
            {
                angleDeg += 360.0;
            }
            constexpr double maxAllowedAngle = 45.0;

            result.angleDeg = angleDeg;
            result.found = (angleDeg <= maxAllowedAngle) || (angleDeg >= (360.0 - maxAllowedAngle));

            APP_INFO ("Matcher: angleDeg: {:.1f}, found: {}", angleDeg, result.found);
        }
    }
    return result;
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
    {
        lock_guard lock{m_mutex};

        if (!m_isNewFrame)
        {
            return false;
        }
        swap (m_frontBuffer, m_backBuffer);
        matchResult = m_matchResult;
        m_isNewFrame = false;
    }

    cv::cvtColor (m_frontBuffer, m_rgb, cv::COLOR_BGR2RGB);

    frame.data = m_rgb.data;
    frame.width = m_rgb.cols;
    frame.height = m_rgb.rows;
    frame.channels = m_rgb.channels ();

    return true;
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

bool OpenCVCamera::setTemplate (string_view fileName)
{
    bool result = false;

    unique_ptr<IImageData> imgData = loadImage (fileName, 3);

    if (imgData)
    {
        cv::Mat matOrig (imgData->getHeight (),
                         imgData->getWidth (), CV_8UC3, const_cast<unsigned char*> (imgData->getPixels ()));
        cv::Mat matGrey;
        cv::cvtColor (matOrig.clone (), matGrey, cv::COLOR_BGR2GRAY);

        cv::flip (matGrey, matGrey, 1);

        {
            lock_guard lock{m_templateMutex};
            result = m_matcher.setTemplate (matGrey);
        }
    }
    return result;
}

void OpenCVCamera::clearTemplate ()
{
    {
        lock_guard lock{m_templateMutex};
        m_matcher.clearTemplate ();
    }

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
            MatchResult result;
            {
                lock_guard lock{m_templateMutex};
                result = m_matcher.match (frame);
            }

            {
                lock_guard lock{m_mutex};
                m_matchResult = result;
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
