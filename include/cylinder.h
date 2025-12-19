#ifndef CYLINDER_H
#define CYLINDER_H

#include "hittable.h"
#include <cmath>

using namespace std;

class cylinder : public hittable {
    public:
        double height;
        double radius;
        shared_ptr<material> mat_ptr;

        // Cilindro centrado na origem (0,0,0) estendendo de -height/2 a +height/2 no eixo Y
        cylinder(double h, double r, shared_ptr<material> m) 
            : height(h), radius(r), mat_ptr(m) {}

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
            // A matemática aqui resolve x^2 + z^2 = r^2
            bool hit_side = false;
            
            // 1. Teste da Superfície Lateral
            auto a = r.direction().x() * r.direction().x() + r.direction().z() * r.direction().z();
            auto b = 2 * (r.origin().x() * r.direction().x() + r.origin().z() * r.direction().z());
            auto c = r.origin().x() * r.origin().x() + r.origin().z() * r.origin().z() - radius*radius;

            if (a > 1e-8) { // Se não for paralelo ao eixo Y
                auto delta = b*b - 4*a*c;
                if (delta >= 0) {
                    auto sqrtd = sqrt(delta);
                    auto root = (-b - sqrtd) / (2*a);
                    if (check_height(r, root, t_min, t_max, rec)) return true;
                    root = (-b + sqrtd) / (2*a);
                    if (check_height(r, root, t_min, t_max, rec)) return true;
                }
            }

            // 2. Teste das Tampas (Círculos em y = +h/2 e y = -h/2)
            if (check_cap(r, height/2, t_min, t_max, true, rec)) return true;
            if (check_cap(r, -height/2, t_min, t_max, false, rec)) return true;

            return false;
        }

    private:
        // Verifica se a interseção lateral está dentro da altura válida
        bool check_height(const ray& r, double t, double t_min, double t_max, hit_record& rec) const {
            if (t < t_min || t > t_max) return false;
            auto y = r.origin().y() + t * r.direction().y();
            if (y < -height/2 || y > height/2) return false;

            rec.t = t;
            rec.p = r.at(t);
            vec3 outward_normal = vec3(rec.p.x(), 0, rec.p.z()) / radius; // Normal aponta para fora no XZ
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            
            // UV mapping cilíndrico
            auto phi = atan2(rec.p.z(), rec.p.x()); 
            rec.u = 1 - (phi + M_PI) / (2*M_PI);
            rec.v = (y + height/2) / height;
            
            return true;
        }

        // Verifica interseção com as tampas planas
        bool check_cap(const ray& r, double y_plane, double t_min, double t_max, bool is_top, hit_record& rec) const {
            auto t = (y_plane - r.origin().y()) / r.direction().y();
            if (t < t_min || t > t_max) return false;
            
            auto x = r.origin().x() + t * r.direction().x();
            auto z = r.origin().z() + t * r.direction().z();
            
            if (x*x + z*z <= radius*radius) {
                rec.t = t;
                rec.p = r.at(t);
                rec.set_face_normal(r, vec3(0, is_top ? 1 : -1, 0));
                rec.mat_ptr = mat_ptr;
                rec.u = (x/radius + 1)/2; // Mapeamento planar simples
                rec.v = (z/radius + 1)/2;
                return true;
            }
            return false;
        }
};

#endif