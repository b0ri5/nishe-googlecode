#ifndef INCLUDE_NISHE_BASICGRAPH_H_
#define INCLUDE_NISHE_BASICGRAPH_H_

/*
  Copyright 2010 Greg Tener
  Released under the Lesser General Public License v3.
*/

#include <nishe/Graph.h>

namespace nishe {

/*
 * Represents a basic graph that just keeps track of a list
 * of neighbors for each vertex.
 *
 * The vertex_degree type is just a vertex, and the degree sums
 * are nonzero integers.
 *
 * Intended for simple symmetric graphs (although allowing self loops).
 */
class BasicGraph: public Graph<vertex_t, size_t, size_t> {
 public:
  // adds the edge u, v to the graph
  // or does nothing if the edge is there already (returns false)
  bool add_edge(vertex_t u, vertex_t v);

  bool add_arc(vertex_t u, vertex_t v);

  vertex_t nbhr_vertex(const BasicGraph::nbhr &nbhr) const {
    return nbhr;
  }

  BasicGraph::attr nbhr_attr(const BasicGraph::nbhr &nbhr) const {
    return 1;
  }
};

}  // namespace nishe

#endif  // INCLUDE_NISHE_BASICGRAPH_H_
