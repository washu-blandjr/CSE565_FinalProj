#include "../../findpeaks.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// Helper function to create test image with Gaussian peaks
std::vector<std::vector<double>> createTestImage(size_t width, size_t height) {
    std::vector<std::vector<double>> img(width, std::vector<double>(height, 0.0));

    // Add three Gaussian peaks
    struct Peak { double x, y, amplitude, width; };
    std::vector<Peak> peaks = {
        {9.0, 9.0, 1.0, 2.0},    // Adjusted for 0-based indexing
        {19.0, 19.0, 0.8, 2.0},  // These positions will create peaks at
        {29.0, 29.0, 0.6, 2.0}   // the same locations as MATLAB
    };

    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            for (const auto& p : peaks) {
                double dx = i - p.x;
                double dy = j - p.y;
                img[i][j] += p.amplitude * std::exp(-(dx*dx + dy*dy)/(2.0*p.width*p.width));
            }
        }
    }
    return img;
}

int main() {
    std::cout << "C++ TEST:" << std::endl;
    // Create test image
    const size_t width = 40;
    const size_t height = 40;
    auto img = createTestImage(width, height);

    // Create mask (all true)
    std::vector<std::vector<bool>> mask(width, std::vector<bool>(height, true));

    // Test parameters
    double cutOff = 0.3;
    double minSep = 5.0;

    // Run findPeaks with one-based indexing
    auto [numPeaks, peakX, peakY] = findPeaks(img, mask, cutOff, minSep, true);

    // Print results (no need to add 1 anymore since we're using one-based indexing)
    std::cout << "Number of peaks found: " << numPeaks << std::endl;
    for (size_t i = 0; i < numPeaks; ++i) {
        std::cout << "Peak " << i + 1 << ": position ("
                  << peakX[i] << ", " << peakY[i] << "), intensity "
                  << std::fixed << std::setprecision(6) << img[peakX[i]-1][peakY[i]-1]
                  << std::endl;
    }

    std::cout << std::endl << "Peak locations:" << std::endl;
    for (size_t i = 0; i < numPeaks; ++i) {
        std::cout << "[" << peakX[i] << ", " << peakY[i] << "]" << std::endl;
    }

    return 0;
}
