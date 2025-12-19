#include "../include/utils.h"
#include "../include/hittable_list.h"
#include "../include/sphere.h"
#include "../include/cylinder.h"
#include "../include/cone.h"
#include "../include/mesh.h"
#include "../include/camera.h"
#include "../include/material.h"     // Agora usa o modelo Phong
#include "../include/instance.h"
#include "../include/texture.h"

#include <iostream>

// --- Definição de Luz (Requisito 1.5 - Pontual) ---
struct PointLight {
    point3 position;
    color intensity;
};

// --- Função Ray Casting (Blinn-Phong) ---
// Não é recursiva! Calcula a cor baseada na luz direta.
color ray_color(const ray& r, const hittable& world, const PointLight& light) {
    hit_record rec;

    // 1. O raio bateu em algo?
    if (world.hit(r, 0.001, infinity, rec)) {
        
        // Dados do Material
        color color_diffuse = rec.mat_ptr->kd->value(rec.u, rec.v, rec.p);
        vec3 color_specular = rec.mat_ptr->ks;
        double shininess = rec.mat_ptr->shininess;

        // A. Componente Ambiental (Luz base constante)
        color ambient = rec.mat_ptr->ka * color_diffuse;

        // Vetores Importantes para iluminação
        vec3 light_dir = unit_vector(light.position - rec.p); // L
        vec3 view_dir = unit_vector(-r.direction());          // V
        vec3 normal = unit_vector(rec.normal);                // N

        // B. Verificação de Sombra (Shadow Ray) - Requisito 4
        // Lança um raio do ponto de impacto em direção à luz
        ray shadow_ray(rec.p + 0.001*normal, light_dir); // +0.001 para evitar "acne"
        hit_record shadow_rec;
        
        // Distância até a luz (para não bater em objetos atrás da luz)
        double light_dist = (light.position - rec.p).length();

        // Se bater em algo no caminho da luz -> Está na sombra!
        if (world.hit(shadow_ray, 0.001, light_dist, shadow_rec)) {
            return ambient; // Na sombra, só tem luz ambiente
        }

        // C. Componente Difusa (Lambert)
        // Quanto a face está virada para a luz (N dot L)
        double diff = fmax(dot(normal, light_dir), 0.0);
        color diffuse = diff * color_diffuse;

        // D. Componente Especular (Blinn-Phong)
        // Brilho onde a luz reflete na direção do olho
        vec3 halfway_dir = unit_vector(light_dir + view_dir); // H
        double spec = pow(fmax(dot(normal, halfway_dir), 0.0), shininess);
        color specular = spec * color_specular;

        // Cor Final = Ambiente + (Difusa + Especular) * Intensidade da Luz
        return ambient + (diffuse + specular) * light.intensity;
    }

    // Fundo (Background)
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    // --- Configurações ---
    double zoom_vfov = 40.0; 
    const auto aspect_ratio = 1.0; 
    const int image_width = 500;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 20; // Antialiasing ainda é permitido e bom

    // --- Mundo ---
    hittable_list world;

    // Textura Xadrez
    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    // Materiais Phong (Sem reflexão de espelho, apenas brilho)
    auto mat_floor  = make_shared<material>(checker, 0.1, 10.0); // Chão fosco
    auto mat_gold   = make_shared<material>(color(0.8, 0.6, 0.2), 0.2, 128.0, color(1, 0.9, 0.5)); // Ouro brilhante
    auto mat_blue   = make_shared<material>(color(0.1, 0.2, 0.5), 0.1, 64.0);  // Plástico Azul
    auto mat_silver = make_shared<material>(color(0.7, 0.7, 0.7), 0.1, 200.0, color(1,1,1)); // Metal Prata
    auto mat_ruby   = make_shared<material>(color(0.9, 0.1, 0.1), 0.2, 100.0); // Rubi

    // --- Objetos ---
    // 1. Chão
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, mat_floor));

    // 2. Altar (Cilindro Ouro) - Transformado
    auto cyl_base = make_shared<cylinder>(3.0, 1.5, mat_gold);
    mat4 cyl_pos = mat4::translate(vec3(0, 1.5, 0));
    mat4 cyl_inv = mat4::translate(vec3(0, -1.5, 0));
    world.add(make_shared<instance>(cyl_base, cyl_pos, cyl_inv));

    // 3. Esfera "Simples" (Não é mais vidro, é Rubi Brilhante) - Em cima do altar
    world.add(make_shared<sphere>(point3(0, 4.0, 0), 1.0, mat_ruby));

    // 4. Cone (Prata) - À direita
    auto cone_base = make_shared<cone>(4.0, 1.0, mat_silver);
    mat4 cone_pos = mat4::translate(vec3(4, 0, 0));
    mat4 cone_inv = mat4::translate(vec3(-4, 0, 0));
    world.add(make_shared<instance>(cone_base, cone_pos, cone_inv));

    // 5. Cubo (Azul) - À esquerda e girado
    auto box_base = make_shared<box_mesh>(point3(0,0,0), point3(1,1,1), mat_blue);
    mat4 box_trans = mat4::translate(vec3(-4, 1, 1)) * mat4::rotate_y(degrees_to_radians(45));
    // Inversa simplificada (na mão ou assumindo biblioteca, aqui fiz manual a translação pra simplificar)
    // Para simplificar a inversa da composta: (T*R)^-1 = R^-1 * T^-1
    mat4 box_inv = mat4::rotate_y(degrees_to_radians(-45)) * mat4::translate(vec3(4, -1, -1)); 
    world.add(make_shared<instance>(box_base, box_trans, box_inv));


    // --- Luz (Requisito 1.5) ---
    // Uma luz forte vindo de cima e da direita
    PointLight main_light;
    main_light.position = point3(10, 20, 10);
    main_light.intensity = color(1.0, 1.0, 1.0); // Luz branca pura


    // --- Câmera ---
    point3 lookfrom(0, 8, 12);
    point3 lookat(0, 2, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = (lookfrom - lookat).length();
    
    camera cam(lookfrom, lookat, vup, zoom_vfov, aspect_ratio, 0.0, dist_to_focus); // Aperture 0 = tudo em foco (Clássico Ray Casting)

    // --- Render Loop ---
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rLinhas restantes: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                
                // CHAMA O NOVO RAY CASTING
                pixel_color += ray_color(r, world, main_light);
            }
            
            // Gamma e Output
            auto r = pixel_color.x();
            auto g = pixel_color.y();
            auto b = pixel_color.z();
            auto scale = 1.0 / samples_per_pixel;
            r = sqrt(scale * r);
            g = sqrt(scale * g);
            b = sqrt(scale * b);

            std::cout << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
                      << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
                      << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
        }
    }
    std::cerr << "\nTerminou!\n";
}