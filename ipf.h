#pragma once
#include <vector>
#include <cmath>

/**
 * @brief Calculate ideal particle image
 * @param cr Distance from particle center
 * @param D Particle diameter
 * @param w Width parameter (2w is the width of 76% of the fall off)
 * @return Vector containing the ideal particle image values
 */
std::vector<double> ipf(const std::vector<double>& cr, double D, double w);
