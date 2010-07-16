/*
 Copyright 2010 Greg Tener
 Released under the Lesser General Public License v3.
 */

#include <nishe/DirectedGraph.h>

#include <utility>
#include <algorithm>

using std::pair;
using std::make_pair;

namespace nishe {

const size_t DirectedGraph::IN = 0;
const size_t DirectedGraph::OUT = 1;
const size_t DirectedGraph::BOTH = 2;

InOutBoth &InOutBoth::operator+=(int k) {
  weights[k] += 1;

  return *this;
}

InOutBoth &InOutBoth::operator=(int k) {
  weights[DirectedGraph::IN] = k;
  weights[DirectedGraph::OUT] = k;
  weights[DirectedGraph::BOTH] = k;

  return *this;
}

bool InOutBoth::operator<(const InOutBoth &a) const {
  for (int i = 0; i < sizeof(weights) / sizeof(*weights); i++) {
    if (weights[i] < a.weights[i]) {
      return true;
    } else if (weights[i] > a.weights[i]) {
      return false;
    }
  }

  return false;
}

bool InOutBoth::operator!=(const InOutBoth &a) const {
  for (int i = 0; i < sizeof(weights) / sizeof(*weights); i++) {
    if (weights[i] != a.weights[i]) {
      return true;
    }
  }

  return false;
}

bool InOutBoth::operator==(const InOutBoth &a) const {
  for (int i = 0; i < sizeof(weights) / sizeof(*weights); i++) {
    if (weights[i] != a.weights[i]) {
      return false;
    }
  }

  return true;
}

bool DirectedGraph::add_arc(vertex_t u, vertex_t v) {
  add_vertex(std::max(u, v));

  int k = find_nbhr(u, v);

  // we have a new edge
  if (k == NOT_FOUND) {
    // add the out edge
    vNbhds.at(u).push_back(make_pair(v, DirectedGraph::OUT));
    // add the in edge
    vNbhds.at(v).push_back(make_pair(u, DirectedGraph::IN));
  } else {  // this edge is already here
    // if the edge is not an incoming one, we can't add it
    if (vNbhds.at(u).at(k).second != DirectedGraph::IN) {
      return false;
    }

    // set u's to be both
    vNbhds.at(u).at(k).second = DirectedGraph::BOTH;

    // find where u is in v's neighborhood and set it to (both) as well
    vNbhds.at(v).at(find_nbhr(v, u)).second = DirectedGraph::BOTH;
  }

  return true;
}

}  // namespace nishe
