#include <iostream>
#include <chrono>

#include "opencv2/opencv.hpp"

int processing()
{
    std::cout << "Hello world" << std::endl;
    // Create a VideoCapture object and open the input file
    // If the input is the web camera, pass 0 instead of the video file name

    cv::VideoCapture cap("2min_1080p.mp4");
    // Check if camera opened successfully
    if (!cap.isOpened())
    {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }
    const std::chrono::time_point<std::chrono::steady_clock> start =
        std::chrono::steady_clock::now();
    int frame_count = 0;
    while (1)
    {
        cv::Mat frame;
        // Capture frame-by-frame
        cap >> frame;
        // If the frame is empty, break immediately
        if (frame.empty())
            break;
        // Display the resulting frame
        // cv::imshow("Frame", frame);
        // Press  ESC on keyboard to exit
        // char c = (char)cv::waitKey(25);
        // if (c == 27)
        //     break;
        frame_count++;
        if (frame_count % 100 == 0)
            std::cout << "Frame processed " << frame_count << std::endl;
    }
    const std::chrono::time_point<std::chrono::steady_clock> end =
        std::chrono::steady_clock::now();
    float fps = (frame_count * 1000.0) / std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "frame_count: " << frame_count << " , FPS " << fps << std::endl;
    // When everything done, release the video capture object
    cap.release();

    return 0;
}
int main(int argc, char const *argv[]) {
    
}