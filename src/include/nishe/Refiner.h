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

namespace nishe {

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
            RefineTraceValue<graph_t> *pTrace, int initial_active_index = -1);

 private:
    int refine(const graph_t &G, PartitionNest *pPi,
        RefineTraceValue<graph_t> *pTrace,
        vector<int> &initial_active_indices);

    vector<typename graph_t::nbhr_sum> nbhr_sums;
};

}  // namespace nishe

#endif  // _REFINER_H_
