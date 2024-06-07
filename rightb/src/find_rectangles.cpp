#include <rectangles.hpp>

//std::string img_path = "rectangle.png";

void Rectangles::findRectangles(cv::Mat img){
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::Mat gray_img, binary;
    //cv::Mat binary;

    try {
        cv::cvtColor(img, gray_img, cv::COLOR_BGR2GRAY);
        cv::threshold(gray_img, binary, 150, 255, cv::THRESH_BINARY);
        //cv::GaussianBlur(gray_img, gray_img, cv::Size(5,5), 0);
        cv::findContours(binary, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
        
        //Draw all found contours -- only rectangles should form a closed area for contours
        for (unsigned long i = 0; i < contours.size(); i++)
        {
            double area = cv::contourArea(contours[i]);
            std::cout << "Area: " << area << std::endl;
            if(hierarchy[i][3] == -1 || area < 100){
                continue;
            }
            cv::Scalar color = cv::Scalar(0, 0, 255); //i*5, (i+10)*10, (i+20)*10
            cv::drawContours(img, contours, -1, color, 2);
            //cv::Rect bounding_box = cv::boundingRect(contours[i]); cv::Point(bounding_box.x, bounding_box.y)
            std::string text = "Level " + std::to_string(hierarchy[i][3]);
            cv::putText(img, text, contours[i][0], cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 2);
        }
        cv::imwrite("thresh_img.jpg", binary);
        cv::imwrite("marked_rectangles.jpg", img);
    }
    catch (const std::exception& e){
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}

int main(int argc, char** argv){
    std::string img_path = "rectangle.png"; //argv[0];
    cv::Mat img = cv::imread(img_path);

    Rectangles rect;
    rect.findRectangles(img);
}
