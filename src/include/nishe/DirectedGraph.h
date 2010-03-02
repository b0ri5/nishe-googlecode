#ifndef _DIRECTED_GRAPH_H_
#define _DIRECTED_GRAPH_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Graph.h>

#include <utility>

// a degree triplet (for in, out, and both arcs)
struct InOutBoth
{
    InOutBoth &operator+=(int k);
    InOutBoth &operator=(int k);
    bool operator<(const InOutBoth &a) const;
    bool operator!=(const InOutBoth &a) const;
    bool operator==(const InOutBoth &a) const;

    size_t weights[3];
};

/*
 * Represents a directed graph encoded as a weighted graph.
 *
 * The vertex_degree type is a pair of a vertex and a weight of 0, 1, or 2,
 * and the degree sums are triplets of nonnegative integers (in, out, both)
 */
class DirectedGraph : public Graph< std::pair<vertex_t, size_t>, InOutBoth, size_t>
{
 public:
    static const size_t IN;
    static const size_t OUT;
    static const size_t BOTH;

    // adds the arc (u, v) to the graph
    // returns false if the arc cannot be added (it already exists)
    bool add_arc(vertex_t u, vertex_t v);

    vertex_t nbhr_vertex(const DirectedGraph::nbhr &nbhr) const
    {
        return nbhr.first;
    }

    DirectedGraph::degree nbhr_degree(const DirectedGraph::nbhr &nbhr) const
    {
        return nbhr.second;
    }
};


#endif  // _DIRECTED_GRAPH_H_
