#ifndef _INTEGER_WEIGHTED_GRAPH_H_
#define _INTEGER_WEIGHTED_GRAPH_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Graph.h>
#include <nishe/MapDegreeSum.h>

class IntegerWeightedGraph : public Graph<std::pair<vertex_t, int>,
    MapDegreeSum<int>, int >
{
 public:
    bool add_weighted_edge(vertex_t u, vertex_t v, int weight);

    vertex_t nbhr_vertex(const IntegerWeightedGraph::nbhr &nbhr) const
    {
        return nbhr.first;
    }

    IntegerWeightedGraph::degree
        nbhr_degree(const IntegerWeightedGraph::nbhr &nbhr) const
    {
        return nbhr.second;
    }
};

#endif  // _INTEGER_WEIGHTED_GRAPH_H_