#include <opencv2/opencv.hpp>

class Rectangles{
    public:
        //std::vector<std::vector<int>, int>> findRectangles();
        void findRectangles(cv::Mat img);
    
    private:
        std::pair<std::vector<std::vector<cv::Point>>, std::vector<cv::Vec4i>> findContours(cv::Mat img);
};