// chiimg.cpp
#include "chiimg.h"
#include "conv2.h"

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
        result_Wip2 = conv2(blk, ip_squared, range);
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

    cv::Mat term1 = conv2(img, ip_W, range);
    cv::Mat term2 = conv2(img_squared, flipped_W, range);

    cv::Mat chiimg_result = 1.0 + (-2.0 * term1 + term2) / result_Wip2;

    // Set boundary values to 0.2
    int m = chiimg_result.rows;
    int n = chiimg_result.cols;
    chiimg_result(cv::Rect(0, 0, n, 2)) = 0.2;
    chiimg_result(cv::Rect(0, m-2, n, 2)) = 0.2;
    chiimg_result(cv::Rect(0, 0, 2, m)) = 0.2;
    chiimg_result(cv::Rect(n-2, 0, 2, m)) = 0.2;

    return std::make_tuple(chiimg_result, result_Wip2);
}
