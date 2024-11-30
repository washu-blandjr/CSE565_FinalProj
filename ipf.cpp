#include "ipf.h"

std::vector<double> ipf(const std::vector<double>& cr, double D, double w) {
    std::vector<double> ipi(cr.size());

    for (size_t i = 0; i < cr.size(); ++i) {
        // Calculate (1-tanh((abs(cr)-D/2)/w))/2 for each element
        ipi[i] = (1.0 - std::tanh((std::abs(cr[i]) - D/2.0)/w))/2.0;
    }

    return ipi;
}
