#ifndef INCLUDE_NISHE_PARTITIONNEST_H_
#define INCLUDE_NISHE_PARTITIONNEST_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <ostream>
#include <istream>

#include <vector>
#include <string>
#include <deque>
#include <utility>

using std::vector;
using std::pair;
using std::deque;
using std::string;
using std::ostream;
using std::istream;

namespace nishe {

/*
 * This class represents a stack of finer ordered partitions pi_0, ..., pi_k.
 * Here, k is the level of the nest and all operations access/modify pi_k.
 */
class PartitionNest {
 public:

  PartitionNest();
  explicit PartitionNest(int size);

  // sets this partition nest to the unit partition of size n
  // [ 0 1 ... n - 1 ]
  int unit(int n);

  int size() const;  // number of elements
  int length() const;  // number of cells at the current level
  int cell_size(int k) const;  // size of cell at index k

  int index_containing(int u) const;  // the index containing the element u
  int *elements();  // returns a pointer to the elements

  int level();  // the current level of the partition nest

  // returns true if and only if k is the index of a cell
  bool is_index(int k) const;

  bool is_nontrivial_index(int k) const;

  // return true when each cell has size one
  bool is_discrete() const;

  // returns true if the cells are the same but (possible) in different order
  bool is_equal_unordered(const PartitionNest &pi) const;

  // returns the next index greater than k
  // terminal_index() if no such index exists
  int next_index(int k) const;

  // when iterating, this is the last index
  int terminal_index() const;

  // returns the first nontrivial index, terminal_index() otherwise
  int first_nontrivial_index() const;

  // returns the next nontrivial index, terminal_index() otherwise
  int next_nontrivial_index(int k) const;

  // creates a new cell at index k
  // returns whether k was an index already or not
  bool add_index(int k);

  // queue's a new cell to start at index k, all enqueue's must be in order
  // within each cell for commit_splits to work
  bool enqueue_new_index(int k);

  // commits all previous calls to enqueue_new_index(k) and makes them indices
  void commit_pending_indices();

  // advances the level by 1 and returns the new current level
  int advance_level();

  // pops to level m and returns the new current level
  int recover_level(int m);

  int backup_level();  // recovers one level

  // splits the cell u is in, returns true if its cell was not trivial
  bool breakout(int u);

  void input_string(string s);
  vector<int> operator[](int k) const;
  string str() const;
  /*    virtual void push_all_levels() = 0;
   virtual void push_level() = 0;
   virtual void pop_to_level(int n) = 0;
   virtual void clear_pop_stack() = 0; */

 private:
  // the elements of the partition nest at the current level
  vector<int> elements_;

  vector<int> index_containing_;  // the index_containing() lookup
  vector<int> cell_sizes_;  // the cell_size() lookup

  deque<int> new_index_queue_;

  // a new index is a tuple (s, k) where
  // s is the start of the cell containing the index k
  // k is the index of the new cell
  vector<pair<int, int> > new_indices_;

  // specifies the number of new indices at each level
  vector<int> new_indices_at_level_;

  // a doubly linked list keeping track of the nontrivial cells
  vector<pair<int, int> > nontrivial_list_;

  void commit_index(int nStart, int k, int k_next);
  void insert_index(int nStart, int k, int nStartSize);
  void erase_index(int nStart, int k, int nStartSize, int k_size);
};


ostream &operator<< (ostream &out, const PartitionNest &pi);
istream &operator>> (istream &in, PartitionNest &pi);

}  // namespace nishe

#endif  // INCLUDE_NISHE_PARTITIONNEST_H_
