// chiimg_wrapper.hpp
#ifndef CHIIMG_WRAPPER_HPP
#define CHIIMG_WRAPPER_HPP

#include <opencv2/core.hpp>
#include <string>
#include "chiimg_kernel.hpp"

void convert_mat_to_array_img(const cv::Mat& mat, data_t* array);
void convert_mat_to_array_template(const cv::Mat& mat, data_t* array);
void convert_array_to_mat(const data_t* array, cv::Mat& mat, int rows, int cols);

std::tuple<cv::Mat, cv::Mat> chiimg_hls(
    const cv::Mat& img,
    const cv::Mat& ip,
    const cv::Mat& W,
    const cv::Mat& Wip2,
    const std::string& range
);

#endif