#include <threads.hpp>


void VideoFeeder::putFrame(const cv::Mat frame) {
        std::unique_lock<std::mutex> lock(mutex);
        if (isFull) {
            read_idx = (read_idx + 1) % bufferSize; // Overwrite the oldest element
        }
        buffer[write_idx] = frame.clone(); // Clone the frame to ensure a deep copy
        write_idx = (write_idx + 1) % bufferSize;
        isFull = write_idx == read_idx;
        condition.notify_all();
    }

bool VideoFeeder::getFrame(cv::Mat frame) {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, [this] { return write_idx != read_idx || isFull; });
    if (write_idx == read_idx && !isFull) {
        return false; // Buffer is empty
    }
    frame = buffer[read_idx];
    read_idx = (read_idx + 1) % bufferSize;
    isFull = false;
    return true;
}

std::atomic<bool> isRunning(true);

void videoProducer(VideoFeeder &videoFeeder, const std::string &inputFile) {
    cv::VideoCapture videoCapture(inputFile); // Open the video file
    if (!videoCapture.isOpened()) {
        std::cerr << "Error: Could not open video file." << std::endl;
        isRunning = false;
        return;
    }

    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    while (isRunning) {
        cv::Mat frame;
        videoCapture >> frame; // Capture a new frame
        if (frame.empty()) {
            std::cerr << "Error: Captured empty frame." << std::endl;
            isRunning = false;
            break;
        }

        videoFeeder.putFrame(frame);
        frameCount++;

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = currentTime - startTime;
        if (elapsedTime.count() >= 1.0) {
            double fps = frameCount / elapsedTime.count();
            std::cout << "Read speed: " << fps << " FPS" << std::endl;
            frameCount = 0;
            startTime = std::chrono::high_resolution_clock::now();
        }
    }

    videoCapture.release();
}

void videoConsumer(VideoFeeder &videoFeeder, const std::string &outputFile) {
    cv::VideoWriter videoWriter;
    int frameWidth, frameHeight, fps;
    bool isFirstFrame = true;

    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    while (isRunning) {
        cv::Mat frame;
        if (videoFeeder.getFrame(frame)) {
            if (isFirstFrame) {
                frameWidth = frame.cols;
                frameHeight = frame.rows;
                fps = 30; // Set your desired FPS for the output video
                videoWriter.open(outputFile, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, cv::Size(frameWidth, frameHeight));
                if (!videoWriter.isOpened()) {
                    std::cerr << "Error: Could not open output video file." << std::endl;
                    isRunning = false;
                    return;
                }
                isFirstFrame = false;
            }

            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsedTime = currentTime - startTime;
            if (elapsedTime.count() >= 1.0) {
                double fps = frameCount / elapsedTime.count();
                cv::putText(frame, "Write speed: " + std::to_string(fps) + " FPS", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
                frameCount = 0;
                startTime = std::chrono::high_resolution_clock::now();
            }

            videoWriter.write(frame);
            frameCount++;

            cv::imshow("Video Feed", frame);
            if (cv::waitKey(30) >= 0) { // Wait for a key press for 30ms
                isRunning = false;
            }
        }
    }

    videoWriter.release();
    cv::destroyAllWindows();
}

int main() {
    const size_t bufferSize = 10;
    VideoFeeder videoFeeder(bufferSize);

    std::string inputFile = "/home/divya.singh/work/rightb/big_buck_bunny_240p_30mb.mp4";
    std::string outputFile = "output_video.avi";

    std::thread producerThread(videoProducer, std::ref(videoFeeder), std::ref(inputFile));
    std::thread consumerThread(videoConsumer, std::ref(videoFeeder), std::ref(outputFile));

    producerThread.join();
    consumerThread.join();

    return 0;
}
