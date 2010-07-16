#ifndef INCLUDE_NISHE_REFINETRACEVALUE_H_
#define INCLUDE_NISHE_REFINETRACEVALUE_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <vector>
#include <utility>

using std::vector;
using std::pair;

/*
 * A refine trace value stores a trace of a refinement procedure.
 * It must be able to compare itself to another refine trace value.
 *
 * The refiner goes through the active indices, stored in affected_indices
 * For each affected index, store the vector of indices this index's nbhrs.
 *
 */

template<typename graph_t>
class RefineTraceValue {
 public:
  RefineTraceValue() :
    clear_(true) {
  }

  /*
   * RefineTraceValues are first compared lexicographically by
   * the active_indices and then by the index sums for each active
   * index.
   */
  int cmp(const RefineTraceValue<graph_t> &a);

  void clear() {
    active_indices.clear();
    adjacent_attr_sums.clear();
    clear_ = true;
  }

  bool is_clear() {
    return clear_;
  }

  void set_clear(bool flag) {
    clear_ = flag;
  }

  void push_active_index(int k) {
    active_indices.push_back(k);
    adjacent_attr_sums.resize(active_indices.size());
  }

  void resize(int n) {
    active_indices.resize(n);
    adjacent_attr_sums.resize(n);
  }

  bool clear_;

  // the set of indices which became active in refinement
  vector<int> active_indices;

  // pair of an index and the attr_sum the affected index had in it
  typedef pair<int, typename graph_t::attr_sum> index_attr_sum;

  // for each affected index, stores the list list of index_sums
  vector<vector<index_attr_sum> > adjacent_attr_sums;
};

template<typename T>
static int compare(const T &a, const T &b) {
  if (a < b) {
    return -1;
  } else if (a > b) {
    return 1;
  }

  return 0;
}

template<typename graph_t>
int RefineTraceValue<graph_t>::cmp(const RefineTraceValue<graph_t> &b) {
  for (int i = 0; i < active_indices.size(); i++) {
    // if there are not enough active indices in the other
    if (b.active_indices.size() <= i) {
      return 1;
    }

    if (active_indices[i] != b.active_indices[i]) {
      return compare(active_indices[i], b.active_indices[i]);
    }

    const vector<index_attr_sum> &attr_sums = adjacent_attr_sums.at(i);
    const vector<index_attr_sum> &b_attr_sums = b.adjacent_attr_sums.at(i);

    for (int j = 0; j < attr_sums.size(); j++) {
      // if there are not enough adjacent nbhr sums
      if (b_attr_sums.size() <= j) {
        return 1;
      }

      if (attr_sums[j] != b_attr_sums[j]) {
        return compare(attr_sums[j], b_attr_sums[j]);
      }
    }

    // if we still have more in b
    if (attr_sums.size() < b_attr_sums.size()) {
      return -1;
    }
  }

  if (active_indices.size() < b.active_indices.size()) {
    return -1;
  }

  return 0;
}

#endif  // INCLUDE_NISHE_REFINETRACEVALUE_H_
