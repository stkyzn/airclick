#include "../include/pipeline.hpp"
#include <iostream>

namespace manage_threads {

    void pipeline::start() {

        std::thread t1(&pipeline::producer, this);
        std::thread t2(&pipeline::consumer, this);

        t1.join();
        t2.join();
    }

    void pipeline::producer() {
            cv::VideoCapture cam_capture(0);
            if(!cam_capture.isOpened()){
                std::cerr << "Error: Camera could not be opened." << std::endl;
                return;
            }

            cv::Mat frame;
            const std::string window_name = "Live Cam";
            cv::namedWindow(window_name,cv::WINDOW_NORMAL);
            cv::resizeWindow(window_name, 800, 600);

            std::cout << "Press 'Esc' to exit." << std::endl;
            int bufferSize = 60;
            while(is_running){
                cam_capture >> frame;

                if(frame.empty()){
                    std::cerr << "Error: Frame is empty." << std::endl;
                    break;
                }

                cv::imshow(window_name, frame);

                // This place is critical section!
                {
                    std::lock_guard<std::mutex> acquire_key(mtx);
                    if(frame_buffer.size() < 3){
                        frame_buffer.push_back(frame.clone());
                    }
                    else{   
                            cv::Mat recycled_frame = frame_buffer.front();
                            frame_buffer.pop_front();
                            frame.copyTo(recycled_frame);
                            frame_buffer.push_back(recycled_frame);
                    }
                }
                cond_var.notify_one();
                 if (cv::waitKey(30) == 27) {
                    is_running = false;
                    cond_var.notify_one();
                    break;
                }
            }
    }
    void pipeline::consumer() {
        cv::Mat frame_to_be_processed;
        while(is_running){
            {
                std::unique_lock<std::mutex> acquire_key(mtx);
                cond_var.wait(acquire_key,[this]() {
                    return !frame_buffer.empty() ||  !is_running;
                });
                if(!is_running && frame_buffer.empty()) break;
                frame_to_be_processed = frame_buffer.front();
                frame_buffer.pop_front();
            }
             // Goruntu İsleme
            std::cout << "Yapay Zeka: " << frame_to_be_processed.cols << "x" << frame_to_be_processed.rows 
            << " boyutunda bir kare isledi." << std::endl;
            //
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
       std::cout << "Consumer thread ended its job" << std::endl;
    }
}

