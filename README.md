## Detect nested rectangles and show their levels   
The first part of the solution in this repository contains solution to show how can we find rectangles nested one within another at multiple levels.  
Solution file: rightb/src/find_rectangles.cpp  
Supporting header file: rightb/include/rectangles.hpp  

### Method:  
1. Convert the image to grayscale
2. Binarize or threshold the image in this case, because the color feature is not important to us.
3. If the gradients were important in certain images, we could apply GaussianBlur to improve the edge quality before finding the edges.
4. Apply FindContour with cv::RETR_TREE because this mode of operation returns the hierarchy of all the contours that can be found in a nested sequence.
5. Later the function findRectangles() overlays the bounding boxes at each hierarchy (excluding the outermost) along with the levels displayed on top of each box.

The code is implemented using OOP principles in a modular way.
Please find the output attached inside the outputs directory.
Refs from internet: https://docs.opencv.org/4.x/df/d0d/tutorial_find_contours.html   

## 2. Video Feed and Multithreading  
 This solution implements two threads, one of which continuously feeds frames to a shared buffer,
and another consumes frames from the shared buffer.

This solution has a class VideoFeeder defined inside the file include/threads.hpp
It implements two functions putFrame(cv::Mat frame) and getFrame(cv::Mat frame)
The class also encloses some private variables which define the **buffer_size, buffer vector(cv::Mat), write_idx, read_idx, mutex, and a condition_variable**.  

The main function begins with creating two threads:  
producerThread(videoProducer, std::ref(videoFeeder), std::ref(inputFile));
consumerThread(videoConsumer, std::ref(videoFeeder), std::ref(outputFile));

1. First, the producer thread is spawned into action and it reads frames from camera source cv::VideoCapture videoCapture(0) or video file cv::VideoCapture videoCapture(inputFile)  
2. Then the putFrame() function is called which lock the mutex for any other operation and checks if the buffer is full. In any case, the write_index is calculate as the last (read_index+1)/size of the buffer. This ensures only the oldest frame is updated in case of an overflow.
3. Later the waiting thread is notified using notify_all() over a condition_variable. That's when the consumer thread spawns into action, acquires the mutex for read operation.
4. VideoConsumer then acquires the lock and reads from the last read_index pointing to the buffer.

5. This code compiles successfully and has its executable attached inside execs/thread.
