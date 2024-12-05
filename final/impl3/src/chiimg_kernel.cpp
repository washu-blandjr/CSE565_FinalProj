// chiimg_kernel.cpp
#include "chiimg_kernel.hpp"

static void conv2_kernel(
    data_t A[MAX_IMG_SIZE * MAX_IMG_SIZE],
    data_t B[MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE],
    int A_rows, int A_cols,
    int B_rows, int B_cols,
    data_t result[MAX_IMG_SIZE * MAX_IMG_SIZE])
{
    int m = A_rows + B_rows - 1;
    int n = A_cols + B_cols - 1;

    // Initialize result matrix
    for (int i = 0; i < m * n; i++)
    {
        result[i] = 0;
    }

    // Convolution computation
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            data_t sum = 0;
            for (int k = 0; k < B_rows; k++)
            {
                for (int l = 0; l < B_cols; l++)
                {
                    if (i - k >= 0 && i - k < A_rows && j - l >= 0 && j - l < A_cols)
                    {
                        sum += B[k * B_cols + l] * A[(i - k) * A_cols + (j - l)];
                    }
                }
            }
            result[i * n + j] = sum;
        }
    }
}

extern "C"
{
    void chiimg_kernel(
        data_t img[MAX_IMG_SIZE * MAX_IMG_SIZE],
        data_t ip[MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE],
        int img_rows, int img_cols,
        int ip_rows, int ip_cols,
        data_t result[MAX_IMG_SIZE * MAX_IMG_SIZE],
        data_t wip2_result[MAX_IMG_SIZE * MAX_IMG_SIZE])
    {
        // Local arrays for processing
        data_t ip_squared[MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE];

// Calculate ip_squared
#pragma HLS PIPELINE II = 1
        for (int i = 0; i < ip_rows * ip_cols; i++)
        {
            ip_squared[i] = ip[i] * ip[i];
        }

// Calculate Wip2
#pragma HLS DATAFLOW
        conv2_kernel(img, ip_squared, img_rows, img_cols, ip_rows, ip_cols, wip2_result);

// Calculate chi-squared image
#pragma HLS DATAFLOW
        conv2_kernel(img, ip, img_rows, img_cols, ip_rows, ip_cols, result);

// Final computation and boundary handling
#pragma HLS PIPELINE II = 1
        for (int i = 0; i < img_rows; i++)
        {
            for (int j = 0; j < img_cols; j++)
            {
                int idx = i * img_cols + j;
                data_t term1 = result[idx];
                data_t img_val = img[idx];
                data_t term2 = img_val * img_val;
                data_t wip2_val = wip2_result[idx];

                if (i < 2 || i >= img_rows - 2 || j < 2 || j >= img_cols - 2)
                {
                    result[idx] = data_t(0.2);
                }
                else
                {
                    result[idx] = data_t(1.0) + (data_t(-2.0) * term1 + term2) / wip2_val;
                }
            }
        }
    }
}