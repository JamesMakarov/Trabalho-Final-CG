#ifndef MATERIAL_H
#define MATERIAL_H

#include "utils.h"
#include "vec3.h"
#include "texture.h"

// Estrutura para Material Phong (Ray Casting Clássico)
class material {
    public:
        shared_ptr<texture> kd; // Cor Difusa (A cor do objeto)
        vec3 ks;                // Cor Especular (O brilho branco/colorido da luz)
        double ka;              // Coeficiente Ambiental (quanto ele "brilha" no escuro)
        double shininess;       // Brilho (Ex: 32 para plástico, 200 para metal)

        // Construtor Simples (Cor sólida)
        material(color color_diffuse, double k_ambient=0.1, double k_shine=30.0, vec3 color_spec=vec3(1,1,1))
            : kd(make_shared<solid_color>(color_diffuse)), ks(color_spec), ka(k_ambient), shininess(k_shine) {}

        // Construtor Textura
        material(shared_ptr<texture> texture_diffuse, double k_ambient=0.1, double k_shine=30.0, vec3 color_spec=vec3(1,1,1))
            : kd(texture_diffuse), ks(color_spec), ka(k_ambient), shininess(k_shine) {}
};

#endif