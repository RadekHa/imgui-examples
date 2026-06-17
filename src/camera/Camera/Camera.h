#pragma once
#include "ICamera.h"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

namespace Camera
{
    class OpenCVCamera : public ICamera
    {
    public:
        bool open (int cameraIndex);
        bool openStream (const std::string& url);
        void close () override;
        bool read (CameraFrame& frame) override;
        bool isOpen () const override;

    private:
        cv::VideoCapture m_capture;

        cv::Mat m_frame;

        cv::Mat m_rgb;
    };
}
