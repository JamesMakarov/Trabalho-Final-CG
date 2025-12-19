#ifndef CONE_H
#define CONE_H

#include "hittable.h"

using namespace std;

class cone : public hittable {
    public:
        double height;
        double radius;
        shared_ptr<material> mat_ptr;

        // Cone com base em y=0 e ponta em y=height
        cone(double h, double r, shared_ptr<material> m) 
            : height(h), radius(r), mat_ptr(m) {}

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
            // Equação simplificada do cone: x^2 + z^2 = (r * (h-y)/h)^2
            double k = radius / height;
            k = k*k;
            
            auto a = r.direction().x()*r.direction().x() + r.direction().z()*r.direction().z() - k*r.direction().y()*r.direction().y();
            auto b = 2 * (r.origin().x()*r.direction().x() + r.origin().z()*r.direction().z() + k*r.direction().y()*(height - r.origin().y()));
            auto c = r.origin().x()*r.origin().x() + r.origin().z()*r.origin().z() - k*(height - r.origin().y())*(height - r.origin().y());

            auto delta = b*b - 4*a*c;
            if (delta >= 0) {
                auto sqrtd = sqrt(delta);
                auto root = (-b - sqrtd) / (2*a);
                if (check_cone(r, root, t_min, t_max, rec)) return true;
                root = (-b + sqrtd) / (2*a);
                if (check_cone(r, root, t_min, t_max, rec)) return true;
            }
            
            // Checa a base (disco em y=0)
            if (check_base(r, t_min, t_max, rec)) return true;

            return false;
        }

    private:
        bool check_cone(const ray& r, double t, double t_min, double t_max, hit_record& rec) const {
            if (t < t_min || t > t_max) return false;
            auto y = r.origin().y() + t * r.direction().y();
            if (y < 0 || y > height) return false; // Corta nas alturas 0 e h

            rec.t = t;
            rec.p = r.at(t);
            
            // Normal do cone
            double r_at_y = radius * (height - y) / height;
            vec3 outward_normal = vec3(rec.p.x(), k_slope_normal(), rec.p.z());
            outward_normal = unit_vector(outward_normal);
            
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            rec.u = rec.p.x() / (2*radius) + 0.5;
            rec.v = y / height;
            return true;
        }
        
        // Helper para a inclinação da normal
        double k_slope_normal() const {
            return (radius/height); 
        }

        bool check_base(const ray& r, double t_min, double t_max, hit_record& rec) const {
            auto t = (0 - r.origin().y()) / r.direction().y();
            if (t < t_min || t > t_max) return false;
            
            auto x = r.origin().x() + t * r.direction().x();
            auto z = r.origin().z() + t * r.direction().z();
            
            if (x*x + z*z <= radius*radius) {
                rec.t = t;
                rec.p = r.at(t);
                rec.set_face_normal(r, vec3(0, -1, 0)); // Aponta para baixo
                rec.mat_ptr = mat_ptr;
                return true;
            }
            return false;
        }
};

#endif