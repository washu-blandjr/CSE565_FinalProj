// conv2.cpp
#include "conv2.h"

cv::Mat conv2(const cv::Mat& A, const cv::Mat& B, const std::string& mode) {
    int m = A.rows + B.rows - 1;
    int n = A.cols + B.cols - 1;
    cv::Mat result = cv::Mat::zeros(m, n, CV_64F);

    // Perform full convolution
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            double sum = 0;
            for(int k = 0; k < B.rows; k++) {
                for(int l = 0; l < B.cols; l++) {
                    if(i-k >= 0 && i-k < A.rows && j-l >= 0 && j-l < A.cols) {
                        sum += B.at<double>(k,l) * A.at<double>(i-k,j-l);
                    }
                }
            }
            result.at<double>(i,j) = sum;
        }
    }

    // Handle different output modes
    if (mode == "valid") {
        int valid_rows = std::max(A.rows - std::max(0, B.rows - 1), 0);
        int valid_cols = std::max(A.cols - std::max(0, B.cols - 1), 0);
        if (valid_rows > 0 && valid_cols > 0) {
            cv::Rect valid_roi(
                (result.cols - valid_cols) / 2,
                (result.rows - valid_rows) / 2,
                valid_cols,
                valid_rows
            );
            result = result(valid_roi);
        }
    }
    else if (mode == "same") {
        cv::Rect same_roi(
            (result.cols - A.cols) / 2,
            (result.rows - A.rows) / 2,
            A.cols,
            A.rows
        );
        result = result(same_roi);
    }

    return result;
}
