#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include "../../chiimg.h"

// Function to save matrix to file
void saveMatrixToFile(const cv::Mat& mat, std::ofstream& file) {
    file << mat.rows << " " << mat.cols << "\n";
    for(int i = 0; i < mat.rows; i++) {
        for(int j = 0; j < mat.cols; j++) {
            file << mat.at<double>(i,j) << " ";
        }
        file << "\n";
    }
}

// Function to read matrix from file
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

// Function to save results to output file
void saveResults(const std::string& filename, const cv::Mat& chiimg_result, const cv::Mat& wip2_result) {
    std::ofstream outFile(filename);
    outFile.precision(10); // Set precision for floating-point numbers

    outFile << "CHIIMG Result:\n";
    saveMatrixToFile(chiimg_result, outFile);
    outFile << "WIP2 Result:\n";
    saveMatrixToFile(wip2_result, outFile);
}

int main() {
    const std::string inputFile = "input.txt";
    const std::string outputFile = "output.txt";

    // Check if input file exists
    std::ifstream inFile(inputFile);
    cv::Mat large_img, template_img;

    if (!inFile.good()) {
        std::cout << "Generating new input file...\n";

        // Generate and save random matrices
        const int LARGE_IMAGE_SIZE = 1000;
        const int TEMPLATE_SIZE = 100;

        large_img = cv::Mat(LARGE_IMAGE_SIZE, LARGE_IMAGE_SIZE, CV_64F);
        template_img = cv::Mat(TEMPLATE_SIZE, TEMPLATE_SIZE, CV_64F);

        // Use constant seed for reproducibility
        cv::RNG rng(12345);
        rng.fill(large_img, cv::RNG::UNIFORM, 0, 1);
        rng.fill(template_img, cv::RNG::UNIFORM, 0, 1);

        // Save matrices to input file
        std::ofstream outFile(inputFile);
        outFile.precision(10);
        outFile << "Large Image:\n";
        saveMatrixToFile(large_img, outFile);
        outFile << "Template Image:\n";
        saveMatrixToFile(template_img, outFile);
        outFile.close();
    } else {
        std::cout << "Reading from existing input file...\n";
        std::string header;
        std::getline(inFile, header); // Read "Large Image:" header
        large_img = readMatrixFromFile(inFile);
        std::getline(inFile, header); // Read empty line
        std::getline(inFile, header); // Read "Template Image:" header
        template_img = readMatrixFromFile(inFile);
    }
    inFile.close();

    std::cout << "Running chiimg with input matrices...\n";
    auto start_time = std::chrono::high_resolution_clock::now();

    // Run chiimg with the matrices
    auto [result_chiimg, result_Wip2] = chiimg(large_img, template_img);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    // Save results
    saveResults(outputFile, result_chiimg, result_Wip2);

    // Print performance stats
    double seconds = duration.count() / 1000000.0;
    std::cout << "\nPerformance Stats:\n";
    std::cout << "Total time: " << seconds << " seconds\n";
    std::cout << "Image size: " << large_img.size() << "\n";
    std::cout << "Template size: " << template_img.size() << "\n";

    return 0;
}
