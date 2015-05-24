# Introduction #

This is where pseudocode will go before it graduates to real code. Some parts are missing/left out with only the most salient points remaining.

# Background #

The problem we're trying to solve is graph isomorphism and one of the fastest ways to do this turns out to be via canonical labeling (even though it may be that canonical labeling of graphs is harder than graph isomorphism). So, the problem is given a graph, select a canonical representative from all those isomorphic to it. That is, compute a function `CR(G)` which has the property that `CR(G)` is isomorphic to `G` and that `CR(G) = CR(G')` where `G'` is any isomorph of `G`.

## Canonical labeling approaches ##

Here is a progression of canonical labeling methods, building up to what we'd like to achieve. A an ordered vertex partition is represented by `pi` or `nu` and `pi.breakout(u)` turns the cell `W` containing `u` into `{u, W - u}`. Graphs are ordered by the binary representation of their adjacency matrices. It doesn't matter how (make an n^2-bit binary number), just as long as the ordering exists. A partition for which each cell has size one is called "discrete".

The goal is to find a canonical representative of a graph's isomorphism class. This is commonly and easily extended to canonically labeling vertex-colored graphs via passing in an ordered partition as a second parameter.

For brevity's sake, the calculation of symmetries and pruning via symmetries is swept under the rug.

### Brute force (with refinement) ###

