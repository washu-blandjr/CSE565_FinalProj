#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../chiimg.h"

void printMat(const cv::Mat& mat, const std::string& name) {
    std::cout << "\n" << name << ":\n";
    for(int i = 0; i < mat.rows; i++) {
        for(int j = 0; j < mat.cols; j++) {
            std::cout << mat.at<double>(i,j) << " ";
        }
        std::cout << "\n";
    }
}

int main() {
    // Create simple test matrices
    double img_data[] = {
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    };

    double ip_data[] = {
        0.5, 1.0,
        1.0, 0.5
    };

    cv::Mat img(3, 3, CV_64F, img_data);
    cv::Mat ip(2, 2, CV_64F, ip_data);

    // Run chiimg
    auto [result_chiimg, result_Wip2] = chiimg(img, ip);

    // Print results
    std::cout << "Input image:";
    printMat(img, "img");

    std::cout << "\nTemplate:";
    printMat(ip, "ip");

    std::cout << "\nResults:";
    printMat(result_chiimg, "chiimg");
    printMat(result_Wip2, "Wip2");

    return 0;
}
