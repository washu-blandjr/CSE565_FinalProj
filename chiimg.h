#pragma once
#include <opencv2/opencv.hpp>
#include <tuple>

std::tuple<cv::Mat, cv::Mat> chiimg(
    const cv::Mat& img,
    const cv::Mat& ip,
    const cv::Mat& W = cv::Mat(),
    const cv::Mat& Wip2 = cv::Mat(),
    const std::string& range = "full"
);
