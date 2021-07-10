// Copyright 2021 DPeshkoff;
// Distributed under the GNU General Public License, Version 3.0. (See
// accompanying file LICENSE)

#include <mosquitto.h>

#include <cmath>
#include <iostream>                     // cout, cerr
#include <opencv2/highgui/highgui.hpp>  // opencv gui
#include <opencv2/imgproc.hpp>          // COLOR_BGR2GRAY
#include <opencv2/opencv.hpp>           // opencv base

#include "json.hpp"

using json = nlohmann::json;

using namespace cv;

void find_nearest_object(std::vector<std::vector<Point>>& points, Point point,
                         bool phase, std::size_t& k) {
    int32_t min_distance = INT32_MAX;
    int32_t min_distance_index = 0;

    for (std::size_t i = 0; i < points.size(); ++i) {
        if (!points[i].empty() and phase == 0) {
            int32_t current_min = sqrt(pow(points[i].back().x - point.x, 2) +
                                       pow(points[i].back().y - point.y, 2));

            if (min_distance > current_min) {
                min_distance = current_min;
                min_distance_index = i;
            }
        } else if (phase == 1) {
            points[k].push_back(point);
            ++k;
            return;
        }
    }

    points[min_distance_index].push_back(point);
}

void draw_track(std::vector<Point> points, Scalar color, Mat frame) {
    for (std::size_t i = 0; i < points.size(); ++i) {
        if (points[i - 1].x == 0 or points[i - 1].y == 0) {
            i++;
        }
        if (points[i].x == 0 or points[i].y == 0) {
            i++;
        }
        line(frame, points[i - 1], points[i], color, 3);
    }
}

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

    mosquitto_lib_init();

    struct mosquitto* client = mosquitto_new("broker", true, NULL);

    int connection = mosquitto_connect(client, "localhost", 1883, 60);

    if (connection != 0) {
        std::cout << "\033[1;31m[ERROR]\033[0m No connection with broker."
                  << std::endl;
        mosquitto_destroy(client);
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
    RNG rng(54321);
    bool phase = true;

    Mat element =
        getStructuringElement(0, Size(2 * 1 + 1, 2 * 1 + 1), Point(1, 1));

    std::vector<std::vector<Point>> points(2);

    uint32_t max_contours = 0;

    uint64_t frame_count = 0;

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

        GaussianBlur(src_gray, src_gray, Size(5, 5), 0, 0);

        medianBlur(src_gray, src_gray, 3);

        threshold(src_gray, src_gray, 190, 255, THRESH_BINARY);  // 188

        morphologyEx(src_gray, src_gray, MORPH_GRADIENT, element);

        dilate(src_gray, src_gray, Mat(), Point(0, 0), 13);

        std::vector<std::vector<Point>> original_contours;
        std::vector<std::vector<Point>> contours;
        std::vector<Vec4i> hierarchy;

        findContours(src_gray, original_contours, hierarchy, RETR_EXTERNAL,
                     CHAIN_APPROX_TC89_L1);

        for (auto i : original_contours) {
            if (contourArea(i) > 1000) {
                contours.push_back(i);
            }
        }
        if (contours.size() > max_contours) {
            max_contours = contours.size();
            points.resize(max_contours);
        }

        std::vector<std::vector<Point>> contours_poly(contours.size());
        std::vector<Rect> boundRect(contours.size());

        std::size_t k = 0;
        for (std::size_t i = 0; i < contours.size(); ++i) {
            approxPolyDP(contours[i], contours_poly[i], 3, true);
            boundRect[i] = boundingRect(contours_poly[i]);
            Point c(boundRect[i].x + boundRect[i].width / 2,
                    boundRect[i].y + boundRect[i].height / 2);
            find_nearest_object(points, c, phase, k);
        }

        for (std::size_t i = 0; i < contours.size(); ++i) {
            Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256),
                                  rng.uniform(0, 256));
            drawContours(frame, contours, static_cast<int>(i), color, 2, LINE_8,
                         hierarchy, 0);
            if (boundRect[i].area() > 35) {
                rectangle(frame, boundRect[i].tl(), boundRect[i].br(), color,
                          4);
                draw_track(points[i], color, frame);
            }
        }

        imshow("opencv-constrast", frame);

        if (points[1].size() != 0) {
            phase = false;
        }

        if (waitKey(30) == 27) {
            std::cout << "\033[1;36m[INFO]\033[0m Escape key is pressed by "
                         "the user. Shutting down."
                      << std::endl;
            break;
        }

        ++frame_count;
        if (frame_count % 10 == 0) {
            nlohmann::json message;

            uint32_t obj_id = 0;
            for (auto i : points) {
                if (i.size() > 1) {
                    message["x"] = i.back().x;
                    message["y"] = i.back().y;
                    ++obj_id;
                }
            }

            std::string s = message.dump();

            if (s.length() > 1) {
                mosquitto_publish(client, NULL, "opencv", s.length(),
                                  s.c_str(), 0, false);
            }
        }
    }

    mosquitto_disconnect(client);
    mosquitto_destroy(client);
    mosquitto_lib_cleanup();

    return EXIT_SUCCESS;
}
