#ifndef INSTANCE_H
#define INSTANCE_H

#include "hittable.h"
#include "mat4.h"

using namespace std;

class instance : public hittable {
    public:
        shared_ptr<hittable> ptr;
        mat4 transform_matrix;
        mat4 inverse_matrix; // Precisamos da inversa para o raio

        instance(shared_ptr<hittable> p, mat4 m) : ptr(p), transform_matrix(m) {
            // AQUI DEVÍAMOS CALCULAR A INVERSA REAL
            // Como mat4.h é simples, vamos assumir que o usuário passa a inversa no construtor
            // ou implementaremos uma inversa simples para translação/rotação depois.
            // Hack temporário: Para este trabalho, passaremos a inversa manualmente na main se precisar,
            // ou implementaremos translações inversas simples.
            
            // Para simplificar a vida do Thiago agora:
            // Vamos focar que a matriz M move o objeto do Local -> Mundo
            // O Raio deve ir do Mundo -> Local, então precisa de M_inv.
            // Vou assumir que 'm' já é a transformação desejada e vamos tentar inverter translação simples aqui.
        }
        
        // Construtor melhor: guarda a matriz e sua inversa
        instance(shared_ptr<hittable> p, mat4 m, mat4 m_inv) 
            : ptr(p), transform_matrix(m), inverse_matrix(m_inv) {}


        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
            // 1. Transforma o raio para o espaço do objeto (Mundo -> Local)
            vec4 origin_local = inverse_matrix * vec4(r.origin(), 1.0);
            vec4 dir_local    = inverse_matrix * vec4(r.direction(), 0.0);

            ray ray_local(origin_local.to_vec3(), dir_local.to_vec3());

            // 2. Testa interseção no espaço local (onde a esfera está na origem, etc)
            if (!ptr->hit(ray_local, t_min, t_max, rec))
                return false;

            // 3. Transforma o ponto de impacto e a normal de volta para o mundo
            vec4 p_world = transform_matrix * vec4(rec.p, 1.0);
            vec4 n_world = transform_matrix * vec4(rec.normal, 0.0); // Nota: normais transformam diferente com escala (transposta da inversa), mas para rotação/translação ok.

            rec.p = p_world.to_vec3();
            rec.normal = unit_vector(n_world.to_vec3());
            rec.set_face_normal(ray_local, rec.normal); // Reajusta face

            return true;
        }
};

#endif