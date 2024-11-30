#include "findpeaks.h"
#include <algorithm>
#include <utility>

std::tuple<size_t, std::vector<size_t>, std::vector<size_t>> findPeaks(
    const std::vector<std::vector<double>>& img,
    const std::vector<std::vector<bool>>& mask,
    double cutOff,
    double minSep,
    bool useOneBasedIndexing) {

    const size_t NNx = img.size();
    const size_t NNy = img[0].size();

    // Create temporary comparison matrix
    std::vector<std::vector<bool>> tempComparison(NNx, std::vector<bool>(NNy, false));

    // First pass: find local maxima
    for (size_t i = 0; i < NNx; ++i) {
        for (size_t j = 0; j < NNy; ++j) {
            bool isPeak = true;
            // Check 8-connected neighborhood
            for (int di = -1; di <= 1 && isPeak; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    if (di == 0 && dj == 0) continue;

                    // Handle periodic boundary conditions
                    size_t ni = ((i + di + NNx) % NNx);
                    size_t nj = ((j + dj + NNy) % NNy);

                    if (img[i][j] <= img[ni][nj]) {
                        isPeak = false;
                        break;
                    }
                }
            }
            tempComparison[i][j] = isPeak;
        }
    }

    // Collect peaks that are above cutoff and within mask
    struct Peak {
        size_t x, y;
        double intensity;
    };
    std::vector<Peak> peaks;

    for (size_t i = 0; i < NNx; ++i) {
        for (size_t j = 0; j < NNy; ++j) {
            if (tempComparison[i][j] && mask[i][j] && img[i][j] > cutOff) {
                peaks.push_back({i, j, img[i][j]});
            }
        }
    }

    size_t Npf = peaks.size();

    if (Npf > 0) {
        // Sort peaks by intensity
        std::sort(peaks.begin(), peaks.end(),
                 [](const Peak& a, const Peak& b) {
                     return a.intensity > b.intensity;
                 });

        // Apply minimum separation
        std::vector<bool> keepPeaks(Npf, true);
        for (size_t i = 0; i < Npf; ++i) {
            if (keepPeaks[i]) {
                for (size_t j = i + 1; j < Npf; ++j) {
                    if (keepPeaks[j]) {
                        double dx = static_cast<double>(peaks[j].x) - static_cast<double>(peaks[i].x);
                        double dy = static_cast<double>(peaks[j].y) - static_cast<double>(peaks[i].y);
                        if (std::sqrt(dx*dx + dy*dy) < minSep) {
                            keepPeaks[j] = false;
                        }
                    }
                }
            }
        }

        // Collect final peaks
        std::vector<size_t> spx, spy;
        size_t finalNpf = 0;

        for (size_t i = 0; i < Npf; ++i) {
            if (keepPeaks[i]) {
                size_t x = peaks[i].x;
                size_t y = peaks[i].y;
                if (useOneBasedIndexing) {
                    x += 1;
                    y += 1;
                }
                spx.push_back(x);
                spy.push_back(y);
                finalNpf++;
            }
        }

        return {finalNpf, std::move(spx), std::move(spy)};
    }

    // No peaks found
    return {0, std::vector<size_t>(), std::vector<size_t>()};
}
