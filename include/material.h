#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"
#include "hittable.h" // Para hit_record

struct hit_record;

// --- Funções Auxiliares de Óptica e Vetores Aleatórios ---

// Gera um vetor aleatório dentro da esfera unitária (para reflexão difusa)
inline vec3 random_in_unit_sphere() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), random_double(-1,1));
        if (p.length_squared() >= 1) continue;
        return p;
    }
}

// Retorna um vetor unitário aleatório (Lambertian moderno)
inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

// Calcula a reflexão especular: v - 2*dot(v,n)*n
inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n;
}

// Calcula a refração (Lei de Snell)
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

// Aproximação de Schlick para refletância (Vidro visto de ângulo reflete mais)
inline double reflectance(double cosine, double ref_idx) {
    auto r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine), 5);
}


// --- Classe Abstrata Material ---
class material {
    public:
        // Scatter: dado um raio incidente (r_in) e onde bateu (rec),
        // calcula a cor de atenuação e o novo raio espalhado (scattered)
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;
};

// 1. Material Fosco (Lambertian)
class lambertian : public material {
    public:
        color albedo; // A cor do material

        lambertian(const color& a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            // Direção de espalhamento aleatória (Normal + Vetor Aleatório)
            auto scatter_direction = rec.normal + random_unit_vector();

            // Proteção contra vetor nulo (se random for exatamente oposto à normal)
            if (scatter_direction.length_squared() < 1e-8)
                scatter_direction = rec.normal;

            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }
};

// 2. Material Metálico
class metal : public material {
    public:
        color albedo;
        double fuzz; // 0 = Espelho perfeito, 1 = Muito fosco

        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            // Raio refletido + pequena aleatoriedade (fuzz)
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            
            // Só reflete se estiver apontando para fora da superfície
            return (dot(scattered.direction(), rec.normal) > 0);
        }
};

// 3. Material Dielétrico (Vidro, Água, Diamante)
class dielectric : public material {
    public:
        double ir; // Índice de Refração (Vidro = 1.5, Água = 1.33, Ar = 1.0)

        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            attenuation = color(1.0, 1.0, 1.0); // Vidro não absorve luz (geralmente)
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            vec3 unit_direction = unit_vector(r_in.direction());
            
            // Verifica Reflexão Total Interna (quando o raio não consegue sair do vidro)
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = ray(rec.p, direction);
            return true;
        }
};

#endif