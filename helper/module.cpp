// Wrap CGAL's Delaunay for benchmarking purposes

// Work around obscure CGAL template bugs
#define BOOST_RESULT_OF_USE_TR1 1
#define CGAL_CFG_NO_CPP0X_TUPLE 1
#define CGAL_CFG_NO_CPP0X_VARIADIC_TEMPLATES 1

#include <other/core/array/ConstantMap.h>
#include <other/core/exact/circle_csg.h>
#include <other/core/exact/circle_predicates.h>
#include <other/core/exact/scope.h>
#include <other/core/random/Random.h>
#include <other/core/structure/Hashtable.h>
#include <other/core/python/module.h>
#include <other/core/python/wrap.h>
#undef isspace

// Force CGAL to not use C++11 for compatibility with clang, etc.
#include <CGAL/config.h>
#undef CGAL_CFG_NO_CPP0X_RVALUE_REFERENCE
#define CGAL_CFG_NO_CPP0X_RVALUE_REFERENCE 1
#undef CGAL_CFG_NO_CPP0X_NEXT_PREV
#define CGAL_CFG_NO_CPP0X_NEXT_PREV 1
#undef CGAL_CFG_NO_CPP0X_COPY_N
#define CGAL_CFG_NO_CPP0X_COPY_N 1
#undef __GMPXX_USE_CXX11
#define __GMPXX_USE_CXX11 0

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/algorithm.h>
#include <vector>

namespace perturb {

using namespace other;

static int cgal_time_delaunay_points(RawArray<const Vector<double,2>> X) {
  typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
  typedef CGAL::Triangulation_vertex_base_2<K> Vb;
  typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
  typedef CGAL::Triangulation_data_structure_2<Vb,Fb> Tds;
  typedef CGAL::Delaunay_triangulation_2<K,Tds> DT;
  typedef DT::Point Point;

  // Convert vertices to cgal
  Array<Point> points(X.size(),false);
  for (const int i : range(X.size()))
    points[i] = Point(X[i].x,X[i].y);

  // Compute Delaunay triangulation
  DT dt;
  dt.insert(points.begin(),points.end());

  // Avoid dead code elimination
  int faces = 0;
  for (auto f=dt.finite_faces_begin();f!=dt.finite_faces_end();++f)
    faces++;
  return faces;
}

// Generate n degenerate circular arc k-gons
Nested<ExactCircleArc> degenerate_circle_arcs(const int n, const int k) {
  IntervalScope scope;
  const auto random = new_<Random>(173119131);
  Hashtable<int> used;
  Nested<ExactCircleArc> arcs(constant_map(n,k).copy(),false);
  for (int i=0;i<n;i++) {
    // Generate a random degenerate arc
    const auto arc = arcs[i];
    for (int j=0;j<k;j++) {
      arc[j].center = Vector<Quantized,2>();
      arc[j].radius = 1;
      do {
        arc[j].index = random->bits<uint32_t>();
      } while (!used.set(arc[j].index));
      arc[j].positive = arc[j].index&1;
      arc[j].left = arc[j].index>>1&1;
    }
    // Check if adjacent arcs intersect
    for (int j1=0,j0=k-1;j1<k;j0=j1++)
      if (!circles_intersect(arc,j0,j1)) {
        i--; // Discard this arc and try again
        break;
      }
  }
  return arcs;
}

// Generate n nearly degenerate circular arc k-gons.  This is intended for more direct comparison of degenerate vs. nondegenerate cases.
Nested<ExactCircleArc> nearly_degenerate_circle_arcs(const int radius, const int shift, const int n, const int k) {
  IntervalScope scope;
  const auto random = new_<Random>(173119131);
  Hashtable<int> used;
  Nested<ExactCircleArc> arcs(constant_map(n,k).copy(),false);
  for (int i=0;i<n;i++) {
    // Generate a random nearly degenerate arc
    const auto arc = arcs[i];
    for (int j=0;j<k;j++) {
      arc[j].center = Vector<Quantized,2>(random->uniform<Vector<int,2>>(-shift,shift));
      arc[j].radius = radius;
      do {
        arc[j].index = random->bits<uint32_t>();
      } while (!used.set(arc[j].index)); 
      arc[j].positive = arc[j].index&1;
      arc[j].left = arc[j].index>>1&1;
    }
    // Check if adjacent arcs intersect
    for (int j1=0,j0=k-1;j1<k;j0=j1++)
      if (!circles_intersect(arc,j0,j1)) {
        i--; // Discard this arc and try again
        break;
      }
  }
  return arcs;
}

}
using namespace perturb;

OTHER_PYTHON_MODULE(perturb_helper) {
  OTHER_FUNCTION(cgal_time_delaunay_points)
  OTHER_FUNCTION(degenerate_circle_arcs)
  OTHER_FUNCTION(nearly_degenerate_circle_arcs)
}
