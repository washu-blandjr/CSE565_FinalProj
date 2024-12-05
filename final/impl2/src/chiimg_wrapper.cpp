#include "chiimg_wrapper.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <cstring>
#include <omp.h>

// Optimized conversion using OpenMP and continuous memory access
void convert_mat_to_array_img(const cv::Mat& mat, data_t* array) {
    const int rows = mat.rows;
    const int cols = mat.cols;
    
    #pragma omp parallel for collapse(2)
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            array[i * cols + j] = (data_t)mat.at<double>(i,j);
        }
    }
}

void convert_mat_to_array_template(const cv::Mat& mat, data_t* array) {
    const int rows = mat.rows;
    const int cols = mat.cols;
    const size_t total_size = rows * cols;
    
    // For small templates, use direct copy
    #pragma omp parallel for
    for(size_t i = 0; i < total_size; i++) {
        const int row = i / cols;
        const int col = i % cols;
        array[i] = (data_t)mat.at<double>(row, col);
    }
}

void convert_array_to_mat(const data_t* array, cv::Mat& mat, int rows, int cols) {
    mat.create(rows, cols, CV_64F);
    
    // Get continuous buffer if possible
    if(mat.isContinuous()) {
        double* mat_data = mat.ptr<double>(0);
        #pragma omp parallel for
        for(int i = 0; i < rows * cols; i++) {
            mat_data[i] = array[i].to_double();
        }
    } else {
        #pragma omp parallel for collapse(2)
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++) {
                mat.at<double>(i,j) = array[i * cols + j].to_double();
            }
        }
    }
}

// Static memory allocation to avoid repeated allocation/deallocation
class HLSMemoryManager {
private:
    static std::unique_ptr<data_t[]> img_buffer;
    static std::unique_ptr<data_t[]> ip_buffer;
    static std::unique_ptr<data_t[]> result_buffer;
    static std::unique_ptr<data_t[]> wip2_buffer;
    static std::mutex mutex;

public:
    static void initialize() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!img_buffer) {
            img_buffer = std::make_unique<data_t[]>(MAX_IMG_SIZE * MAX_IMG_SIZE);
            ip_buffer = std::make_unique<data_t[]>(MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE);
            result_buffer = std::make_unique<data_t[]>(MAX_IMG_SIZE * MAX_IMG_SIZE);
            wip2_buffer = std::make_unique<data_t[]>(MAX_IMG_SIZE * MAX_IMG_SIZE);
        }
    }

    static data_t* get_img_buffer() { return img_buffer.get(); }
    static data_t* get_ip_buffer() { return ip_buffer.get(); }
    static data_t* get_result_buffer() { return result_buffer.get(); }
    static data_t* get_wip2_buffer() { return wip2_buffer.get(); }
};

// Static member initialization
std::unique_ptr<data_t[]> HLSMemoryManager::img_buffer;
std::unique_ptr<data_t[]> HLSMemoryManager::ip_buffer;
std::unique_ptr<data_t[]> HLSMemoryManager::result_buffer;
std::unique_ptr<data_t[]> HLSMemoryManager::wip2_buffer;
std::mutex HLSMemoryManager::mutex;

std::tuple<cv::Mat, cv::Mat> chiimg_hls(
    const cv::Mat& img,
    const cv::Mat& ip,
    const cv::Mat& W,
    const cv::Mat& Wip2,
    const std::string& range
) {
    // Initialize static memory buffers
    HLSMemoryManager::initialize();
    
    // Get buffer pointers
    data_t* img_array = HLSMemoryManager::get_img_buffer();
    data_t* ip_array = HLSMemoryManager::get_ip_buffer();
    data_t* result_array = HLSMemoryManager::get_result_buffer();
    data_t* wip2_array = HLSMemoryManager::get_wip2_buffer();
    
    if (!img_array || !ip_array || !result_array || !wip2_array) {
        throw std::runtime_error("Failed to get memory buffers");
    }
    
    // Validate input dimensions
    if (img.rows > MAX_IMG_SIZE || img.cols > MAX_IMG_SIZE ||
        ip.rows > MAX_TEMPLATE_SIZE || ip.cols > MAX_TEMPLATE_SIZE) {
        throw std::runtime_error("Input dimensions exceed maximum allowed size");
    }
    
    // Convert OpenCV matrices to arrays (now using OpenMP)
    convert_mat_to_array_img(img, img_array);
    convert_mat_to_array_template(ip, ip_array);
    
    // Call HLS kernel
    chiimg_kernel(
        img_array, ip_array,
        img.rows, img.cols,
        ip.rows, ip.cols,
        result_array, wip2_array
    );
    
    // Convert results back to OpenCV matrices
    cv::Mat result_mat, wip2_mat;
    convert_array_to_mat(result_array, result_mat, img.rows, img.cols);
    convert_array_to_mat(wip2_array, wip2_mat, img.rows, img.cols);
    
    return std::make_tuple(result_mat, wip2_mat);
}