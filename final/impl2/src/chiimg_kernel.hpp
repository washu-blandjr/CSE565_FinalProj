//chiimg_kernel.hpp
#ifndef CHIIMG_KERNEL_HPP
#define CHIIMG_KERNEL_HPP

#include <ap_fixed.h>

// Define fixed point data type
typedef ap_fixed<32,16> data_t;

// Maximum dimensions
#define MAX_IMG_SIZE 1024
#define MAX_TEMPLATE_SIZE 128

extern "C" {
void chiimg_kernel(
    data_t img[MAX_IMG_SIZE * MAX_IMG_SIZE],
    data_t ip[MAX_TEMPLATE_SIZE * MAX_TEMPLATE_SIZE],
    int img_rows, int img_cols,
    int ip_rows, int ip_cols,
    data_t result[MAX_IMG_SIZE * MAX_IMG_SIZE],
    data_t wip2_result[MAX_IMG_SIZE * MAX_IMG_SIZE]
);
}

#endif