#include "ray.h"     // Nossa classe de raios
#include <iostream>

// --- Função que testa se o raio bateu na esfera ---
// Retorna 'true' se bater, 'false' se errar
bool hit_sphere(const point3& center, double radius, const ray& r) {
    // Vetor que vai da origem do raio até o centro da esfera (A - C)
    vec3 oc = r.origin() - center;
    
    // Coeficientes da equação de 2º grau: at^2 + bt + c = 0
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius*radius;
    
    // O discriminante (delta)
    auto discriminant = b*b - 4*a*c;
    
    // Se delta > 0, tem interseção!
    return (discriminant > 0);
}

// --- Função que decide a cor do pixel ---
color ray_color(const ray& r) {
    // Testamos uma esfera no ponto (0, 0, -1) com raio 0.5
    // Lembre-se: A câmera está em (0,0,0) e a tela em z=-1
    if (hit_sphere(point3(0,0,-1), 0.5, r)) {
        return color(1, 0, 0); // Bateu? Pinta de VERMELHO
    }
    
    // Se não bateu na esfera, desenha o fundo (degradê azul/branco)
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
    // 1. Definições da Imagem
    const auto aspect_ratio = 1.0; // Imagem quadrada
    const int image_width = 500;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // 2. Definições da Câmera e Viewport (Janela virtual)
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0; // Distância entre olho e tela

    auto origin = point3(0, 0, 0);       // Onde está o olho
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    
    // Canto inferior esquerdo da tela virtual
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

    // 3. Renderização
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rLinhas restantes: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            
            // Mapeia (i, j) para coordenadas u, v entre 0 e 1
            auto u = double(i) / (image_width-1);
            auto v = double(j) / (image_height-1);
            
            // Cria o raio que sai do olho e vai para este pixel na tela
            ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
            
            // Calcula a cor do raio
            color pixel_color = ray_color(r);
            
            // Escreve a cor
            int ir = static_cast<int>(255.999 * pixel_color.x());
            int ig = static_cast<int>(255.999 * pixel_color.y());
            int ib = static_cast<int>(255.999 * pixel_color.z());
            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    std::cerr << "\nTerminou!\n";
}