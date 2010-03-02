#ifndef _REFINER_H_
#define _REFINER_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Graph.h>
#include <nishe/PartitionNest.h>
#include <nishe/RefineTraceValue.h>

#include <map>
#include <vector>

using std::map;
using std::vector;

/*
 * This makes a partition pi equitable with respect to G.
 * This means that for each u, v in a cell of pi, the nbhr_sum of
 * u into every cell of pi must be equal to v's nbhr_sum into every
 * cell of pi as well.
 *
 * The function also takes in and returns a trace of the refinement. The return
 * value is a comparison (-1, 0, or 1) between the trace of the current
 * refinement and the passed in trace.
 */

template <typename graph_t>
class Refiner
{
 public:
    int refine(const graph_t &G, PartitionNest *pPi,
            RefineTraceValue<graph_t> *pTrace);

 private:
    vector<typename graph_t::nbhr_sum> nbhr_sums;
};

// a helper for is_equitable
template <typename graph_t>
static void sow_vertex(map<int, typename graph_t::nbhr_sum> *pnbhr_sums,
        const graph_t &G, vertex_t u)
{
    const typename graph_t::nbhr *nbhd = G.get_nbhd(u);

    for (int i = 0; i < G.get_nbhd_size(u); i++)
    {
        vertex_t v = G.nbhr_vertex(nbhd[i]);
        (*pnbhr_sums)[v] += G.nbhr_degree(nbhd[i]);
    }
}

/*
 * Determine if a partition is equitable with respect to a graph.
 * Used to verify the correctness of Refiner.
 */
template <typename graph_t>
bool is_equitable(const graph_t &G, const PartitionNest &pi)
{
    for (int i = 0; i != pi.terminal_index(); i = pi.next_index(i) )
    {
        if (pi.cell_size(i) == 1)
        {
            continue;
        }

        vector<int> cell = pi[i];
        vertex_t u = cell[0];
        map<int, typename graph_t::nbhr_sum> nbhr_sums;

        sow_vertex(&nbhr_sums, G, u);

        // now verify that the other cells all sow the same
        for (int j = 1; j < pi.cell_size(i); j++)
        {
            map<int, typename graph_t::nbhr_sum> other_nbhr_sums;
            vertex_t v = cell[j];

            sow_vertex(&other_nbhr_sums, G, v);

            // if two vertices in the same cell sow differently
            if (nbhr_sums != other_nbhr_sums)
            {
                return false;
            }
        }
    }

    return true;
}


#endif  // _REFINER_H_
