#pragma once

#include <opencv2/opencv.hpp>
#include <string>

// Performs 2D convolution similar to MATLAB's conv2
cv::Mat conv2(const cv::Mat& A, const cv::Mat& B, const std::string& mode = "full");
