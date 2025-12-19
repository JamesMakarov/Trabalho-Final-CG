#include "../include/utils.h"
#include "../include/hittable_list.h"
#include "../include/sphere.h"
#include "../include/cylinder.h"
#include "../include/cone.h"
#include "../include/mesh.h"
#include "../include/camera.h"
#include "../include/material.h"
#include "../include/instance.h"
#include "../include/texture.h"

#include <iostream>

// --- Definição de Luz (Pontual) ---
struct PointLight {
    point3 position;
    color intensity;
};

// --- Ray Casting (Blinn-Phong) ---
color ray_color(const ray& r, const hittable& world, const PointLight& light) {
    hit_record rec;

    if (world.hit(r, 0.001, infinity, rec)) {
        // Dados do Material
        color color_diffuse = rec.mat_ptr->kd->value(rec.u, rec.v, rec.p);
        vec3 color_specular = rec.mat_ptr->ks;
        double shininess = rec.mat_ptr->shininess;

        // A. Ambiental
        color ambient = rec.mat_ptr->ka * color_diffuse;

        // Vetores
        vec3 light_dir = unit_vector(light.position - rec.p);
        vec3 view_dir = unit_vector(-r.direction());
        vec3 normal = unit_vector(rec.normal);

        // B. Sombra (Shadow Ray)
        ray shadow_ray(rec.p + 0.001*normal, light_dir);
        hit_record shadow_rec;
        double light_dist = (light.position - rec.p).length();

        if (world.hit(shadow_ray, 0.001, light_dist, shadow_rec)) {
            return ambient; 
        }

        // C. Difusa e Especular
        double diff = fmax(dot(normal, light_dir), 0.0);
        color diffuse = diff * color_diffuse;

        vec3 halfway_dir = unit_vector(light_dir + view_dir);
        double spec = pow(fmax(dot(normal, halfway_dir), 0.0), shininess);
        color specular = spec * color_specular;

        return ambient + (diffuse + specular) * light.intensity;
    }

    // Fundo
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

// --- FUNÇÃO DE PICKING (Interatividade 5.1) ---
// Recebe coordenadas de tela (pixel_x, pixel_y) e diz o que tem lá
void perform_pick(int x, int y, int width, int height, const camera& cam, const hittable& world) {
    // 1. Converte Pixel (0..width) para Coordenada de Textura u,v (0..1)
    // O eixo Y da imagem é invertido em relação ao mouse geralmente, mas aqui vamos assumir Y=0 embaixo
    double u = double(x) / (width - 1);
    double v = double(y) / (height - 1);

    // 2. Gera o Raio (A mesma lógica da câmera)
    ray r = cam.get_ray(u, v);

    // 3. Testa Interseção
    hit_record rec;
    std::cerr << "\n--- PICKING TEST EM (" << x << ", " << y << ") ---\n";
    std::cerr << "Raio disparado: Origem(" << r.origin() << ") Direcao(" << r.direction() << ")\n";

    if (world.hit(r, 0.001, infinity, rec)) {
        std::cerr << "[ACERTOU!] \n";
        std::cerr << "  -> Coordenada do Ponto (World): " << rec.p << "\n";
        std::cerr << "  -> Normal da Superficie: " << rec.normal << "\n";
        std::cerr << "  -> Distancia da Camera (t): " << rec.t << "\n";
        std::cerr << "  -> Coordenadas UV: " << rec.u << ", " << rec.v << "\n";
    } else {
        std::cerr << "[FUNDO] O raio nao atingiu nenhum objeto.\n";
    }
    std::cerr << "------------------------------------------\n";
}

int main() {
    // Configurações
    double zoom_vfov = 40.0; 
    const auto aspect_ratio = 1.0; 
    const int image_width = 500;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 20;

    // Mundo
    hittable_list world;
    auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

    // Materiais Phong
    auto mat_floor  = make_shared<material>(checker, 0.1, 10.0);
    auto mat_gold   = make_shared<material>(color(0.8, 0.6, 0.2), 0.2, 128.0, color(1, 0.9, 0.5));
    auto mat_silver = make_shared<material>(color(0.7, 0.7, 0.7), 0.1, 200.0, color(1,1,1));
    auto mat_ruby   = make_shared<material>(color(0.9, 0.1, 0.1), 0.2, 100.0);
    auto mat_blue   = make_shared<material>(color(0.1, 0.2, 0.5), 0.1, 64.0);

    // Objetos (Cena Altar)
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, mat_floor)); // Chão

    auto cyl_base = make_shared<cylinder>(3.0, 1.5, mat_gold);
    mat4 cyl_pos = mat4::translate(vec3(0, 1.5, 0));
    mat4 cyl_inv = mat4::translate(vec3(0, -1.5, 0));
    world.add(make_shared<instance>(cyl_base, cyl_pos, cyl_inv)); // Altar

    world.add(make_shared<sphere>(point3(0, 4.0, 0), 1.0, mat_ruby)); // Esfera

    auto cone_base = make_shared<cone>(4.0, 1.0, mat_silver);
    mat4 cone_pos = mat4::translate(vec3(4, 0, 0));
    mat4 cone_inv = mat4::translate(vec3(-4, 0, 0));
    world.add(make_shared<instance>(cone_base, cone_pos, cone_inv)); // Cone

    auto box_base = make_shared<box_mesh>(point3(0,0,0), point3(1,1,1), mat_blue);
    mat4 box_trans = mat4::translate(vec3(-4, 1, 1)) * mat4::rotate_y(degrees_to_radians(45));
    mat4 box_inv = mat4::rotate_y(degrees_to_radians(-45)) * mat4::translate(vec3(4, -1, -1)); 
    world.add(make_shared<instance>(box_base, box_trans, box_inv)); // Cubo

    // Reflexão (Bonus 1.4.5) - Cópia Espelhada do Cone
    mat4 mirror_matrix = mat4::reflection(true, false, false); // Espelha X
    mat4 mirror_pos = mirror_matrix * cone_pos;
    mat4 mirror_pos_inv = cone_inv * mirror_matrix; // Inversa é igual
    world.add(make_shared<instance>(cone_base, mirror_pos, mirror_pos_inv));

    // Luz
    PointLight main_light;
    main_light.position = point3(10, 20, 10);
    main_light.intensity = color(1.0, 1.0, 1.0);

    // Câmera
    point3 lookfrom(0, 8, 12);
    point3 lookat(0, 2, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = (lookfrom - lookat).length();
    camera cam(lookfrom, lookat, vup, zoom_vfov, aspect_ratio, 0.0, dist_to_focus);

    // --- MODO RENDERIZAÇÃO (Para Arquivo) ---
    // Importante: Usamos cerr para logs e cout para imagem
    std::cerr << "Iniciando Renderizacao...\n";
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        if (j % 50 == 0) std::cerr << "\rLinhas restantes: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, main_light);
            }
            auto r = pixel_color.x();
            auto g = pixel_color.y();
            auto b = pixel_color.z();
            auto scale = 1.0 / samples_per_pixel;
            r = sqrt(scale * r); g = sqrt(scale * g); b = sqrt(scale * b);
            std::cout << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
                      << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
                      << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
        }
    }
    std::cerr << "\nRenderizacao Concluida!\n";

    // --- MODO INTERATIVO (Picking) ---
    std::cerr << "\n============================================\n";
    std::cerr << "      MODO INTERATIVO DE PICKING (5.1)      \n";
    std::cerr << "============================================\n";
    std::cerr << "Digite as coordenadas X Y do pixel para inspecionar.\n";
    std::cerr << "A imagem tem " << image_width << "x" << image_height << " pixels.\n";
    std::cerr << "Exemplo: 250 250 (Centro) ou -1 para sair.\n";

    int px, py;
    while (true) {
        std::cerr << "\nPick (X Y): ";
        // Nota: O cin lê do teclado, não interfere no cout da imagem se redirecionado corretamente
        if (!(std::cin >> px)) break; 
        if (px == -1) break;
        std::cin >> py;

        if (px >= 0 && px < image_width && py >= 0 && py < image_height) {
            // Executa o Picking Matemático
            // Invertemos Y aqui porque no loop de render j vai de height-1 até 0
            // Se o usuário digitar 0 (fundo), queremos o j=0.
            perform_pick(px, py, image_width, image_height, cam, world);
        } else {
            std::cerr << "Coordenada invalida! Use X entre 0-" << image_width-1 << " e Y entre 0-" << image_height-1 << "\n";
        }
    }
}