#ifndef MESH_H
#define MESH_H

#include "hittable.h"
#include "hittable_list.h"

// Triângulo Individual
class triangle : public hittable {
    public:
        point3 v0, v1, v2;
        shared_ptr<material> mat_ptr;

        triangle(point3 _v0, point3 _v1, point3 _v2, shared_ptr<material> m)
            : v0(_v0), v1(_v1), v2(_v2), mat_ptr(m) {}

        // Algoritmo de Möller–Trumbore para interseção raio-triângulo
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
            vec3 v0v1 = v1 - v0;
            vec3 v0v2 = v2 - v0;
            vec3 pvec = cross(r.direction(), v0v2);
            double det = dot(v0v1, pvec);

            if (fabs(det) < 1e-8) return false; 
            double invDet = 1.0 / det;

            vec3 tvec = r.origin() - v0;
            double u = dot(tvec, pvec) * invDet;
            if (u < 0 || u > 1) return false;

            vec3 qvec = cross(tvec, v0v1);
            double v = dot(r.direction(), qvec) * invDet;
            if (v < 0 || u + v > 1) return false;

            double t = dot(v0v2, qvec) * invDet;
            if (t < t_min || t > t_max) return false;

            rec.t = t;
            rec.p = r.at(t);
            vec3 normal = unit_vector(cross(v0v1, v0v2));
            rec.set_face_normal(r, normal);
            rec.mat_ptr = mat_ptr;
            rec.u = u;
            rec.v = v;
            return true;
        }
};

// Caixa completa (6 faces, 12 triângulos)
class box_mesh : public hittable_list {
    public:
        box_mesh(const point3& p0, const point3& p1, shared_ptr<material> ptr) {
            point3 min = point3(fmin(p0.x(), p1.x()), fmin(p0.y(), p1.y()), fmin(p0.z(), p1.z()));
            point3 max = point3(fmax(p0.x(), p1.x()), fmax(p0.y(), p1.y()), fmax(p0.z(), p1.z()));

            vec3 dx(max.x()-min.x(), 0, 0);
            vec3 dy(0, max.y()-min.y(), 0);
            vec3 dz(0, 0, max.z()-min.z());

            // Frente (Z normal +)
            add(make_shared<triangle>(min,           min+dx,      min+dy,      ptr));
            add(make_shared<triangle>(min+dx,        min+dx+dy,   min+dy,      ptr));

            // Trás (Z normal -)
            add(make_shared<triangle>(min+dx+dz,     min+dz,      min+dx+dy+dz, ptr));
            add(make_shared<triangle>(min+dz,        min+dy+dz,   min+dx+dy+dz, ptr));

            // Topo (Y normal +)
            add(make_shared<triangle>(min+dy,        min+dy+dx,   min+dy+dz,    ptr));
            add(make_shared<triangle>(min+dy+dx,     min+dy+dx+dz,min+dy+dz,    ptr));

            // Fundo (Y normal -)
            add(make_shared<triangle>(min,           min+dz,      min+dx,       ptr));
            add(make_shared<triangle>(min+dx,        min+dz,      min+dx+dz,    ptr));

            // Esquerda (X normal -)
            add(make_shared<triangle>(min,           min+dy,      min+dz,       ptr));
            add(make_shared<triangle>(min+dz,        min+dy,      min+dy+dz,    ptr));

            // Direita (X normal +)
            add(make_shared<triangle>(min+dx+dz,     min+dy+dx+dz, min+dx,      ptr));
            add(make_shared<triangle>(min+dx,        min+dy+dx+dz, min+dy+dx,   ptr));
        }
};

#endif