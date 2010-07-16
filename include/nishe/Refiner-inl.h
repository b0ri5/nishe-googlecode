#ifndef INCLUDE_NISHE_REFINER_INL_H_
#define INCLUDE_NISHE_REFINER_INL_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Refiner.h>
#include <nishe/Util.h>

#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>

namespace nishe {

static void nontrivial_indices(const PartitionNest &pi, vector<int> *v) {
  int k = 0;

  while (k != pi.terminal_index()) {
    (*v).push_back(k);
    k = pi.next_index(k);
  }
}

/*
 * Checks trace_ptr to see if the index k matches what's present in its own trace.
 * It sets cmp_ptr accordingly and returns false if *cmp_ptr ends up 1
 * Assumes that we never pass in *cmp_ptr == 1
 */
template<typename graph_t>
static void trace_active_index(int k, int active_count, RefineTraceValue<
    graph_t> *trace_ptr, int *cmp_ptr) {
  assert(*cmp_ptr != 1);

  if (*cmp_ptr == 0) {
    // if there are not enough indices in trace_ptr
    // the current trace is larger
    if (trace_ptr->active_indices.size() <= active_count) {
      *cmp_ptr = 1;
      return;
    } else {
      int k_trace = trace_ptr->active_indices[active_count];

      // if our index is smaller than what's in the trace
      if (k < k_trace) {
        *cmp_ptr = -1;
        trace_ptr->resize(active_count);
      } else if (k > k_trace) {  // otherwise if it's larger
        *cmp_ptr = 1;
        return;
      }
    }
  }

  if (*cmp_ptr == -1) {
    trace_ptr->push_active_index(k);
  }
}

template<typename graph_t>
static void trace_attr_sum(int active_index, int adjacent_index,
    const typename graph_t::attr_sum &attr_sum, int attr_sum_count,
    RefineTraceValue<graph_t> *trace_ptr, int *cmp_ptr) {
  // this type is a mouthfull... so typedef it real quick
  typedef typename RefineTraceValue<graph_t>::index_attr_sum index_attr_sum;

  index_attr_sum adjacent_sum(adjacent_index, attr_sum);

  // cmp should never == 1
  assert(!cmp_ptr != 1);

  if (*cmp_ptr == 0) {
    index_attr_sum &curr_adjacent_sum =
        trace_ptr->adjacent_attr_sums[active_index][attr_sum_count];

    // if we're larger than what's already there :(
    if (adjacent_sum > curr_adjacent_sum) {
      *cmp_ptr = 1;
      return;
    } else if (adjacent_sum < curr_adjacent_sum) {
      // if we're smaller than what's already there :)
      *cmp_ptr = -1;

      // cut off everything past this active index
      trace_ptr->resize(active_index + 1);
      trace_ptr->adjacent_attr_sums.at(active_index).resize(attr_sum_count);
    }
  }

  // if we're adding to the trace, just append the index and sum
  if (*cmp_ptr == -1) {
    trace_ptr->adjacent_attr_sums.at(active_index).push_back(adjacent_sum);
  }
}

template<typename graph_t>
int Refiner<graph_t>::refine(const graph_t &G, PartitionNest *pi_ptr,
    RefineTraceValue<graph_t> *trace_ptr, int initial_active_index) {
  vector<int> active_indices;

  // add every index if the initial one isn't specified
  if (initial_active_index == -1) {
    nontrivial_indices(*pi_ptr, &active_indices);
  } else {
    active_indices.push_back(initial_active_index);
  }

  if (attr_sums.size() < G.vertex_count()) {
    attr_sums.resize(G.vertex_count());
  }

  return refine(G, pi_ptr, trace_ptr, &active_indices);
}

template<typename graph_t>
int Refiner<graph_t>::refine(const graph_t &G, PartitionNest *pi_ptr,
    RefineTraceValue<graph_t> *trace_ptr, vector<int> *active_indices_ptr) {
  // assume initially that the traces will be equal
  int cmp = 0;

  // if this trace is new, populate it
  if (trace_ptr->is_clear()) {
    trace_ptr->set_clear(false);
    cmp = -1;
  }

  int active_count = 0;

  while (active_indices_ptr->size() > 0) {
    int k = active_indices_ptr->back();
    active_indices_ptr->pop_back();

    trace_active_index(k, active_count, trace_ptr, &cmp);

    // if we observe a larger active index
    if (cmp == 1) {
      break;
    }

    split_with_index(k, active_count, G, pi_ptr, trace_ptr, active_indices_ptr,
        &cmp);

    // if we observed a larger attr_sum somewhere in there
    if (cmp == 1) {
      break;
    }

    active_count += 1;
  }

  return cmp;
}

/*
 * Sows the contents of the active index and sorts each of the adjacent
 * indices based on the attr_sums.
 */
template<typename graph_t>
void Refiner<graph_t>::split_with_index(int active_index, int active_count,
    const graph_t &G, PartitionNest *pi_ptr,
    RefineTraceValue<graph_t> *trace_ptr, vector<int> *active_indices_ptr,
    int *cmp_ptr) {
  set<int> adjacent_indices;

  // sow the active cell
  for (int i = active_index;
       i < active_index + pi_ptr->cell_size(active_index); i++) {
    vertex_t u = pi_ptr->elements()[i];
    const typename graph_t::nbhr *nbhd = G.get_nbhd(u);

    for (int j = 0; j < G.get_nbhd_size(u); j++) {
      vertex_t v = G.nbhr_vertex(nbhd[j]);

      attr_sums[v] += G.nbhr_attr(nbhd[j]);

      // keep track of which indices were sown to for the trace
      adjacent_indices.insert(pi_ptr->index_containing(v));
    }
  }

  // sort and split each adjacent index
  sort_and_split_indices(active_count, adjacent_indices, pi_ptr, trace_ptr,
      active_indices_ptr, cmp_ptr);
}

/*
 * Goes through each adjacent index and sorts and splits the index
 * if it is required.
 */
template<typename graph_t>
void Refiner<graph_t>::sort_and_split_indices(int active_count,
    const set<int> &adjacent_indices, PartitionNest *pi_ptr, RefineTraceValue<
        graph_t> *trace_ptr, vector<int> *active_indices_ptr, int *cmp_ptr) {
  int attr_sum_count = 0;

  // go over the adjacent indices in sorted order (important for trace!)
  for (set<int>::const_iterator it = adjacent_indices.begin(); it
      != adjacent_indices.end(); it++) {
    int adjacent_index = (*it);  // the adjacent index
    int adjacent_cell_size = pi_ptr->cell_size(adjacent_index);

    // sort and split the cell k
    if (adjacent_cell_size > 1) {
      sort_and_split_index(active_count, adjacent_index, pi_ptr, trace_ptr,
          active_indices_ptr, &attr_sum_count, cmp_ptr);
    } else {  // pi_ptr->cell_size(k) == 1
      int u = pi_ptr->elements()[adjacent_index];
      trace_attr_sum(active_count, adjacent_index, attr_sums[u],
          attr_sum_count, trace_ptr, cmp_ptr);
    }

    // erase our tracks
    for (int i = adjacent_index; i < adjacent_index + adjacent_cell_size; i++) {
      int u = pi_ptr->elements()[i];
      attr_sums[u] = 0;
    }

    if (*cmp_ptr == 1) {
      // clear what was sown
      for (; it != adjacent_indices.end(); it++) {
        adjacent_index = (*it);

        for (int i = adjacent_index; i < adjacent_index + pi_ptr->cell_size(
            adjacent_index); i++) {
          int u = pi_ptr->elements()[i];
          attr_sums[u] = 0;
        }
      }

      // and bail
      return;
    }
  }
}

/*
 * A comparator to sort a cell based on attr_sums
 */
template<typename graph_t>
struct NbhrSumComparator {
  vector<typename graph_t::attr_sum> *attr_sums_ptr;

