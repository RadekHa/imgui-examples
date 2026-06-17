#pragma once
#include "ICamera.h"

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <thread>

namespace Camera
{
    class OpenCVCamera : public ICamera
    {
    public:
        OpenCVCamera ();
        ~OpenCVCamera ();

        bool open (int cameraIndex);
        void close () override;
        bool read (CameraFrame& frame) override;
        bool isOpen () const override;

    private:
        void captureLoop (std::stop_token stopToken);

        cv::VideoCapture m_capture;

        std::mutex m_mutex;

        cv::Mat m_backBuffer;
        cv::Mat m_frontBuffer;
        bool m_isNewFrame;

        cv::Mat m_rgb;

        std::jthread m_thread;
    };
}
