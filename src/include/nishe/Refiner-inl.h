#ifndef _REFINER_INL_H_
#define _REFINER_INL_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Refiner.h>

#include <map>
#include <vector>

namespace nishe {

template <typename graph_t>
int Refiner<graph_t>::refine(const graph_t &G, PartitionNest *pPi,
            RefineTraceValue<graph_t> *pTrace,
            int initial_active_index)
{
}

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

}  // namespace nishe

#endif  // _REFINER_INL_H_
