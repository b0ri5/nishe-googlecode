/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/BasicGraph.h>

#include <vector>
#include <algorithm>

using std::vector;

bool BasicGraph::add_edge(vertex_t u, vertex_t v)
{
    add_vertex(std::max(u, v) );

    vector<BasicGraph::nbhr> &vNbhd = vNbhds.at(u);

    // if v is not in u's nbhd yet
    if (find(vNbhd.begin(), vNbhd.end(), v) == vNbhd.end() )
    {
        vNbhd.push_back(v);

        // this means u also should be in v's nbhd
        vNbhds.at(v).push_back(u);

        return true;
    }

    return false;
}
