#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include <memory> // Essencial para shared_ptr

using std::shared_ptr;

class sphere : public hittable {
    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;

        sphere() {}
        sphere(point3 cen, double r, shared_ptr<material> m)
            : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
            vec3 oc = r.origin() - center;
            auto a = r.direction().length_squared();
            auto half_b = dot(oc, r.direction());
            auto c = oc.length_squared() - radius*radius;

            auto discriminant = half_b*half_b - a*c;
            if (discriminant < 0) return false;
            auto sqrtd = sqrt(discriminant);

            // Encontra a raiz mais próxima no intervalo (t_min, t_max)
            auto root = (-half_b - sqrtd) / a;
            if (root < t_min || root > t_max) {
                root = (-half_b + sqrtd) / a;
                if (root < t_min || root > t_max)
                    return false;
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            
            rec.set_face_normal(r, outward_normal);
            get_sphere_uv(outward_normal, rec.u, rec.v); // Calcula textura
            rec.mat_ptr = mat_ptr;

            return true;
        }

    private:
        static void get_sphere_uv(const point3& p, double& u, double& v) {
            auto theta = acos(-p.y());
            auto phi = atan2(-p.z(), p.x()) + 3.14159265359;

            u = phi / (2 * 3.14159265359);
            v = theta / 3.14159265359;
        }
};

#endif