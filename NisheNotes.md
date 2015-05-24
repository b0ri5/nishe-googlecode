# Introduction #

The goal is to implement a very fast and portable canonical labeling (with implied symmetry detection) algorithm similar to _nauty_ and its descendants _bliss_ and _saucy_. It is called _nishe_ because calling it _nice_ is too cliche and _nishe_ sort of sounds like _nice_. There are several descriptions of the "standard" canonical labeling algorithm out there. Our favorite descriptions are (Junttila and Kaski 2007) and (obviously) (Tener 2009).

# Preliminaries #

The goal is to find a canonical labeling of a graph as well as generators of the graph's automorphism group. The term _graph_ is somewhat ambiguous because the concept of edge can vary. The edges could be directed, weighted, labeled, etc. One goal in implementing the algorithm is to design it to work on any type of graph that is appropriately defined.

Here are the basics:
  * Sym(_V_) denotes the set of all permutations of _V_
  * Elements of Sym(_V_) are denoted by _g_
  * A _graph_ _G_ is an ordered pair (_V_, _E_) which consists of
    * the vertex set _V_
    * the edge set _E_
      * consists of ordered triplets (_u_, _v_, _a_) meaning that _u_ has a neighbor _v_ with attribute _a_
      * attributes must be totally ordered
    * the action of Sym(_V_) on a graph _G_ = (_V_, _E_) is _G<sup>g</sup>_ = (_V_, { (_u<sup>g</sup>_, _v<sup>g</sup>_, _a_) : (_u_, _v_, _a_) \in _E_ } )
  * An _ordered set partition_ (simply _partition_ henceforth) _pi_ = (_W<sub>1</sub>_, .., _W<sub>k</sub>_) of a vertex set _V_ is an ordered sequence of subsets _W<sub>i</sub>_ for 1 <= _i_ <= _k_ of _V_ which are disjoint and union to _V_.
    * each _W<sub>i</sub>_ is called a _cell_ of _pi_
    * _k_ is the _length_ of _pi_
    * if _W<sub>i</sub>_ has cardinality one it is called _trivial (nontrivial otherwise)
    * if each_W<sub>i</sub>_is trivial then_pi_is called_discrete_* the action of Sym(_V_) on a partition_pi_= (_W<sub>1</sub>_, ..,_W<sub>k</sub>_) is_pi<sup>g</sup>_= (_W<sub>1</sub><sup>g</sup>_, ..,_W<sub>k</sub><sup>g</sup>_)
  * A colored graph is a pair (_G_,_pi_)
  * Aut(_G_,_pi_) = {_g_\in Sym(_V_) : (_G<sup>g</sup>_,_pi<sup>g</sup>_) = (_G_,_pi_) }
  * Orbit(_G_,_pi_,_v_) = {_v<sup>g</sup>_:_g_\in Aut(_G_) } is the orbit of_v_in Aut(_G_,_pi_)_


# The refinement algorithm #

One of the most basic operations of any isomorphism algorithm is the refinement operation. Given a colored graph (_G_, _pi_) The refinement operation attempts to split vertices which cannot be in the same orbit of Aut(_G_, _pi_). It does this by examining the elements of a cell, and the attributes of their neighbors within another cell. If they are determined to be different, _u_ and _v_ should be split up into different cells.

```
def refine(G, pi):
  active_indices = indices(pi)
  attr_sums = [0 for u in V]
    
  for active_index in active_indices:
    adjacent_indices = set()

    # sow the active cell
    for (v, a) in G.nbhrs(u):
      attr_sums[v] += a
      adjacent_indices.add(pi.index_containing(v) )

    # sort and split the adjacent indices
    for adjacent_index in adjacent_indices:
      sort(attr_sums, k, k + pi.cell_size(k), key = lambda u: attr_sums[u] )
      
      # split pi[k] where the attr_sums differ

  return pi 
```