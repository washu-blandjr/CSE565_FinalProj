#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "../../chiimg.h"

void printPerformanceStats(const std::string& testName,
                          std::chrono::microseconds duration,
                          int iterations) {
    double seconds = duration.count() / 1000000.0;
    std::cout << "\n" << testName << " Performance:\n";
    std::cout << "Total time: " << seconds << " seconds\n";
    std::cout << "Average time per iteration: " << (seconds / iterations) << " seconds\n";
    std::cout << "Iterations per second: " << (iterations / seconds) << "\n";
}

cv::Mat generateRandomMatrix(int rows, int cols) {
    cv::Mat mat(rows, cols, CV_64F);
    cv::randu(mat, 0.0, 1.0);
    return mat;
}

int main() {
    // Test parameters
    const int LARGE_IMAGE_SIZE = 1000;  // 1000x1000 image
    const int TEMPLATE_SIZE = 100;      // 100x100 template
    const int ITERATIONS = 10;          // Number of times to run the test

    std::cout << "Starting benchmark test with:\n";
    std::cout << "Image size: " << LARGE_IMAGE_SIZE << "x" << LARGE_IMAGE_SIZE << "\n";
    std::cout << "Template size: " << TEMPLATE_SIZE << "x" << TEMPLATE_SIZE << "\n";
    std::cout << "Iterations: " << ITERATIONS << "\n";

    // Generate large random test matrices
    cv::Mat large_img = generateRandomMatrix(LARGE_IMAGE_SIZE, LARGE_IMAGE_SIZE);
    cv::Mat template_img = generateRandomMatrix(TEMPLATE_SIZE, TEMPLATE_SIZE);

    // Warm-up run (to avoid first-run overhead)
    auto [warmup_result, warmup_wip2] = chiimg(large_img, template_img);

    // Benchmark with different modes
    std::vector<std::string> modes = {"full", "same", "valid"};

    for (const auto& mode : modes) {
        std::cout << "\nTesting mode: " << mode << "\n";

        auto start_time = std::chrono::high_resolution_clock::now();

        // Run multiple iterations
        for (int i = 0; i < ITERATIONS; i++) {
            // Generate slightly different random noise each time
            cv::Mat noise(large_img.size(), CV_64F);
            cv::randn(noise, 0, 0.01);
            cv::Mat noisy_img = large_img + noise;

            auto [result_chiimg, result_Wip2] = chiimg(noisy_img, template_img, cv::Mat(), cv::Mat(), mode);

            // Print progress
            std::cout << "Completed iteration " << (i + 1) << "/" << ITERATIONS << "\r" << std::flush;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>
            (end_time - start_time);

        printPerformanceStats("Mode: " + mode, duration, ITERATIONS);
    }

    // Additional stress test with different template sizes
    std::cout << "\nRunning stress test with varying template sizes...\n";
    std::vector<int> template_sizes = {50, 100, 200};

    for (int size : template_sizes) {
        cv::Mat test_template = generateRandomMatrix(size, size);
        std::cout << "\nTesting template size: " << size << "x" << size << "\n";

        auto start_time = std::chrono::high_resolution_clock::now();

        auto [result_chiimg, result_Wip2] = chiimg(large_img, test_template);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>
            (end_time - start_time);

        printPerformanceStats("Template size: " + std::to_string(size), duration, 1);
    }

    return 0;
}
