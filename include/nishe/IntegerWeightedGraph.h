#ifndef _INTEGER_WEIGHTED_GRAPH_H_
#define _INTEGER_WEIGHTED_GRAPH_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Graph.h>
#include <nishe/MapDegreeSum.h>

#include <utility>

namespace nishe {

class IntegerWeightedGraph : public Graph<std::pair<vertex_t, int>,
    int, MapDegreeSum<int> >
{
 public:
    bool add_weighted_edge(vertex_t u, vertex_t v, int weight);
    bool add_weighted_arc(vertex_t u, vertex_t v, int weight);

    vertex_t nbhr_vertex(const IntegerWeightedGraph::nbhr &nbhr) const
    {
        return nbhr.first;
    }

    IntegerWeightedGraph::attr
        nbhr_attr(const IntegerWeightedGraph::nbhr &nbhr) const
    {
        return nbhr.second;
    }
};

}  // namespace nishe

#endif  // _INTEGER_WEIGHTED_GRAPH_H_
