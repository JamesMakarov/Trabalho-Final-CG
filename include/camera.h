#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"
#include "vec3.h"
#include "ray.h"

class camera {
    public:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w; // Vetores da base da câmera (Direita, Cima, Trás)
        double lens_radius;

        // Construtor Completo
        // lookfrom: Onde está o olho
        // lookat:   Para onde estamos olhando
        // vup:      Qual lado é "pra cima" (geralmente 0,1,0)
        // vfov:     Campo de visão vertical em graus (Zoom)
        // aspect:   Razão de aspecto (largura/altura)
        // aperture: Abertura da lente (0 para pinhole perfeito, >0 para desfoque de profundidade)
        // focus_dist: Distância de foco
        camera(
            point3 lookfrom,
            point3 lookat,
            vec3   vup,
            double vfov, 
            double aspect_ratio,
            double aperture = 0.0,
            double focus_dist = 10.0
        ) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            // Criação da Base Ortonormal (Espaço da Câmera)
            w = unit_vector(lookfrom - lookat); // Vetor apontando para trás (do alvo para a cam)
            u = unit_vector(cross(vup, w));     // Vetor apontando para a direita
            v = cross(w, u);                    // Vetor apontando para cima (local)

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            
            // O canto inferior esquerdo é calculado relativo à rotação da câmera
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            lens_radius = aperture / 2;
        }

        // Gera um raio para as coordenadas de textura (s, t) da tela
        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();

            // Se lens_radius for 0 (pinhole), o offset é (0,0,0)
            return ray(
                origin + offset,
                lower_left_corner + s*horizontal + t*vertical - origin - offset
            );
        }

    private:
        // Gera ponto aleatório num disco (para simular abertura de lente/defocus)
        static vec3 random_in_unit_disk() {
            while (true) {
                auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
                if (p.length_squared() >= 1) continue;
                return p;
            }
        }
};

#endif