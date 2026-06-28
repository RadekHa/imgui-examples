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
{
}

MatchResult Matcher::match (const cv::Mat& frame) const
{
    cv::Mat hsv, mask;
    cv::cvtColor (frame, hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower (20, 100, 100);
    cv::Scalar upper (35, 255, 255);
    cv::inRange (hsv, lower, upper, mask);

    cv::Mat blurred;
    cv::GaussianBlur (mask, blurred, cv::Size (9, 9), 2);

    int minRadius = frame.cols / 4;
    int maxRadius = frame.cols;

    vector<cv::Vec3f> circles;
    cv::HoughCircles (blurred, circles, cv::HOUGH_GRADIENT, 1,
                      mask.rows / 8,
                      100, 30,
                      minRadius, maxRadius);


    MatchResult result;

    if (!circles.empty ())
    {
        for (auto c : circles)
        {
            cv::Point center (cvRound (c [0]), cvRound (c [1]));
            int radius = cvRound (c [2]);

            cv::Mat circleMask = cv::Mat::zeros (mask.size (), CV_8U);
            cv::circle (circleMask, center, radius, 255, -1);

            int total = cv::countNonZero (circleMask);
            int yellow = cv::countNonZero (mask & circleMask);
            float ratio = static_cast<float> (yellow) / total;

            APP_INFO ("yellow ratio: {:.2f} /{}", ratio, radius);

            constexpr float minYellowRatio = 0.6f;

            cv::circle (frame, center, radius, cv::Scalar (0, 255, 0), 2);
            cv::circle (frame, center, 3, cv::Scalar (0, 0, 255), -1);

            if (ratio > minYellowRatio)
            {
                result.found = true;
            }
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
            MatchResult result = m_matcher.match (frame);

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
