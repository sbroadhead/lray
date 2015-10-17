#include "common.h"
#include "math/math.h"
#include "renderer/intersection.h"
#include "renderer/entity/mesh.h"

namespace raytrace {;

Box TriangleEntity::BoundingBox() const {
  Point p1 = mesh->points[face->v1];
  Point p2 = mesh->points[face->v2];
  Point p3 = mesh->points[face->v3];
  return AddPoint(AddPoint(Box(p1), p2), p3);
}

bool TriangleEntity::Intersect(Ray const& ray, Intersection *out) const {
  Point& p1 = mesh->points[face->v1];
  Point& p2 = mesh->points[face->v2];
  Point& p3 = mesh->points[face->v3];
  Normal& n1 = mesh->normals[face->n1];
  Normal& n2 = mesh->normals[face->n2];
  Normal& n3 = mesh->normals[face->n3];
  Vector v1 = p2 - p1;
  Vector v2 = p3 - p1;
  Vector s1 = Cross(ray.direction, v2);
  double d = Dot(s1, v1);
  if (d == 0) return false;
  d = 1.0 / d;
  
  Vector ray_point = ray.origin - p1;
  double b1 = Dot(ray_point, s1) * d;
  if (b1 < 0.0 || b1 > 1.0) return false;

  Vector s2 = Cross(ray_point, v1);
  double b2 = Dot(ray.direction, s2) * d;
  if (b2 < 0.0 || b1 + b2 > 1.0) return false;

  double hit_dist = Dot(v2, s2) * d;
  if (hit_dist < ray.min_dist || hit_dist > ray.max_dist) return false;

  if (!out) return true;

  UV uv[3] = { mesh->uv[face->t1], mesh->uv[face->t2], mesh->uv[face->t3] };

  double b0 = 1.0 - b1 - b2;
  out->geometry.u = b0 * uv[0].u + b1 * uv[1].u + b2 * uv[2].u;
  out->geometry.v = b0 * uv[0].v + b1 * uv[1].v + b2 * uv[2].v;
  out->entity = this;
  out->distance = hit_dist;
  out->geometry.normal = b0 * n1 + b1 * n2 + b2 * n3;
  out->geometry.point = ray(hit_dist);
  out->geometry.point_local = ray(hit_dist);

  // If the ray is going the same direction as the normal, we're behind the
  // triangle so reverse the normal
  if (Dot(out->geometry.normal, ray.direction) > 0.0)
    out->geometry.normal = -out->geometry.normal;

  return true;
}

}