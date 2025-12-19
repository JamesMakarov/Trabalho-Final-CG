#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
    public:
        std::vector<shared_ptr<hittable>> objects;

        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        // Percorre a lista para ver se o raio bate em ALGO
        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
            hit_record temp_rec;
            bool hit_anything = false;
            auto closest_so_far = t_max;

            for (const auto& object : objects) {
                // Se bateu neste objeto E ele está mais perto que o anterior...
                if (object->hit(r, t_min, closest_so_far, temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t; // Atualiza o "recorde" de distância
                    rec = temp_rec;              // Salva os dados da interseção
                }
            }

            return hit_anything;
        }
};

#endif