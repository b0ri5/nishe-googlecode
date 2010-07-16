#ifndef INCLUDE_NISHE_REFINER_H_
#define INCLUDE_NISHE_REFINER_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Graph.h>
#include <nishe/PartitionNest.h>
#include <nishe/RefineTraceValue.h>

#include <map>
#include <vector>
#include <set>

using std::map;
using std::vector;
using std::set;

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
bool is_equitable(const graph_t &G, const PartitionNest &pi);

template<typename graph_t>
class Refiner {
 public:
  int refine(const graph_t &G, PartitionNest *pi_ptr,
      RefineTraceValue<graph_t> *trace_ptr, int initial_active_index = -1);

 private:
  int refine(const graph_t &G, PartitionNest *pi_ptr,
      RefineTraceValue<graph_t> *trace_ptr, vector<int> *active_indices_ptr);

  void split_with_index(int active_index, int active_count, const graph_t &G,
      PartitionNest *pi_ptr, RefineTraceValue<graph_t> *trace_ptr,
      vector<int> *active_indices_ptr, int *cmp_ptr);

  void sort_and_split_indices(int active_count,
      const set<int> &adjacent_indices, PartitionNest *pi_ptr,
      RefineTraceValue<graph_t> *trace_ptr, vector<int> *active_indices_ptr,
      int *cmp_ptr);

  void sort_and_split_index(int active_count, int adjacent_index,
      PartitionNest *pi_ptr, RefineTraceValue<graph_t> *trace_ptr,
      vector<int> *active_indices_ptr, int *pNbhrSumCount, int *cmp_ptr);

  // the place to sow nbhrs in
  vector<typename graph_t::attr_sum> attr_sums;
};

}  // namespace nishe

#endif  // INCLUDE_NISHE_REFINER_H_
