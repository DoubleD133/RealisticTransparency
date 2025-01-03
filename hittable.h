#ifndef HITTABLE_H
#define HITTABLE_H

#include "interval.h"
#include "ray.h"
#include "texture.h"

class material;

class hit_record {
public:
  float u;
  float v;
  point3 p;
  vec3 normal;
  float t;
  bool front_face;
  material* m;
  bool InOmbrabile = true;
  float d_dot_n;

  void set_face_normal(const ray& r, const vec3& outward_normal) {
    // Sets the hit record normal vector.
    // NOTE: the parameter `outward_normal` is assumed to have unit length.
    d_dot_n = dot(r.d, outward_normal);
    front_face = d_dot_n < 0;
    if (front_face) {
      normal = outward_normal;
      d_dot_n = -d_dot_n;
    }
    else
      normal = -outward_normal;
  }
};

class hittable {
public:
  bool InOmbrabile = true;
  bool FaOmbra = true;

  virtual ~hittable() = default;

  virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
  virtual bool hit_shadow(const ray& r, interval ray_t) const = 0;
};

#endif

