#ifndef _RAYTRACE_RENDERER_ACCEL_KDTREE_H_
#define _RAYTRACE_RENDERER_ACCEL_KDTREE_H_

#include <boost/cstdint.hpp>
#include "common.h"
#include "math/math.h"
#include "renderer/entity/entity.h"
#include "renderer/intersection.h"
#include "util/zone.h"

namespace raytrace {;

using ::boost::uint32_t;
using ::std::vector;

static const unsigned int kEntitiesPerLeaf = 3;
static const int kIntersectionCost = 80;
static const int kTraversalCost = 1;
static const double kEmptyBonus = 0.5;

//
//
//  NOTE:
//  This kd-tree class is based mostly on the design given in the book
//  Physically Based Rendering by Matt Pharr and Greg Humphreys
//  http://www.pbrt.org/
//
//

struct KdPlane {
  double pos;
  int entity;
  bool start;

  bool operator<(KdPlane const& other) const {
    return (pos < other.pos) || (pos == other.pos && start && !other.start);
  }

  KdPlane(double pos = 0.0, int entity = 0, bool start = false)
      : pos(pos), entity(entity), start(start)
  { }
};

// kd-tree node optimized to fit into 8 bytes
// (will be 12 on 64-bit machines..TODO: Fix this?)
struct KdNode {
  union {
    float split_pos;
    uint32_t single_entity;
    uint32_t *entities;
  };

  struct {
    unsigned int type : 2;
    union {
      unsigned int num_entities : 30;
      unsigned int other_child : 30;
    };
  } p;

  void MakeLeaf(vector<uint32_t> const& overlapped, MemoryZone& zone);
  void MakeNode(int axis, int child, double split) {
    p.type = axis;
    p.other_child = child;
    split_pos = (float)split;
  }
};

class KdTree {
public:
  void Build(int node, Box const& node_bounds,
             vector<uint32_t> const& overlapped,
             int depth, int bad_splits);
  bool Intersect(Ray const& ray, Intersection *intersection) const;

  KdTree(vector<boost::shared_ptr<Entity> > const& ents);
private:
  unsigned int max_depth;
  vector<KdNode> nodes;
  unsigned int free_node;
  vector<boost::shared_ptr<Entity> > entities;
  MemoryZone zone;
  Box bounds;
  vector<Box> boxes;
  vector<KdPlane> planes[3];
};

}

#endif