#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include <memory> // Necessário para smart pointers

class material; // "Forward declaration": avisa que a classe material vai existir no futuro

struct hit_record {
    point3 p;         // Ponto onde o raio bateu
    vec3 normal;      // O vetor normal nesse ponto
    std::shared_ptr<material> mat_ptr; // Do que é feito esse objeto?
    double t;         // A distância t onde bateu
    
    // Coordenadas de Textura (Requisito 1.3.3)
    double u;
    double v;
    
    bool front_face;  // True se o raio bateu de fora, False se bateu de dentro

    // Define a normal sempre contra o raio
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Se o produto escalar é negativo, eles são opostos (raio entra na face)
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
    public:
        // Função virtual pura: obriga as filhas (Sphere, Cone, Mesh) a implementarem
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif