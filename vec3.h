#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {
    public:
        double e[3]; // O vetor são apenas 3 números: e[0], e[1], e[2]

        // Construtores: Cria um vetor vazio (0,0,0) ou com valores específicos
        vec3() : e{0,0,0} {}
        vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

        // Funções para pegar x, y, z (facilitadores)
        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        // Operador de Negação: -v
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        
        // Acesso direto aos índices: v[0]
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        // Operador += (v += u)
        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        // Operador *= por escalar (v *= 2)
        vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        // Operador /= por escalar (v /= 2)
        vec3& operator/=(const double t) {
            return *this *= 1/t;
        }

        // Comprimento (Norma) do vetor
        double length() const {
            return sqrt(length_squared());
        }

        // Comprimento ao quadrado (útil para otimização, evita raiz quadrada)
        double length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }
};

// --- Funções Utilitárias de Álgebra Linear ---

// Imprimir vetor: std::cout << v
inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// Soma: u + v
inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

// Subtração: u - v
inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

// Multiplicação componente a componente (Hadamard): u * v
inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

// Escalar * Vetor: t * v
inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

// Vetor * Escalar: v * t
inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

// Divisão por escalar: v / t
inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

// PRODUTO ESCALAR (Dot Product) - Essencial para iluminação!
inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

// PRODUTO VETORIAL (Cross Product) - Essencial para normais e câmeras!
inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

// Normalizar (retornar versor unitário)
inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

// Alias para facilitar a leitura: ponto3 e color são a mesma coisa que vec3
using point3 = vec3;   // Coordenadas 3D
using color = vec3;    // Cores RGB

#endif