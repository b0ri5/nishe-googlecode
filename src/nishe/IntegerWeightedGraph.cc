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
    add_vertex(std::max(u, v) );

    int k = find_nbhr(u, v);

    // if there is no edge going to v
    if (k == -1)
    {
        // add the uv nbhr
        vNbhds.at(u).push_back(make_pair(v, weight) );
        // add the vu nbhr
        vNbhds.at(v).push_back(make_pair(u, weight) );

        return true;
    }

    return false;
}

}  // namespace nishe
