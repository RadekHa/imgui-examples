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

bool OpenCVCamera::open (int cameraIndex)
{
    close ();

    m_capture.open (cameraIndex);

    if (!m_capture.isOpened ())
    {
        APP_ERROR ("[Camera] Can not open camera with index {}", cameraIndex);
        return false;
    }
    APP_INFO ("Camera opened: index {}", cameraIndex);

    cv::Mat dummy;

    for (int i = 0; i < 5; ++i)
    {
        m_capture.read (dummy);
    }
    return true;
}

bool OpenCVCamera::openStream (const std::string& url)
{
    close ();

    m_capture.open (url);

    if (!m_capture.isOpened ())
    {
        APP_WARN ("Failed to open stream: {}", url);
        return false;
    }
    APP_INFO ("Stream opened: {}", url);
    return true;
}

void OpenCVCamera::close ()
{
    if (m_capture.isOpened ())
    {
        m_capture.release ();
    }
}

bool OpenCVCamera::read (CameraFrame& frame)
{
    if (!isOpen () || !m_capture.read (m_frame) || m_frame.empty ())
    {
        return false;
    }
    cv::cvtColor (m_frame, m_rgb, cv::COLOR_BGR2RGB);

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
