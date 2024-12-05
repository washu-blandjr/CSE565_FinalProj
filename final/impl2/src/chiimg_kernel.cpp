#include "chiimg_kernel.hpp"

static void conv2_kernel(
    data_t A[MAX_IMG_SIZE * MAX_IMG_SIZE],
    data_t B[MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE],
    int A_rows, int A_cols,
    int B_rows, int B_cols,
    data_t result[MAX_IMG_SIZE * MAX_IMG_SIZE]
) {    
    #pragma HLS INLINE OFF
    
    int m = A_rows + B_rows - 1;
    int n = A_cols + B_cols - 1;
    
    // Partition the B array for parallel access
    #pragma HLS ARRAY_PARTITION variable=B complete dim=1
    
    // Local buffer for A to improve memory access
    data_t A_buf[MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE];
    #pragma HLS ARRAY_PARTITION variable=A_buf complete dim=1
    
    // Initialize result matrix
    init_result: for(int i = 0; i < m * n; i++) {
        #pragma HLS PIPELINE II=1
        result[i] = 0;
    }
    
    // Convolution computation with optimized memory access
    conv_outer: for(int i = 0; i < m; i++) {
        conv_inner: for(int j = 0; j < n; j++) {
            #pragma HLS PIPELINE II=1
            
            data_t sum = 0;
            
            // Load relevant part of A into buffer
            load_buffer: for(int k = 0; k < B_rows; k++) {
                for(int l = 0; l < B_cols; l++) {
                    #pragma HLS UNROLL
                    if(i-k >= 0 && i-k < A_rows && j-l >= 0 && j-l < A_cols) {
                        A_buf[k * B_cols + l] = A[(i-k) * A_cols + (j-l)];
                    } else {
                        A_buf[k * B_cols + l] = 0;
                    }
                }
            }
            
            // Compute convolution using buffered data
            conv_compute: for(int k = 0; k < B_rows; k++) {
                #pragma HLS UNROLL factor=4
                for(int l = 0; l < B_cols; l++) {
                    #pragma HLS UNROLL factor=4
                    sum += B[k * B_cols + l] * A_buf[k * B_cols + l];
                }
            }
            
            result[i * n + j] = sum;
        }
    }
}

extern "C" {
void chiimg_kernel(
    data_t img[MAX_IMG_SIZE * MAX_IMG_SIZE],
    data_t ip[MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE],
    int img_rows, int img_cols,
    int ip_rows, int ip_cols,
    data_t result[MAX_IMG_SIZE * MAX_IMG_SIZE],
    data_t wip2_result[MAX_IMG_SIZE * MAX_IMG_SIZE]
) {    
    #pragma HLS INTERFACE m_axi port=img bundle=gmem0
    #pragma HLS INTERFACE m_axi port=ip bundle=gmem1
    #pragma HLS INTERFACE m_axi port=result bundle=gmem2
    #pragma HLS INTERFACE m_axi port=wip2_result bundle=gmem3
    
    #pragma HLS INTERFACE s_axilite port=img_rows bundle=control
    #pragma HLS INTERFACE s_axilite port=img_cols bundle=control
    #pragma HLS INTERFACE s_axilite port=ip_rows bundle=control
    #pragma HLS INTERFACE s_axilite port=ip_cols bundle=control
    #pragma HLS INTERFACE s_axilite port=img bundle=control
    #pragma HLS INTERFACE s_axilite port=ip bundle=control
    #pragma HLS INTERFACE s_axilite port=result bundle=control
    #pragma HLS INTERFACE s_axilite port=wip2_result bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
    // Local arrays for processing with better memory access
    data_t ip_squared[MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE];
    #pragma HLS ARRAY_PARTITION variable=ip_squared complete dim=1
    
    // Calculate ip_squared with pipeline
    square_ip: for(int i = 0; i < ip_rows * ip_cols; i++) {
        #pragma HLS PIPELINE II=1
        ip_squared[i] = ip[i] * ip[i];
    }
    
    // Calculate Wip2 and chi-squared image
    conv2_kernel(img, ip_squared, img_rows, img_cols, ip_rows, ip_cols, wip2_result);
    conv2_kernel(img, ip, img_rows, img_cols, ip_rows, ip_cols, result);
    
    // Final computation and boundary handling
    process_results: for(int i = 0; i < img_rows; i++) {
        #pragma HLS PIPELINE II=1
        process_cols: for(int j = 0; j < img_cols; j++) {
            #pragma HLS UNROLL factor=4
            
            int idx = i * img_cols + j;
            data_t term1 = result[idx];
            data_t img_val = img[idx];
            data_t term2 = img_val * img_val;
            data_t wip2_val = wip2_result[idx];
            
            if(i < 2 || i >= img_rows-2 || j < 2 || j >= img_cols-2) {
                result[idx] = data_t(0.2);
            } else {
                result[idx] = data_t(1.0) + (data_t(-2.0) * term1 + term2) / wip2_val;
            }
        }
    }
}
}