The most naive approach is to choose the graph whose adjacency matrix is smallest (smallest is arbitrary, but we're going to stick with it). This can be improved by performing refinement:

```
def CR(G, pi):
  nu = R(G, pi)

  if nu.is_discrete():
    return G**nu

  k = tc(G, nu)
  G_min = G

  for u in nu[k]:
    R = CR(G, nu.breakout(u))  # R for Representative
    if R < G_min:
      G_min = R

  return G_min
```

This has the advantage that we don't need to look at every adjacency matrix, only those which make sense with respect to refinement, and is a huge improvement.

### Using partial certificates ###

The next step, used by nauty (and descendants) uses partial certificates in an attempt to prune the search at an early stage using partial certificates (see the _bliss_ paper, or imagine a trace of the refinement operation). Because of this an auxiliary structure is needed to store the partial certificates at each level:
```
class GuideNode:
  def __init__(self):
    self.partial_certificate = None  # None is the largest partial certificate

  def child(self):
    # allocate a child if none exists
```

Furthermore, we assume that an additional output, the partial certificate, is returned by the refiner.

```
def CR(G, pi, curr):
  i, nu = R(G, pi)  # i for invariant... see bliss paper

  if i <= curr.partial_certificate:
    curr.partial_certificate = i
  else:
    return None

  if nu.is_discrete():
    return G**nu

  k = tc(G, nu)
  G_min = G
  
  for u in nu[k]:
    R = CR(G, nu.breakout(u), curr.child())  # R for Representative
    if R is not None:
      G_min = R

  return G_min
```

This allows for pruning based on the partial certificates and is pretty close to what is being used in nauty, bliss, saucy, and nishe today.

### Adaptive refinement (purifying cells) ###

Whenever a partial certificate is found to be different that what's expected, the refiner has failed to split a target-cell completely (there exist a least two elements in the cell for which no automorphism moves one to the other; such a cell is called _impure_). So, we should try to help it out a little more and split based on the partial certificate values. At a high level this is:
```
def CR(G, pi):
  nu = R(G, pi)
  
  if nu.is_discrete():
    return G**nu

  k = tc(G, pi)

  # if a cell is pure, then for all u, v in nu[k],
  #   CR(G, nu.breakout(u)) == CR(G, nu.breakout(v))
  if nu[k] is pure:
    return CR(G, nu.breakout(u))  # where u is any element of nu[k]
  
  # otherwise,
  # split the cell nu[k] based on the different canonical forms of its children
  return CR(G, split(u -> CR(G, nu.breakout(u)), nu)
```

The above embodies the idea of "adaptive refinement" presented in my thesis. The drawback is that in the above, the values of `CR(G, split(u -> CR(G, nu.breakout(u))), nu)` can only be found via a full traversal. Often, a full traversal is not needed, and the impure cell can be split just by looking a few levels down to find a difference in partial certificates.

Note that if `Aut(G, pi)` is nontrivial, then eventually a cell will be purified.

# Goal #

Graph isomorphism algorithms run slowly because the refiner can fail to split vertices which should be in different orbits. A cell of an ordered partition is _impure_ if it contains multiple orbits. The goal here is to find when a cell is impure, and purify it. This is ideally extendable to using automorphisms in target-cell computation. It's the next logical place to explore after my thesis work.

# Overview #

The best way of starting this seems to be via recursion. The insight is that we're not taking advantage of the fact that whenever an invariant is not what's seen before, we know exactly which cell contains multiple orbits. This cell should be somehow purified. So, the algorithm sort of goes "try to see if we have an impure cell, if so then purify it."

The data-structure that will be used to aid the search is called a guide tree (which consists of guide nodes). Right now it's sort of a guide path with a possible filtered child, but later on when I figure out how to incorporate using automorphisms in target-cell choosing, it'll be a tree.

```
class GuideNode:  
  guide_level  # depth in guide tree
  search_level  # of elements fixed to get here
  partial_certificate  # partial leaf certificate calculated in the refiner
  child  # result of a breakout
  filtered_child  # after filtering
  parent  # either from child or filtered_child
  discriminator  # non-null implies that discriminator is a descendant of parent for which parent's impurity becomes visible
  first_branch  # first branch used to reach this, -1 if we are someone's filtered child
  labeling  # labeling of leaf, only valid if we represent a leaf
```

# The search for a canonical labeling #

Two different things can happen to return from the search function.
  * An impurity is found: something "unexpected" happened according to the guide tree, or
  * A return to a previous level. This could be due to either an automorphism being discovered or by exhausting the target cell's branches.

```
class SearchResult:
  def __init__(self, new_curr)
    self.new_curr = new_curr  # the new current guide node

  def is_backup():
    return not self.is_impurity()

  def is_impurity():
    return not self.is_backup()


class Backup(SearchResult):
  def __init__(self, new_curr):
    SearchResult.__init__(new_curr)
    
  def is_backup():
    return True


class Impurity(SearchResult):
  def __init__(self, new_curr, discriminator, cmp):
    SearchResult.__init__(new_curr)
    self.discriminator = discriminator  # the guide node at which an impurity was found
    self.cmp = cmp  # the comparison value of the difference (either -1 or 1)

  def is_impurity():
    return True
```

## Pseudocode ##

The search function will be described recursively at first, because it seems the most natural way. It has the view that it's given a guide node, a partition, the active index in the partition, and a discriminator, which says how far a search needs to go before an impurity should be discovered. The variable `fp` stands for the fixed path of `nu`.

```
def search(curr, nu, active_index, discriminator):
  # first refine and compare certificates
  cmp = refine(G, nu, curr.partial_certificate, active_index)
  
  # if the partial certificate doesn't match, we have an impurity
  if cmp != 0:
    return Impurity(gca(curr, fp), curr, cmp)

  if nu.is_discrete():
    if curr.labeling is None:
      curr.labeling = nu
      return Backup(curr.parent)
    else:
      # find automorphism
      return Backup(gca(curr, fp))

  # find and check the target cell
  tcell = tc(G, nu)
  cmp = curr.compare_with_expected_tcell(tcell)
  if cmp != 0:
    return Impurity(gca(curr, fp), curr, cmp))

  # if this guide node has a filtered child
  if curr.filtered_child is not None:
    res = filter(curr, nu, curr.filtered_child.discriminator)
    # can't filter when we should have been able to
    if res.cmp != 0:
      return Impurity(gca(curr, fp), curr, -1)  # -1 could have been 1
    
    # otherwise the filtering worked, so resume searching at the filtered child
    return search(res.new_curr, nu, res.active_index, res.new_curr.discriminator)

  child = curr.child()
  child.initialize_branches(nu)

  while child.next_branch() != -1:
    # update the first branch if it's the first time
    if child.first_branch == -1:
      child.first_branch = b

    nu_b = nu.breakout(b)

    # search from the child now
    res = search(child, nu_b, nu_b.index_containing(b))

    # if a backup was returned and it's to an ancestor, propagate it
    if res.backup:
      if curr.guide_level > res.new_curr.guide_level:
        return res
    # otherwise if an ancestor's tcell was determined to be impure
    else if res.impurity:
      if curr.guide_level > res.new_curr.guide_level:
        return res

      # otherwise, this is the first we learn that we're impure, so filter
      res = filter(curr, nu)
      return search(res.new_curr, res.new_nu, res.active_index, res.discriminator)

  return Backup(curr.parent)
```

The above search function makes use of another function, `filter`, so named because it purifies a cell. To purify a cell, one must begin a search via breaking out each element of that cell, and search to the discriminator, at which point it will be determined if the element should be less than, equal to, or greater than the smallest that has so far been seen. Only the boundary between those elements which are minimal and those which are not is considered, because as my dissertation showed, it is probably too expensive to do full sorting.

```
def filter(curr, nu, discriminator):
  # try to split, updating curr's discriminator along the way
  
  if (no split):
    // find location where we must have an impure cell
    return Impurity(impure_node, curr, -1)

  return (i dunno what yet)
```


## Appendix ##


The greatest common ancestor of a node and a fixed path can be computed as follows:

```
  def gca(curr, fp):
    path = stack()
    
    # find the root of the guide tree
    while curr.parent is not None:
      path.push(curr)

    # eat filtered children   
    while cur.filtered_child is not None:
      curr = path.pop()

    # go until you find the first point at which they differ
    i = 0
    while curr.first_branch == fp[i]:
      curr = path.pop()
      i += 1

      # eating filtered children along the way
      while curr.filtered_child is not None:
        curr = path.pop()

    # we found a difference in first_branch, so return the parent
    return curr.parent
```