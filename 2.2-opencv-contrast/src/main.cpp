// Copyright 2021 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#include <iostream>                     // cout, cerr
#include <opencv2/highgui/highgui.hpp>  // opencv gui
#include <opencv2/imgproc.hpp>          // COLOR_BGR2GRAY
#include <opencv2/opencv.hpp>           // opencv base

using namespace cv;

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::cout << "\033[1;36m[INFO]\033[0m Opening: " << argv[1]
                  << std::endl;
    } else {
        std::cerr << "\033[1;31m[ERROR]\033[0m Not enough arguments."
                  << std::endl;
        std::cerr << "Usage: \033[1;35m ./hw ${test video file name} \033[0m"
                  << std::endl;
        std::cerr
            << "Example: \033[1;36m ./hw ../examples/01-rtsoft-example.mov "
               "\033[0m"
            << std::endl;
        return EXIT_FAILURE;
    }

    VideoCapture cap(argv[1]);

    if (!cap.isOpened()) {
        std::cerr << "\033[1;31m[ERROR]\033[0m Cannot open the video file."
                  << std::endl;
        return EXIT_FAILURE;
    }

    double fps = cap.get(CAP_PROP_FPS);
    std::cout << "\033[1;36m[INFO]\033[0m Frames per second: " << fps
              << std::endl;

    namedWindow("opencv-constrast", WINDOW_AUTOSIZE);

    Mat src_gray;
    int thresh = 55;  // 55
    RNG rng(54321);

    while (true) {
        Mat frame;
        bool bSuccess = cap.read(frame);

        if (!bSuccess) {
            std::cout << "\033[1;36m[INFO]\033[0m Video ended. Shutting down."
                      << std::endl;
            break;
        }

        cvtColor(frame, src_gray, COLOR_BGR2GRAY);

        Mat thresh_m;

        threshold(src_gray, thresh_m, 188, 255, THRESH_BINARY);

        // medianBlur(src_gray, src_gray, 5);

        Mat canny_output;
        Canny(thresh_m, canny_output, thresh, thresh * 2);

        std::vector<std::vector<Point>> contours;
        std::vector<Vec4i> hierarchy;

        findContours(canny_output, contours, hierarchy, RETR_CCOMP,
                     CHAIN_APPROX_TC89_L1);

        std::vector<std::vector<Point>> contours_poly(contours.size());
        std::vector<Rect> boundRect(contours.size());

        for (std::size_t i = 0; i < contours.size(); ++i) {
            approxPolyDP(contours[i], contours_poly[i], 3, true);
            boundRect[i] = boundingRect(contours_poly[i]);
        }

        for (std::size_t i = 0; i < contours.size(); ++i) {
            Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256),
                                  rng.uniform(0, 256));
            drawContours(frame, contours, static_cast<int>(i), color, 2, LINE_8,
                         hierarchy, 0);
            rectangle(frame, boundRect[i].tl(), boundRect[i].br(), color, 4);
        }

        imshow("opencv-constrast", frame);

        if (waitKey(30) == 27) {
            std::cout << "\033[1;36m[INFO]\033[0m Escape key is pressed by "
                         "the user. Shutting down."
                      << std::endl;
            break;
        }
    }

    return EXIT_SUCCESS;
}
