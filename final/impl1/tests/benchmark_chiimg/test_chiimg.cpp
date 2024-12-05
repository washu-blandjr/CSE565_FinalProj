#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "chiimg_wrapper.hpp"  // Changed from chiimg.hpp

int main() {
    // Generate test matrices with fixed dimensions
    const int LARGE_IMAGE_SIZE = 128;
    const int TEMPLATE_SIZE = 32;

    cv::Mat large_img(LARGE_IMAGE_SIZE, LARGE_IMAGE_SIZE, CV_64F);
    cv::Mat template_img(TEMPLATE_SIZE, TEMPLATE_SIZE, CV_64F);
    cv::Mat W;        // Empty matrix for default W
    cv::Mat Wip2;     // Empty matrix for default Wip2
    std::string range = "same";  // Default range parameter

    // Use constant seed for reproducibility
    cv::RNG rng(12345);
    rng.fill(large_img, cv::RNG::UNIFORM, 0, 1);
    rng.fill(template_img, cv::RNG::UNIFORM, 0, 1);

    std::cout << "Running chiimg with generated matrices...\n";
    auto start_time = std::chrono::high_resolution_clock::now();

    // Run chiimg with all required parameters - using the HLS wrapper function
    auto [result_chiimg, result_Wip2] = chiimg_hls(large_img, template_img, W, Wip2, range);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    // Print performance stats
    double seconds = duration.count() / 1000000.0;
    std::cout << "\nPerformance Stats:\n";
    std::cout << "Total time: " << seconds << " seconds\n";
    std::cout << "Image size: " << large_img.size() << "\n";
    std::cout << "Template size: " << template_img.size() << "\n";

    // Print first few elements of results for verification (optional)
    std::cout << "\nFirst few elements of results:\n";
    std::cout << "CHIIMG Result (top-left 3x3):\n";
    for(int i = 0; i < std::min(3, result_chiimg.rows); i++) {
        for(int j = 0; j < std::min(3, result_chiimg.cols); j++) {
            std::cout << result_chiimg.at<double>(i,j) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "\nWIP2 Result (top-left 3x3):\n";
    for(int i = 0; i < std::min(3, result_Wip2.rows); i++) {
        for(int j = 0; j < std::min(3, result_Wip2.cols); j++) {
            std::cout << result_Wip2.at<double>(i,j) << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
