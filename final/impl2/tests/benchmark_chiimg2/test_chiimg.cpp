#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "chiimg_wrapper.hpp"

void printResults(const cv::Mat& chiimg_result, const cv::Mat& wip2_result,
                 const std::string& mode, int template_size = -1) {
    if (template_size != -1) {
        std::cout << "\nTemplate Size " << template_size << "x" << template_size << ":\n";
    }
    std::cout << "Mode: " << mode << "\n";

    std::cout << "CHIIMG Result (3x3 sample):\n";
    for(int i = 0; i < std::min(3, chiimg_result.rows); i++) {
        for(int j = 0; j < std::min(3, chiimg_result.cols); j++) {
            std::cout << chiimg_result.at<double>(i,j) << " ";
        }
        std::cout << "\n";
    }

    std::cout << "WIP2 Result (3x3 sample):\n";
    for(int i = 0; i < std::min(3, wip2_result.rows); i++) {
        for(int j = 0; j < std::min(3, wip2_result.cols); j++) {
            std::cout << wip2_result.at<double>(i,j) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int main() {
    const int LARGE_IMAGE_SIZE = 1000;
    const int TEMPLATE_SIZE = 100;
    const int ITERATIONS = 10;

    // Generate base matrices with fixed seed
    cv::RNG rng(12345);
    cv::Mat large_img(LARGE_IMAGE_SIZE, LARGE_IMAGE_SIZE, CV_64F);
    cv::Mat template_img(TEMPLATE_SIZE, TEMPLATE_SIZE, CV_64F);
    rng.fill(large_img, cv::RNG::UNIFORM, 0, 1);
    rng.fill(template_img, cv::RNG::UNIFORM, 0, 1);

    // Generate noise matrices for iterations
    std::vector<cv::Mat> noise_matrices;
    for (int i = 0; i < ITERATIONS; i++) {
        cv::Mat noise(LARGE_IMAGE_SIZE, LARGE_IMAGE_SIZE, CV_64F);
        rng.fill(noise, cv::RNG::NORMAL, 0, 0.01);
        noise_matrices.push_back(noise);
    }

    // Since we're using Vitis HLS, we'll only test with 'same' mode
    std::cout << "\nTesting with Vitis HLS implementation...\n";
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERATIONS; i++) {
        cv::Mat noisy_img = large_img + noise_matrices[i];
        auto [result_chiimg, result_Wip2] = chiimg_hls(noisy_img, template_img, cv::Mat(), cv::Mat(), "same");

        // Print results for first iteration only
        if (i == 0) {
            printResults(result_chiimg, result_Wip2, "same");
        }
        std::cout << "Completed iteration " << (i + 1) << "/" << ITERATIONS << "\r" << std::flush;
    }

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::high_resolution_clock::now() - start_time);
    double seconds = duration.count() / 1000000.0;
    std::cout << "\nTotal time: " << seconds << " seconds\n";
    std::cout << "Average time per iteration: " << seconds/ITERATIONS << " seconds\n";

    // Test different template sizes
    std::vector<int> template_sizes = {50, 100, 200};

    for (int size : template_sizes) {
        if (size > MAX_TEMPLATE_SIZE) {
            std::cout << "\nSkipping template size " << size << " (exceeds maximum of " << MAX_TEMPLATE_SIZE << ")\n";
            continue;
        }

        std::cout << "\nTesting template size: " << size << "x" << size << "\n";
        cv::Mat test_template(size, size, CV_64F);
        rng.fill(test_template, cv::RNG::UNIFORM, 0, 1);

        auto start_time = std::chrono::high_resolution_clock::now();
        auto [result_chiimg, result_Wip2] = chiimg_hls(large_img, test_template, cv::Mat(), cv::Mat(), "same");
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::high_resolution_clock::now() - start_time);

        printResults(result_chiimg, result_Wip2, "same", size);
        double seconds = duration.count() / 1000000.0;
        std::cout << "Time taken: " << seconds << " seconds\n";
    }

    std::cout << "\nPerformance Stats:\n";
    std::cout << "Base image size: " << large_img.size() << "\n";
    std::cout << "Base template size: " << template_img.size() << "\n";
    std::cout << "Number of iterations: " << ITERATIONS << "\n";

    return 0;
}