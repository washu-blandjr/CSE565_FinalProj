#include "chiimg_wrapper.hpp"
#include <iostream>
#include <memory>

void convert_mat_to_array_img(const cv::Mat& mat, data_t* array) {
    for(int i = 0; i < mat.rows; i++) {
        for(int j = 0; j < mat.cols; j++) {
            array[i * mat.cols + j] = (data_t)mat.at<double>(i,j);
        }
    }
}

void convert_mat_to_array_template(const cv::Mat& mat, data_t* array) {
    for(int i = 0; i < mat.rows; i++) {
        for(int j = 0; j < mat.cols; j++) {
            array[i * mat.cols + j] = (data_t)mat.at<double>(i,j);
        }
    }
}

void convert_array_to_mat(const data_t* array, cv::Mat& mat, int rows, int cols) {
    mat = cv::Mat(rows, cols, CV_64F);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            mat.at<double>(i,j) = array[i * cols + j].to_double();
        }
    }
}

std::tuple<cv::Mat, cv::Mat> chiimg_hls(
    const cv::Mat& img,
    const cv::Mat& ip,
    const cv::Mat& W,
    const cv::Mat& Wip2,
    const std::string& range
) {
    // Allocate arrays on the heap using unique_ptr for automatic cleanup
    auto img_array = std::make_unique<data_t[]>(MAX_IMG_SIZE * MAX_IMG_SIZE);
    auto ip_array = std::make_unique<data_t[]>(MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE);
    auto result_array = std::make_unique<data_t[]>(MAX_IMG_SIZE * MAX_IMG_SIZE);
    auto wip2_array = std::make_unique<data_t[]>(MAX_IMG_SIZE * MAX_IMG_SIZE);
    
    if (!img_array || !ip_array || !result_array || !wip2_array) {
        throw std::runtime_error("Failed to allocate memory for arrays");
    }
    
    // Convert OpenCV matrices to arrays
    convert_mat_to_array_img(img, img_array.get());
    convert_mat_to_array_template(ip, ip_array.get());
    
    // Call HLS kernel
    chiimg_kernel(
        img_array.get(), ip_array.get(),
        img.rows, img.cols,
        ip.rows, ip.cols,
        result_array.get(), wip2_array.get()
    );
    
    // Convert results back to OpenCV matrices
    cv::Mat result_mat, wip2_mat;
    convert_array_to_mat(result_array.get(), result_mat, img.rows, img.cols);
    convert_array_to_mat(wip2_array.get(), wip2_mat, img.rows, img.cols);
    
    return std::make_tuple(result_mat, wip2_mat);
}