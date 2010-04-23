/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/BasicGraph.h>

#include <vector>
#include <algorithm>

using std::vector;

namespace nishe {

bool BasicGraph::add_edge(vertex_t u, vertex_t v)
{
    return add_arc(u, v) && add_arc(v, u);
}

bool BasicGraph::add_arc(vertex_t u, vertex_t v)
{
    add_vertex(std::max(u, v) );

    int k = find_nbhr(u, v);

    // if v is not in u's nbhd yet
    if (k == NOT_FOUND)
    {
        vNbhds.at(u).push_back(v);

        return true;
    }

    return false;
}

}  // namespace nishe
