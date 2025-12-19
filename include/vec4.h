#ifndef VEC4_H
#define VEC4_H

#include "vec3.h"

class vec4 {
    public:
        double e[4];

        vec4() : e{0,0,0,0} {}
        vec4(double e0, double e1, double e2, double e3) : e{e0, e1, e2, e3} {}
        
        // Construtor a partir de vec3 e w
        // W=1 para Pontos (são afetados por translação)
        // W=0 para Vetores (direções, NÃO são afetados por translação)
        vec4(const vec3& v, double w) : e{v.x(), v.y(), v.z(), w} {}

        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }
        double w() const { return e[3]; }

        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }

        // Converte de volta para vec3 dividindo por W (Perspectiva/Homogeneização)
        vec3 to_vec3() const {
            // Evita divisão por zero
            if (e[3] != 0 && e[3] != 1.0) {
                return vec3(e[0]/e[3], e[1]/e[3], e[2]/e[3]);
            }
            return vec3(e[0], e[1], e[2]);
        }
};

inline double dot(const vec4 &u, const vec4 &v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2] + u.e[3] * v.e[3];
}

#endif