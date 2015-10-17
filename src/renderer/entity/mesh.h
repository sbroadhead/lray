#ifndef _RAYTRACE_RENDERER_ENTITY_MESH_H_
#define _RAYTRACE_RENDERER_ENTITY_MESH_H_

#include "common.h"
#include "renderer/scene.h"
#include "renderer/entity/entity.h"
#include "renderer/texture/texture.h"
#include "renderer/intersection.h"

namespace raytrace {;

struct UV {
  double u;
  double v;

  UV(double u = 0.0, double v = 0.0) : u(u), v(v)
  { }
};

struct MeshFace {
  int v1, v2, v3;
  int n1, n2, n3;
  int t1, t2, t3;
};

struct Mesh {
  vector<Point> points;
  vector<Normal> normals;
  vector<UV> uv;
  vector<MeshFace> faces;
  bool smooth;

  Mesh() : smooth(false)
  { }
};

class TriangleEntity : public Entity {
public:
  virtual bool Intersect(Ray const& ray, Intersection *out) const;
  virtual Box BoundingBox() const;

  TriangleEntity(boost::shared_ptr<Mesh> mesh, MeshFace *face)
      : mesh(mesh), face(face)
  { }
private:
  boost::shared_ptr<Mesh> mesh;
  MeshFace *face;
};

// Construct a mesh and add the triangles directly to a scene. In the future
// I may add an aggregate entity which can group sub-entities into itself, but
// for now this will have to do.
static void ConstructMesh(Transform const& transformation,
  vector<Point> const& points, vector<Normal> const& normals,
  vector<UV> const& uv, vector<MeshFace> const& faces, bool smooth, 
  boost::shared_ptr<Texture> const& texture, Scene& scene)
{
  boost::shared_ptr<Mesh> mesh = boost::make_shared<Mesh>();
  mesh->faces = faces;
  mesh->points = points;
  mesh->uv = uv;
  mesh->normals = normals;
  mesh->smooth = smooth;

  for (size_t i = 0; i < mesh->faces.size(); ++i) {
    MeshFace& face = mesh->faces[i];
    boost::shared_ptr<TriangleEntity> entity =
      boost::make_shared<TriangleEntity>(mesh, &face);
    entity->texture = texture;
    scene.AddEntity(entity);
  }

  // If there are no pre-defined normals, we just create them ourselves
  if (mesh->normals.size() == 0) {
    // If we're doing smooth shading, we can just make one normal per vertex.
    // Otherwise, we need each face to have three unique normals.
    if (smooth) {
      mesh->normals.resize(points.size(), Normal(0, 0, 0));
    }

    for (size_t i = 0; i < mesh->faces.size(); ++i) {
      MeshFace& face = mesh->faces[i];
      Vector v1 = mesh->points[face.v2] - mesh->points[face.v1];
      Vector v2 = mesh->points[face.v3] - mesh->points[face.v2];
      Normal n = Normalized(Cross(v1, v2));

      // If one of the triangles is degenerate (i.e. a line), then the computed normal
      // will be zero, and the normalized version will be NaN. So we just put a dummy
      // value in in that case.
      if (n != n) { n = direction::up; }

      if (!smooth) {
        mesh->normals.push_back(n);
        face.n1 = face.n2 = face.n3 =
          mesh->normals.size() - 1;
      } else {
        face.n1 = face.v1;
        face.n2 = face.v2;
        face.n3 = face.v3;

        mesh->normals[face.n1] += n;
        mesh->normals[face.n2] += n;
        mesh->normals[face.n3] += n;
      }
    }
  }

  // If there are UVs specified, we just map the whole texture
  // to each triangle
  if (mesh->uv.size() == 0) {
    mesh->uv.push_back(UV(0.0, 0.0));
    mesh->uv.push_back(UV(0.0, 1.0));
    mesh->uv.push_back(UV(1.0, 1.0));
    
    for (size_t i = 0; i < mesh->faces.size(); ++i) {
      mesh->faces[i].t1 = 0;
      mesh->faces[i].t2 = 1;
      mesh->faces[i].t3 = 2;
    }
  }

  for (size_t i = 0; i < mesh->normals.size(); ++i) {
    mesh->normals[i] = Normalized(transformation.Apply(mesh->normals[i]));
    if (mesh->normals[i] != mesh->normals[i]) {
      int a = 0;
    }
  }

  for (size_t i = 0; i < mesh->points.size(); ++i) {
    mesh->points[i] = transformation.Apply(mesh->points[i]);
  }
}

}

#endif
