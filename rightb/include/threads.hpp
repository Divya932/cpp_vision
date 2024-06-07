#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <opencv2/opencv.hpp>

class VideoFeeder {
public:
    VideoFeeder(size_t size) : bufferSize(size), buffer(size), write_idx(0), read_idx(0), isFull(false) {}

    void putFrame(const cv::Mat frame);
    bool getFrame(cv::Mat frame);
    //void videoProducer(VideoFeeder &videoFeeder, const std::string &inputFile)
    //void videoConsumer(VideoFeeder &videoFeeder, const std::string &outputFile);

private:
    size_t bufferSize;
    std::vector<cv::Mat> buffer;
    size_t write_idx;
    size_t read_idx;
    bool isFull;
    std::mutex mutex;
    std::condition_variable condition;

};