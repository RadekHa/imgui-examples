#include "Camera.h"
#include "TraceLog/Log.hpp"

#include <opencv2/imgproc.hpp>

using namespace Camera;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Factory function.

unique_ptr<ICamera> Camera::createCamera (int cameraIndex)
{
    unique_ptr<OpenCVCamera> cam = make_unique<OpenCVCamera>();
    cam->open (cameraIndex);

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
    }
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
    return m_thread.joinable () && m_capture.isOpened ();
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
