#ifndef _GRAPH_IO_H_
#define _GRAPH_IO_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Graphs.h>
#include <nishe/PartitionNest.h>

#include <istream>
#include <sstream>
#include <string>

using std::istream;
using std::ostream;
using std::stringstream;

namespace nishe {

/*
 * This class aids in the input and output of graphs (and partitions).
 *
 * The goal is to have a centralized location to read and write graphs, and to
 * convert between graph types.
 */

/*
 * There are currently three file formats aimed to be supported:
 * 1) list_ascii (0-based)
 *         u : u1 u2 ...;
 *         v : v1 v2 ...;
 *         ...
 *         <pi>
 *
 * 2) weighted_list_ascii (0-based)
 *         u : u1,w1 u2,w2 ...;
 *         v : v1,x1 v2,x2 ...;
 *         ...
 *         <pi>
 *
 * 3) dimacs (1-based)
 *         p edge <vertex_count> <edge_count>
 *         n v c
 *         ...
 *         e u v
 *         ...
 *
 *         n v c means vertex v has color c, where c is an unsigned int
 *         e u v means there's an edge from u to v
 */

void fail(string err);

class GraphIO
{
 public:

    // input methods

    static bool input_list_ascii(istream &in,
            BasicGraph *pG, PartitionNest *pPi);

    static bool input_list_ascii(istream &in,
            DirectedGraph *pG, PartitionNest *pPi);

    static bool input_list_ascii(istream &in,
            IntegerWeightedGraph *pG, PartitionNest *pPi);

    template<typename graph_t>
    static bool input_list_ascii(string s,
            graph_t *pG, PartitionNest *pPi);

    // output methods

    static void output_list_ascii(ostream &out,
            const BasicGraph &G);

    static void output_list_ascii(ostream &out,
            const DirectedGraph &G);

    static void output_list_ascii(ostream &out,
            const IntegerWeightedGraph &G);

    template <typename graph_t>
    static string output_list_ascii_string(ostream &out,
            const graph_t &G);

    // conversions

    static void symmetric_closure(BasicGraph *pBasic,
            const DirectedGraph &directed);

    // named graphs
    static void path(BasicGraph *pG, int n);
    static void path(BasicGraph *pG, PartitionNest *pPi, int n);

    static void directed_path(DirectedGraph *pG, int n);
    static void directed_path(DirectedGraph *pG, PartitionNest *pPi, int n);

    template <typename graph_t>
    static void null(graph_t *pG, int n);

    template <typename graph_t>
    static void null(graph_t *pG, PartitionNest *pPi, int n);

 private:
    // returns false if cannot read any of the graph (eof)
    // fails if graph is input improperly
    template <typename graph_t>
    static bool input_list_ascii(istream &in,
            graph_t *pG, PartitionNest *pPi,
            bool (*)(graph_t *, vertex_t, string) );

    template <typename graph_t, typename nbhr_t>
    static void output_list_ascii(ostream &out,
            const graph_t &G,
            void (*)(ostream &in, const nbhr_t &) );
};

}  // namespace nishe

#endif  // _GRAPH_IO_H_
