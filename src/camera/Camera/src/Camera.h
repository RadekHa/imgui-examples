#pragma once
#include "Camera/ICamera.h"

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>

#include <mutex>
#include <thread>
#include <vector>

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

        bool read (CameraFrame& frame, MatchResult& matchResult);

        bool read (CameraFrame& frame) override;
        /** {@inheritDoc} */
        bool isOpen () const override;

        /** Set template image to search for in the camera frames. */
        void setTemplate (const cv::Mat& templ);
        /** Clear the template image. */
        void clearTemplate ();
    private:
        /** A thread procedure for asynchronous frame reading. */
        void captureLoop (std::stop_token stopToken);
        /** Run template matching on the given frame. */
        void runMatching (const cv::Mat& frame);

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

        /** Last match result from runMatching. */
        MatchResult m_matchResult;

        /** Mutex for protecting template members. */
        std::mutex m_templateMutex;
        /** Grayscale template image. */
        cv::Mat m_template;

        cv::Ptr<cv::ORB> m_orbDetector;
        cv::Ptr<cv::BFMatcher> m_matcher;

        std::vector<cv::KeyPoint> m_templateKeypoints;
        cv::Mat m_templateDescriptors;
        cv::Size m_templateSize;

        /** Thread for asynchronous reading. */
        std::jthread m_thread;
    };
}
