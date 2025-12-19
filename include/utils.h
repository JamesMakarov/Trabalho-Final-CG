#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>
#include <random>

// Constantes Matemáticas
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Conversão Graus -> Radianos
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

// --- Geração de Números Aleatórios ---

// Retorna um real aleatório em [0, 1)
inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

// Retorna um real aleatório em [min, max)
inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

// Clamping: garante que x fique entre min e max
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

#endif