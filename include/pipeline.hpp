#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <opencv2/opencv.hpp>

// Thread management namespace to easy access and proper utilization of thread functions
namespace manage_threads{
    //Pipeline class encapsulates thread data and their functions
    class pipeline{
        private:
            std::deque<cv::Mat> frame_buffer;
            std::mutex mtx;                   // Mutex lock to manage buffer pool
            std::condition_variable cond_var;        // Condition variable to enhance performance and cease polling
            bool is_running = true;            // An inifinite loop is required to take camera frames constantly and process them
        public:
            // A producer - consumer paradigm is implemented
            void start();
            void producer();
            void consumer();
    };
}

#endif