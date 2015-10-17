#include <boost/cstdint.hpp>
#include "common.h"
#include "math/math.h"
#include "renderer/entity/entity.h"
#include "renderer/accel/kdtree.h"
#include "renderer/intersection.h"
#include "util/zone.h"

#include <set>

namespace raytrace {;

using namespace std;

void KdNode::MakeLeaf(vector<uint32_t> const& overlapped, MemoryZone& zone) {
  p.type = 3;
  p.num_entities = overlapped.size();
  
  if (overlapped.size() == 1) {
    single_entity = overlapped[0];
  } else {
    entities = zone.Alloc<uint32_t>(overlapped.size());
    for (size_t i = 0; i < overlapped.size(); ++i) {
      entities[i] = overlapped[i];
    }
  }
}

KdTree::KdTree(vector<boost::shared_ptr<Entity> > const& ents)
    : entities(ents),
      free_node(0)
{
  // Max depth heuristic from the pbrt raytracer.
  max_depth = (int)ceil(8 + 1.3f * logf((ents.size()) / logf(2.0f)));
  boxes.reserve(ents.size());
  for (size_t i = 0; i < ents.size(); ++i) {
    Box ent_bounds = ents[i]->BoundingBox();
    bounds = Union(bounds, ent_bounds);
    boxes.push_back(ent_bounds);
  }

  vector<uint32_t> overlapped(ents.size());
  for (size_t i = 0; i < ents.size(); ++i) {
    overlapped[i] = i;
  }

  nodes.resize(1024);
  for (int i = 0; i < 3; ++i) {
    planes[i].resize(ents.size() * 2);
  }

  Build(0, bounds, overlapped, 0, 0);
}

void KdTree::Build(int node, Box const& node_bounds,
                   vector<uint32_t> const& overlapped,
                   int depth, int bad_splits)
{
  int n = free_node;
  ++free_node;
  
  if (free_node >= nodes.size()) {
    nodes.resize(nodes.size() * 2);
  }

  // Check to see if there are a sufficiently small number of entities in this
  // node, or we're too deep to keep recursive
  if (depth == max_depth || overlapped.size() <= kEntitiesPerLeaf) {
    nodes[n].MakeLeaf(overlapped, zone);
    return;
  }

  // The heuristic used for splitting tries to minimize the probability that
  // a ray will need to intersect with a given cell using its surface area and
  // number of elements on each side of the split plane
  double min_cost = INFINITY;
  int min_axis = -1, min_ent = -1;
  double last_cost = kIntersectionCost * overlapped.size();
  double area = SurfaceArea(node_bounds);
  int axis = LongestSide(node_bounds);

  for (int retry = 0; retry < 2; ++retry) {
    for (size_t i = 0; i < overlapped.size(); ++i) {
      int ent = overlapped[i];
      Box const& box = boxes[ent];
      planes[axis][2*i] = KdPlane(box.ll[axis], ent, true);
      planes[axis][2*i+1] = KdPlane(box.ur[axis], ent, false);
    }
    sort(planes[axis].begin(), planes[axis].begin() + (overlapped.size() * 2));

    // Find the best place to split
    int below = 0, above = overlapped.size();
    for (size_t i = 0; i < 2*overlapped.size(); ++i) {
      if (!planes[axis][i].start) --above;
      double plane_pos = planes[axis][i].pos;
      if (plane_pos > node_bounds.ll[axis] && plane_pos < node_bounds.ur[axis]) {
        Box above_box = node_bounds;
        Box below_box = node_bounds;
        above_box.ll[axis] = below_box.ur[axis] = plane_pos;
        double prob_below = SurfaceArea(below_box) / area;
        double prob_above = SurfaceArea(above_box) / area;
        double empty_bonus = (above == 0 && below == 0) ? kEmptyBonus : 0;
        double cost = kTraversalCost + kIntersectionCost * (1.0 - empty_bonus) *
                        (prob_below * below + prob_above * above);

        if (cost < min_cost) {
          min_cost = cost;
          min_axis = axis;
          min_ent = i;
        }
      }
      if (planes[axis][i].start) ++below;
    }

    if (min_axis >= 0) break;
    // No good splits were found, try again with the next axis
    axis = (axis + 1) % 3;
  }

  // Make a leaf if we can't do an effective split
  if (min_cost > last_cost) ++bad_splits;
  if ((min_cost > 4.0 * last_cost) || min_axis == -1 || bad_splits == 4) {
    nodes[n].MakeLeaf(overlapped, zone);
    return;
  }

  // Categorize nodes which overlap the two sides of the chosen split
  vector<uint32_t> below_ents, above_ents;
  below_ents.reserve(overlapped.size());
  above_ents.reserve(overlapped.size());
  for (int i = 0; i < min_ent; ++i) {
    if (planes[min_axis][i].start) {
      below_ents.push_back(planes[min_axis][i].entity);
    }
  }
  for (int i = min_ent + 1; i < (int)overlapped.size() * 2; ++i) {
    if (!planes[min_axis][i].start) {
      above_ents.push_back(planes[min_axis][i].entity);
    }
  }

  // Build children
  double plane_pos = planes[min_axis][min_ent].pos;
  Box above_box = node_bounds;
  Box below_box = node_bounds;
  above_box.ll[min_axis] = below_box.ur[min_axis] = plane_pos;

  Build(n + 1, below_box, below_ents, depth + 1, bad_splits);
  nodes[n].MakeNode(min_axis, free_node, plane_pos);
  Build(free_node, above_box, above_ents, depth + 1, bad_splits);
}

bool KdTree::Intersect(Ray const& iray, Intersection *intersection) const {
  double near, far;
  Ray ray = iray;
  if (!bounds.Intersect(ray, &near, &far)) {
    return false;
  }

  struct {
    const KdNode *node;
    double near, far;
  } remaining[64];
  int cur = 0;

  bool hit = false;
  const KdNode *node = &nodes[0];
  Intersection nearest;
  nearest.distance = INFINITY;

  while (node) {
    if (ray.max_dist < near) break;

    if (node->p.type != 3) { // not a leaf?
      int axis = node->p.type;

      // If the ray is directly on a splitting plane, jitter it slightly to
      // prevent NaNs from cropping up
      if (ray.origin[axis] == node->split_pos && ray.direction[axis] == 0) {
        ray.direction[axis] -= 0.00001;
      }

      double plane_pos = (node->split_pos - ray.origin[axis])
        / ray.direction[axis];

      const KdNode *node1, *node2;
      if ((ray.origin[axis] < node->split_pos) ||
          (ray.origin[axis] == node->split_pos && ray.direction[axis] < 0))
      {
        node1 = node + 1;
        node2 = &nodes[node->p.other_child];
      } else {
        node1 = &nodes[node->p.other_child];
        node2 = node + 1;
      }

      if (plane_pos > far || plane_pos <= 0) {
        node = node1;
      } else if (plane_pos < near) {
        node = node2;
      } else {
        remaining[cur].node = node2;
        remaining[cur].near = plane_pos;
        remaining[cur].far = far;
        ++cur;

        node = node1;
        far = plane_pos;
      }
    } else {
      if (node->p.num_entities == 1) {
        boost::shared_ptr<Entity> const& ent = entities[node->single_entity];
        Intersection inter;
        if (ent->Intersect(ray, &inter) && inter.distance < nearest.distance) {
          nearest = inter;
          hit = true;
          if (!intersection) return true;
        }
      } else {
        for (unsigned int i = 0; i < node->p.num_entities; ++i) {
          boost::shared_ptr<Entity> const& ent = entities[node->entities[i]];
          Intersection inter;
          if (ent->Intersect(ray, &inter) && inter.distance < nearest.distance) {
            nearest = inter;
            hit = true;
            if (!intersection) return true;
          }
        }
      }

      if (cur > 0) {
        --cur;
        node = remaining[cur].node;
        near = remaining[cur].near;
        far = remaining[cur].far;
      } else {
        break;
      }
    }
  }

  if (intersection) *intersection = nearest;
  return hit;
}

}