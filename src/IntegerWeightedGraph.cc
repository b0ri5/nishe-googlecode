/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/IntegerWeightedGraph.h>

#include <vector>
#include <utility>
#include <algorithm>

using std::vector;
using std::make_pair;
using std::max;

namespace nishe {

bool IntegerWeightedGraph::add_weighted_edge(vertex_t u, vertex_t v, int weight)
{
    return add_weighted_arc(u, v, weight) && add_weighted_arc(v, u, weight);
}

bool IntegerWeightedGraph::add_weighted_arc(vertex_t u, vertex_t v, int weight)
{
    add_vertex(std::max(u, v) );

    int k = find_nbhr(u, v);

    // if there is no edge going to v
    if (k == NOT_FOUND)
    {
        // add the uv nbhr
        vNbhds.at(u).push_back(make_pair(v, weight) );

        return true;
    }

    return false;
}

}  // namespace nishe
