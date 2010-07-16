/*
 Copyright 2010 Greg Tener
 Released under the Lesser General Public License v3.
 */

#include <nishe/PartitionNest.h>
#include <nishe/Util.h>

#include <ostream>
#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <set>
#include <string>
#include <cstdio>

using std::make_pair;
using std::cout;
using std::endl;

namespace nishe {

PartitionNest::PartitionNest() {
}

PartitionNest::PartitionNest(int size) {
  unit(size);
}

int PartitionNest::unit(int n) {
  int i = 0;

  if (n == 0) {
    return 0;
  }

  // set the elements to the integers from 0 to n - 1
  // the index containing to 0 for all
  // and the cell sizes to n for the 0th index
  elements_.resize(n);
  index_containing_.resize(n);

  for (i = 0; i < n; i++) {
    elements_[i] = i;
    index_containing_[i] = 0;
  }

  cell_sizes_.resize(n);
  cell_sizes_[0] = n;

  // clear all pending new indices and the stack
  new_index_queue_.clear();
  new_indices_.clear();
  new_indices_at_level_.clear();

  // start off with no new indices at level 0
  new_indices_at_level_.push_back(0);

  // reset the nontrivial list
  // the first one points to the last element, and vice versa,
  // this is okay because n - 1 cannot ever be a nontrivial index
  nontrivial_list_.resize(n);
  nontrivial_list_.back() = make_pair(0, 0);
  nontrivial_list_[0] = make_pair(n - 1, n - 1);

  return n;
}

int PartitionNest::size() const {
  return elements_.size();
}

int PartitionNest::length() const {
  if (size() == 0) {
    return 0;
  }

  return new_indices_.size() + 1;
}

int PartitionNest::cell_size(int k) const {
  return cell_sizes_[k];
}

int PartitionNest::index_containing(int u) const {
  return index_containing_[u];
}

int *PartitionNest::elements() {
  return &elements_[0];
}

int PartitionNest::level() {
  return new_indices_at_level_.size() - 1;
}

bool PartitionNest::is_index(int k) const {
  return index_containing_[elements_[k]] == k;
}

bool PartitionNest::is_nontrivial_index(int k) const {
  return is_index(k) && cell_sizes_[k] > 1;
}

bool PartitionNest::is_discrete() const {
  return length() == size();
}

bool PartitionNest::is_equal_unordered(const PartitionNest &pi) const {
  // check basic size/length requirements for equality
  if (pi.size() != size() || pi.length() != length()) {
    return false;
  }

  // now check that each nontrivial cell is found in the other
  // this necessitates that the trivial cells must be found in the other
  // as well

  int k = first_nontrivial_index();

  while (k != terminal_index()) {
    int u = elements_[k];
    int k_pi = pi.index_containing(u);

    // cell sizes must be equal
    if (cell_size(k) != pi.cell_size(k_pi)) {
      return false;
    }

    for (int i = k + 1; i < k + cell_size(k); i++) {
      u = elements_[i];

      if (k_pi != pi.index_containing(u)) {
        return false;
      }
    }

    k = next_nontrivial_index(k);
  }

  return true;
}

int PartitionNest::next_index(int k) const {
  return k + cell_sizes_[k];
}

int PartitionNest::terminal_index() const {
  return size();
}

int PartitionNest::first_nontrivial_index() const {
  int second = nontrivial_list_[size() - 1].second;

  if (second < size() - 1) {
    return second;
  }

  return terminal_index();
}

int PartitionNest::next_nontrivial_index(int k) const {
  int second = nontrivial_list_[k].second;

  if (second < size() - 1) {
    return second;
  }

  return terminal_index();
}

bool PartitionNest::add_index(int k) {
  bool rv = enqueue_new_index(k);
  commit_pending_indices();

  return rv;
}

bool PartitionNest::enqueue_new_index(int k) {
  if (!is_index(k)) {
    new_index_queue_.push_back(k);

    return true;
  }

  return false;
}

void PartitionNest::commit_pending_indices() {
  int i = 0;

  while (i < new_index_queue_.size()) {
    int k_next = 0;  // the next place to add a split
    int k = new_index_queue_[i];
    int u = elements_[k];

    // the cell containing the run of splits
    int first = index_containing_[u];
    int first_size = cell_sizes_[first];

    // a moving starting point of the current cell we're adding a split to
    int start = first;

    while (i + 1 < new_index_queue_.size()) {
      k_next = new_index_queue_[i + 1];
      u = elements_[k_next];

      // check that this index is in the same cell as all of the others
      if (index_containing_[u] != first) {
        break;
      }

      commit_index(start, k, k_next);

      start = k;
      k = k_next;
      i += 1;
    }

    k_next = first + first_size;
    commit_index(start, k, k_next);

    i += 1;
  }

  new_index_queue_.clear();
}

void PartitionNest::commit_index(int nStart, int k, int k_next) {
  int i = 0;
  int nStartSize = 0;

  // set the elements to be members of the new cell
  for (i = k; i < k_next; i++) {
    index_containing_[elements_[i]] = k;
  }

  // update the cell sizes
  nStartSize = cell_sizes_[nStart];
  cell_sizes_[nStart] = k - nStart;
  cell_sizes_[k] = nStartSize - cell_sizes_[nStart];

  // record this split
  new_indices_.push_back(make_pair(nStart, k));

  // increase the number of splits at this level
  new_indices_at_level_.back() += 1;

  // updates the vNontrivialList structure
  insert_index(nStart, k, nStartSize);
}

void PartitionNest::insert_index(int nStart, int k, int nStartSize) {
  pair<int, int> &pk = nontrivial_list_[k];
  pair<int, int> &pnStart = nontrivial_list_[nStart];
  pair<int, int> &pnNext = nontrivial_list_[pnStart.second];
  pair<int, int> &pnPrev = nontrivial_list_[pnStart.first];

  // case 1:
  // if k isn't nStart + 1 and k isn't at the end
  // (it's < nStart + nStartSize - 1)
  if (k > nStart + 1 && k < nStart + nStartSize - 1) {
    // join k and nStart's next
    pk.second = pnStart.second;
    pnNext.first = k;

    // join nStart and k
    pnStart.second = k;
    pk.first = nStart;
  }
  // case 2:
  // k is nStart + 1, and k is not at the end of the cell
  else if (k == nStart + 1 && k < nStart + nStartSize - 1) {  // NOLINT
    // k takes the place of nStart in the list
    pk = pnStart;
    pnPrev.second = k;
    pnNext.first = k;
  }
  // case 3:
  // k is nStart + 1, and k is at the end of the cell (nSizetSize == 2)
  else if (k == nStart + 1 && k == nStart + nStartSize - 1) {  // NOLINT
    // neither nStart nor k will be non-trivial after this
    // bypass them altogether
    pnPrev.second = pnStart.second;
    pnNext.first = pnStart.first;
  }
  // case 4:
  // k > nStart + 1, and k == nStart + nStartSize - 1
  // do nothing!!!
}

int PartitionNest::advance_level() {
  new_indices_at_level_.push_back(0);
}

int PartitionNest::recover_level(int m) {
  int split_count = 0;
  set<int> start_indices;
  vector<int> cell_end;
  vector<int> original_sizes;
  int k = 0;
  int k_size = 0;
  int k_temp = 0;
  int nStart = 0;
  int nStartSize = 0;
  int nEnd = 0;
  int i = 0;

  // the cell ends are 0
  cell_end.resize(size());
  new_index_queue_.clear();

  while (level() > m) {
    split_count += new_indices_at_level_.back();
    new_indices_at_level_.pop_back();
  }

  for (i = 0; i < split_count; i++) {
    nStart = new_indices_.back().first;
    k = new_indices_.back().second;
    new_indices_.pop_back();

    // record the sizes
    k_size = cell_sizes_[k];
    nStartSize = cell_sizes_[nStart];

    // update the nontrivial linked list
    erase_index(nStart, k, nStartSize, k_size);

    // update the sizes (k is removed, nStart is enlarged)
    cell_sizes_[k] = 0;
    cell_sizes_[nStart] += k_size;

    // manage keeping track of where nStart ends
    nEnd = cell_end[k];

    // if k has no end yet, end where k ended
    if (nEnd == 0) {
      nEnd = k + k_size - 1;
    } else {  // otherwise k isn't a start anymore
      cell_end[k] = 0;
      start_indices.erase(k);
    }

    start_indices.insert(nStart);
    cell_end[nStart] = nEnd;
  }

  // run through cStartIndices and update index containing
  set<int>::iterator it;

  for (it = start_indices.begin(); it != start_indices.end(); it++) {
    nStart = (*it);
    nEnd = cell_end[nStart];

    for (i = nStart; i <= nEnd; i++) {
      index_containing_[elements_[i]] = nStart;
    }
  }

  return level();
}

int PartitionNest::backup_level() {
  return recover_level(level() - 1);
}

bool PartitionNest::breakout(int u) {
  int k = 0;
  int *anElements = elements();

  k = index_containing(u);

  // don't do anything if there's no need to
  if (cell_size(k) <= 1) {
    fprintf(stderr, "Error Error Examine: %s %d %s %d: %s\n",
        "attempting to breakout", u, "from cell of size", cell_size(k),
        str().c_str());
    exit(1);
  }

  int i = k;

  for (; i < k + cell_size(k); i++) {
    if (anElements[i] == u) {
      break;
    }
  }

  // u should always be found, if not foul play is involved
  if (i == k + cell_size(k)) {
    fprintf(stderr, "Error Error Examine: %d %s %d of %s\n", u,
        "was not found at index", k, str().c_str());
    exit(1);
  }

  std::swap(anElements[k], anElements[i]);

  add_index(k + 1);

  return true;
}

void PartitionNest::erase_index(int nStart, int k, int nStartSize, int k_size) {
  int nStartPrev = 0;
  int nStartNext = 0;
  int k_next = 0;

  if (nStartSize == 1) {
    nStartPrev = nontrivial_list_[nStart].first;
    nStartNext = nontrivial_list_[nStart].second;
    nontrivial_list_[nStartPrev].second = nStart;
    nontrivial_list_[nStartNext].first = nStart;
  } else if (nStartSize > 1 && k_size > 1) {
    k_next = nontrivial_list_[k].second;
    nontrivial_list_[nStart].second = k_next;
    nontrivial_list_[k_next].first = nStart;
  }
}

void PartitionNest::input_string(string s) {
  stringstream ss(s);

  ss >> (*this);
}

vector<int> PartitionNest::operator[](int k) const {
  vector<int> v;
  int i = 0;
  int k_next = 0;

  k_next = next_index(k);

  for (i = k; i < k_next; i++) {
    v.push_back(elements_[i]);
  }

  sort(v.begin(), v.end());

  return v;
}

string PartitionNest::str() const {
  stringstream ss;
  ss << (*this);
  return ss.str();
}

ostream &operator<<(ostream &out, const PartitionNest &pi) {
  int k = 0;
  vector<int> v;

  k = 0;
  out << "[";

  // special case for n = 0
  if (pi.size() > 0) {
    out << " ";
  }

  while (k < pi.terminal_index()) {
    v = pi[k];
    out << range_compress(v);
    k = pi.next_index(k);

    if (k != pi.terminal_index()) {
      out << " | ";
    }
  }

  out << " ]";

  return out;
}

void process_token(string token, vector<int> *elements_ptr,
    vector<int> *indices_ptr, set<int> *element_set_ptr) {
  stringstream ss(token);

  if (token == "|") {
    // check that that we're not adding index 0, which is always there
    if (elements_ptr->size() == 0) {
      fprintf(stderr, "Error Error Examine: cannot add 0 as an index\n");
      exit(1);
    }

    if (indices_ptr->size() > 0 &&
        indices_ptr->back() == elements_ptr->size()) {
      fprintf(stderr, "%s: cannot add an index twice (| | occurred)\n",
          "Error Error Examine");
      exit(1);
    }

    indices_ptr->push_back(elements_ptr->size());
  } else if (token.find(':') != string::npos) {
    int low = 0;
    int high = 0;
    char c = 0;

    int field_count = sscanf(token.c_str(), "%d:%d", &low, &high);  // NOLINT

    // check that we read two ints,
    // that they're both nonnegative and increasing
    if (field_count < 2 || low < 0 || high < 0 || high <= low) {
      fprintf(stderr, "%s: %s but got %s instead\n", "Error Error Examine",
          "expected <m>:<n> (increasing, nonnegative)", token.c_str());
      exit(1);
    }

    for (int i = low; i <= high; i++) {
      elements_ptr->push_back(i);
      element_set_ptr->insert(i);
    }
  } else {  // otherwise we should be reading in an integer
    int m = 0;

    int field_count = sscanf(token.c_str(), "%d", &m);  // NOLINT

    if (field_count == 0 || m < 0) {
      fprintf(stderr, "%s: expected a nonnegative integer, got %d instead\n",
          "Error Error Examine", m);
      exit(1);
    }

    elements_ptr->push_back(m);
    element_set_ptr->insert(m);
  }
}

// read in a partition like [ 1 | 0:3 | 4 5 ]
// clears what might already be here as well
istream &operator>>(istream &in, PartitionNest &pi) {
  char c = 0;

  // read in the [
  in >> c;

  if (in.fail()) {
    fprintf(stderr,
        "Error Error Examine: couldn't read character from stream\n");
    exit(1);
  }

  if (c != '[') {
    fprintf(stderr, "Error Error Examine: expected [ but got %c instead\n", c);
    exit(1);
  }

  string sToken = "[";
  vector<int> indices;
  vector<int> elements;
  set<int> element_set;  // to check that the elements are 0 ... n - 1
  int nIndex = 0;

  in >> sToken;

  // process the stream one token at a time until ] is found
  while (sToken != "]") {
    process_token(sToken, &elements, &indices, &element_set);
    in >> sToken;

    if (in.fail()) {
      fprintf(stderr, "Error Error Examine: error reading token\n");
      exit(1);
    }
  }

  // check that the elements are 0 ... n - 1
  for (int i = 0; i < elements.size(); i++) {
    if (element_set.find(i) == element_set.end()) {
      fprintf(stderr, "%s: %s {0, ..., %d} but %d is missing\n",
          "Error Error Examine", "element set should be", elements.size() - 1,
          i);
      exit(1);
    }
  }

  // check that n - 1 is not an index
  if (indices.size() > 0 && indices.back() == elements.size()) {
    fprintf(stderr, "%s: n == %d cannot be an index\n", "Error Error Examine",
        elements.size() - 1, elements.size());
    exit(1);
  }

  // now set pi
  pi.unit(elements.size());

  for (int i = 0; i < elements.size(); i++) {
    pi.elements()[i] = elements[i];
  }

  for (int i = 0; i < indices.size(); i++) {
    pi.enqueue_new_index(indices[i]);
  }

  pi.commit_pending_indices();
}

}  // namespace nishe
