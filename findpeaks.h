#pragma once

#include <vector>
#include <tuple>
#include <cmath>

/**
 * @brief Find intensity peaks in an image
 * @param img Input image as 2D vector
 * @param mask Binary mask as 2D vector
 * @param cutOff Minimum intensity threshold for peaks
 * @param minSep Minimum separation between peaks in pixels
 * @param useOneBasedIndexing If true, returned coordinates will use 1-based indexing
 * @return Tuple containing:
 *         - number of peaks found
 *         - vector of x coordinates of peaks
 *         - vector of y coordinates of peaks
 */
std::tuple<size_t, std::vector<size_t>, std::vector<size_t>> findPeaks(
    const std::vector<std::vector<double>>& img,
    const std::vector<std::vector<bool>>& mask,
    double cutOff,
    double minSep,
    bool useOneBasedIndexing = false);
