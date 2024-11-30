// chiimg.cpp
#include "chiimg.h"

std::tuple<cv::Mat, cv::Mat> chiimg(
    const cv::Mat& img,
    const cv::Mat& ip,
    const cv::Mat& W,
    const cv::Mat& Wip2,
    const std::string& range
) {
    cv::Mat W_used = W.empty() ? ip : W;
    cv::Mat result_Wip2;

    // Calculate Wip2 if not provided
    if (Wip2.empty()) {
        cv::Mat blk = cv::Mat::ones(img.size(), CV_64F);
        cv::Mat ip_squared;
        cv::multiply(ip, ip, ip_squared);
        cv::multiply(ip_squared, W_used, ip_squared);

        // Manual convolution for Wip2 to match MATLAB exactly
        int m = blk.rows + ip_squared.rows - 1;
        int n = blk.cols + ip_squared.cols - 1;
        result_Wip2 = cv::Mat::zeros(m, n, CV_64F);

        for(int i = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                double sum = 0;
                for(int k = 0; k < ip_squared.rows; k++) {
                    for(int l = 0; l < ip_squared.cols; l++) {
                        if(i-k >= 0 && i-k < blk.rows && j-l >= 0 && j-l < blk.cols) {
                            sum += ip_squared.at<double>(k,l) * blk.at<double>(i-k,j-l);
                        }
                    }
                }
                result_Wip2.at<double>(i,j) = sum;
            }
        }
    } else {
        result_Wip2 = Wip2;
    }

    // Flip ip and W for convolution
    cv::Mat flipped_ip, flipped_W;
    cv::flip(ip, flipped_ip, -1);
    cv::flip(W_used, flipped_W, -1);

    // Calculate chi-squared image
    cv::Mat ip_W;
    cv::multiply(flipped_ip, flipped_W, ip_W);

    cv::Mat img_squared;
    cv::multiply(img, img, img_squared);

    int m = img.rows + ip_W.rows - 1;
    int n = img.cols + ip_W.cols - 1;
    cv::Mat term1 = cv::Mat::zeros(m, n, CV_64F);
    cv::Mat term2 = cv::Mat::zeros(m, n, CV_64F);

    // Manual convolution for term1 and term2
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            double sum1 = 0, sum2 = 0;
            for(int k = 0; k < ip_W.rows; k++) {
                for(int l = 0; l < ip_W.cols; l++) {
                    if(i-k >= 0 && i-k < img.rows && j-l >= 0 && j-l < img.cols) {
                        sum1 += ip_W.at<double>(k,l) * img.at<double>(i-k,j-l);
                        sum2 += flipped_W.at<double>(k,l) * img_squared.at<double>(i-k,j-l);
                    }
                }
            }
            term1.at<double>(i,j) = sum1;
            term2.at<double>(i,j) = sum2;
        }
    }

    cv::Mat chiimg_result = 1.0 + (-2.0 * term1 + term2) / result_Wip2;

    // Set boundary values to 0.2
    chiimg_result(cv::Rect(0, 0, n, 2)) = 0.2;
    chiimg_result(cv::Rect(0, m-2, n, 2)) = 0.2;
    chiimg_result(cv::Rect(0, 0, 2, m)) = 0.2;
    chiimg_result(cv::Rect(n-2, 0, 2, m)) = 0.2;

    return std::make_tuple(chiimg_result, result_Wip2);
}
