#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "../../chiimg.h"

void saveMatrixToFile(const cv::Mat& mat, std::ofstream& file) {
    file << mat.rows << " " << mat.cols << "\n";
    for(int i = 0; i < mat.rows; i++) {
        for(int j = 0; j < mat.cols; j++) {
            file << mat.at<double>(i,j) << " ";
        }
        file << "\n";
    }
}

cv::Mat readMatrixFromFile(std::ifstream& file) {
    int rows, cols;
    file >> rows >> cols;
    cv::Mat mat(rows, cols, CV_64F);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            file >> mat.at<double>(i,j);
        }
    }
    return mat;
}

void saveResults(std::ofstream& outFile, const cv::Mat& chiimg_result, const cv::Mat& wip2_result,
                const std::string& mode, int template_size = -1) {
    outFile.precision(10);
    if (template_size != -1) {
        outFile << "Template Size " << template_size << "x" << template_size << ":\n";
    }
    outFile << "Mode: " << mode << "\n";
    outFile << "CHIIMG Result:\n";
    saveMatrixToFile(chiimg_result, outFile);
    outFile << "WIP2 Result:\n";
    saveMatrixToFile(wip2_result, outFile);
    outFile << "\n";
}

int main() {
    const std::string inputFile = "input.txt";
    const std::string outputFile = "output.txt";
    const int LARGE_IMAGE_SIZE = 1000;
    const int TEMPLATE_SIZE = 100;
    const int ITERATIONS = 10;

    // Check if input file exists
    std::ifstream inFile(inputFile);
    cv::Mat large_img, template_img;
    std::vector<cv::Mat> noise_matrices;

    if (!inFile.good()) {
        std::cout << "Generating new input file...\n";

        // Generate matrices with fixed seed
        cv::RNG rng(12345);
        large_img = cv::Mat(LARGE_IMAGE_SIZE, LARGE_IMAGE_SIZE, CV_64F);
        template_img = cv::Mat(TEMPLATE_SIZE, TEMPLATE_SIZE, CV_64F);
        rng.fill(large_img, cv::RNG::UNIFORM, 0, 1);
        rng.fill(template_img, cv::RNG::UNIFORM, 0, 1);

        // Generate noise matrices for iterations
        for (int i = 0; i < ITERATIONS; i++) {
            cv::Mat noise(LARGE_IMAGE_SIZE, LARGE_IMAGE_SIZE, CV_64F);
            rng.fill(noise, cv::RNG::NORMAL, 0, 0.01);
            noise_matrices.push_back(noise);
        }

        // Save all matrices to input file
        std::ofstream outFile(inputFile);
        outFile.precision(10);
        outFile << "Large Image:\n";
        saveMatrixToFile(large_img, outFile);
        outFile << "Template Image:\n";
        saveMatrixToFile(template_img, outFile);
        outFile << "Noise Matrices:\n";
        outFile << ITERATIONS << "\n";
        for (const auto& noise : noise_matrices) {
            saveMatrixToFile(noise, outFile);
        }
        outFile.close();
    } else {
        std::cout << "Reading from existing input file...\n";
        std::string header;
        std::getline(inFile, header); // Read "Large Image:" header
        large_img = readMatrixFromFile(inFile);
        std::getline(inFile, header); // Read empty line
        std::getline(inFile, header); // Read "Template Image:" header
        template_img = readMatrixFromFile(inFile);
        std::getline(inFile, header); // Read empty line
        std::getline(inFile, header); // Read "Noise Matrices:" header
        int num_noise;
        inFile >> num_noise;
        for (int i = 0; i < num_noise; i++) {
            noise_matrices.push_back(readMatrixFromFile(inFile));
        }
    }
    inFile.close();

    std::ofstream outFile(outputFile);
    std::vector<std::string> modes = {"full", "same", "valid"};

    // Test different modes
    for (const auto& mode : modes) {
        std::cout << "\nTesting mode: " << mode << "\n";
        auto start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < ITERATIONS; i++) {
            cv::Mat noisy_img = large_img + noise_matrices[i];
            auto [result_chiimg, result_Wip2] = chiimg(noisy_img, template_img, cv::Mat(), cv::Mat(), mode);

            // Save results for each iteration
            saveResults(outFile, result_chiimg, result_Wip2, mode);
            std::cout << "Completed iteration " << (i + 1) << "/" << ITERATIONS << "\r" << std::flush;
        }

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::high_resolution_clock::now() - start_time);
        double seconds = duration.count() / 1000000.0;
        std::cout << "\nMode " << mode << " total time: " << seconds << " seconds\n";
    }

    // Test different template sizes
    std::vector<int> template_sizes = {50, 100, 200};
    cv::RNG rng(12345);  // Use same seed for reproducibility

    for (int size : template_sizes) {
        std::cout << "\nTesting template size: " << size << "x" << size << "\n";
        cv::Mat test_template(size, size, CV_64F);
        rng.fill(test_template, cv::RNG::UNIFORM, 0, 1);

        auto [result_chiimg, result_Wip2] = chiimg(large_img, test_template);
        saveResults(outFile, result_chiimg, result_Wip2, "full", size);
    }

    outFile.close();
    return 0;
}
