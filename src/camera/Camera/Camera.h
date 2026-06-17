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
        /** Initialize attributes of the class. */
        OpenCVCamera ();
        /** Stop the thread and delete class resources. */
        ~OpenCVCamera ();

        /** Open camera with selected index. */
        bool open (int cameraIndex);

        /** {@inheritDoc} */
        void close () override;
        /** {@inheritDoc} */
        bool read (CameraFrame& frame) override;
        /** {@inheritDoc} */
        bool isOpen () const override;

    private:
        /** A thread procedure for asynchronous frame reading. */
        void captureLoop (std::stop_token stopToken);
        /** Class for video capturing from cameras */
        cv::VideoCapture m_capture;
        /** Mutex for protecting asynchronously updated members. */
        std::mutex m_mutex;
        /** Buffer for reading from camera. */
        cv::Mat m_backBuffer;
        /** Buffer for processing. */
        cv::Mat m_frontBuffer;
        /** Buffer used to switch from BRG to TGB.*/
        cv::Mat m_rgb;
        /** Flag, that the new frame is ready. */
        bool m_isNewFrame;
        /** Thread for asynchronous reading. */
        std::jthread m_thread;
    };
}