  explicit NbhrSumComparator(
      vector<typename graph_t::attr_sum> *attr_sums_ptr) :
    attr_sums_ptr(attr_sums_ptr) {
  }

  bool operator()(int u, int v) {
    return (*attr_sums_ptr)[u] < (*attr_sums_ptr)[v];
  }
};

template<typename attr_sum>
static void clear_rest_of_cell(int k, int start, PartitionNest *pi_ptr, vector<
    attr_sum> *attr_sums_ptr) {
  for (int i = start; i < k + pi_ptr->cell_size(k); i++) {
    int u = pi_ptr->elements()[i];
    (*attr_sums_ptr)[u] = 0;
  }
}

/*
 * Splits a single nontrivial cell based on the attr_sums.
 * Also cleans up its portion of the attr_sums.
 */
template<typename graph_t>
void Refiner<graph_t>::sort_and_split_index(int active_count, int k,
    PartitionNest *pi_ptr, RefineTraceValue<graph_t> *trace_ptr,
    vector<int> *active_indices_ptr, int *attr_sum_count_ptr, int *cmp_ptr) {
  NbhrSumComparator<graph_t> cmp(&attr_sums);

  // sort the elements of the cell k based on their attr_sums
  std::sort(pi_ptr->elements() + k, pi_ptr->elements() + k + pi_ptr->cell_size(
      k), cmp);

  // go cell by cell and record when the attr_sum changes
  int u = pi_ptr->elements()[k];
  typename graph_t::attr_sum *prev_attr_sum_ptr = &attr_sums[u];

  // check the first cell
  trace_attr_sum(active_count, k, *prev_attr_sum_ptr, *attr_sum_count_ptr,
      trace_ptr, cmp_ptr);

  if (*cmp_ptr == 1) {
    return;
  }

  // go through and add the splits
  for (int i = k + 1; i < k + pi_ptr->cell_size(k); i++) {
    u = pi_ptr->elements()[i];

    // if we encounter a different attr_sum
    if (attr_sums[u] != *prev_attr_sum_ptr) {
      // enqueue the new index
      pi_ptr->enqueue_new_index(i);

      // add this as an active index
      active_indices_ptr->push_back(i);

      // update the pointer to the previous attr_sum
      prev_attr_sum_ptr = &attr_sums[u];

      // see if the trace checks out
      trace_attr_sum(active_count, i, *prev_attr_sum_ptr, *attr_sum_count_ptr,
          trace_ptr, cmp_ptr);
      *attr_sum_count_ptr += 1;

      if (*cmp_ptr == 1) {
        return;
      }
    }
  }

  pi_ptr->commit_pending_indices();
}

// a helper for is_equitable
template<typename graph_t>
static void sow_vertex(map<int, typename graph_t::attr_sum> *attr_sums_ptr,
    const graph_t &G, vertex_t u) {
  const typename graph_t::nbhr *nbhd = G.get_nbhd(u);

  for (int i = 0; i < G.get_nbhd_size(u); i++) {
    vertex_t v = G.nbhr_vertex(nbhd[i]);
    (*attr_sums_ptr)[v] += G.nbhr_attr(nbhd[i]);
  }
}

/*
 * Determine if a partition is equitable with respect to a graph.
 * Used to verify the correctness of Refiner.
 */
template<typename graph_t>
bool is_equitable(const graph_t &G, const PartitionNest &pi) {
  for (int i = 0; i != pi.terminal_index(); i = pi.next_index(i)) {
    if (pi.cell_size(i) == 1) {
      continue;
    }

    vector<int> cell = pi[i];
    vertex_t u = cell[0];
    map<int, typename graph_t::attr_sum> attr_sums;

    sow_vertex(&attr_sums, G, u);

    // now verify that the other cells all sow the same
    for (int j = 1; j < pi.cell_size(i); j++) {
      map<int, typename graph_t::attr_sum> other_attr_sums;
      vertex_t v = cell[j];

      sow_vertex(&other_attr_sums, G, v);

      // if two vertices in the same cell sow differently
      if (attr_sums != other_attr_sums) {
        return false;
      }
    }
  }

  return true;
}

}  // namespace nishe

#endif  // INCLUDE_NISHE_REFINER_INL_H_
