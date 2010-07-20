/*
 Copyright 2010 Greg Tener
 Released under the Lesser General Public License v3.
 */

#include <nishe/GraphIO-inl.h>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <string>

namespace nishe {

void fail(string err) {
  fprintf(stderr, "%s: %s\n", "Error Error Examine", err.c_str());
  exit(1);
}

static bool add_edge(BasicGraph *G_ptr, vertex_t u, string token) {
  std::stringstream ss(token);
  vertex_t v = 0;

  ss >> v;

  if (ss.fail() || token.size() != ss.tellg()) {
    fail("expected vertex (nonnegative integer) got " + token + " instead");
  }

  return G_ptr->add_edge(u, v);
}

static bool add_arc(DirectedGraph *G_ptr, vertex_t u, string token) {
  std::stringstream ss(token);
  vertex_t v = 0;

  ss >> v;

  if (ss.fail() || token.size() != ss.tellg()) {
    fail("expected vertex (nonnegative integer) got " + token + " instead");
  }

  return G_ptr->add_arc(u, v);
}

// token should be of the form %d,%d
static bool add_weighted_edge(IntegerWeightedGraph *G_ptr, vertex_t u,
    string token) {
  std::stringstream ss(token);
  vertex_t v = 0;
  int weight = 0;
  char comma = 0;

  string err = "expected <v>,<w> token for integer weighted edge, ";
  err += "got " + token + " instead";

  ss >> v;

  if (ss.fail()) {
    fail(err);
  }

  ss >> comma;

  if (comma != ',') {
    fail(err);
  }

  ss >> weight;

  if (ss.fail() || token.size() != ss.tellg()) {
    fail(err);
  }

  return G_ptr->add_weighted_edge(u, v, weight);
}

void output_nbhr_basic_graph(ostream &out, const BasicGraph::nbhr &u) {
  out << u;
}

void output_nbhr_directed_graph(ostream &out, const DirectedGraph::nbhr &nbhr) {
  if (nbhr.second == DirectedGraph::OUT) {
    out << nbhr.first;
  }
}

void output_nbhr_integer_weighted_graph(ostream &out,
    const DirectedGraph::nbhr &nbhr) {
  out << nbhr.first << "," << nbhr.second;
}

bool GraphIO::input_list_ascii(istream &in, BasicGraph *G_ptr,
    PartitionNest *pPi) {
  GraphIO::input_list_ascii(in, G_ptr, pPi, add_edge);
}

bool GraphIO::input_list_ascii(istream &in, DirectedGraph *G_ptr,
    PartitionNest *pPi) {
  GraphIO::input_list_ascii(in, G_ptr, pPi, add_arc);
}

bool GraphIO::input_list_ascii(istream &in, IntegerWeightedGraph *G_ptr,
    PartitionNest *pPi) {
  GraphIO::input_list_ascii(in, G_ptr, pPi, add_weighted_edge);
}

void GraphIO::output_list_ascii(ostream &out, const BasicGraph &G) {
  GraphIO::output_list_ascii(out, G, output_nbhr_basic_graph);
}

void GraphIO::output_list_ascii(ostream &out, const DirectedGraph &G) {
  GraphIO::output_list_ascii(out, G, output_nbhr_directed_graph);
}

void GraphIO::output_list_ascii(ostream &out, const IntegerWeightedGraph &G) {
  GraphIO::output_list_ascii(out, G, output_nbhr_integer_weighted_graph);
}

// ignore the arc type and just add the edge
static void directed2basic(vertex_t u, vertex_t v,
    const DirectedGraph::attr &attr, BasicGraph *G_ptr) {
  G_ptr->add_edge(u, v);
}

void GraphIO::convert(const DirectedGraph &directed, BasicGraph *basic_ptr) {
  convert(directed, basic_ptr, directed2basic);
}

// use the arc type as the weight
static void directed2integer_weighted(vertex_t u, vertex_t v,
    const DirectedGraph::attr &attr, IntegerWeightedGraph *G_ptr) {
  G_ptr->add_weighted_arc(u, v, attr);
}

void GraphIO::convert(const DirectedGraph &directed,
    IntegerWeightedGraph *integer_weighted_ptr) {
  convert(directed, integer_weighted_ptr, directed2integer_weighted);
}

void GraphIO::path(BasicGraph *G_ptr, int n) {
  G_ptr->clear();

  for (int u = 0; u < n - 1; u++) {
    G_ptr->add_edge(u, u + 1);
  }
}

void GraphIO::path(BasicGraph *G_ptr, PartitionNest *pPi, int n) {
  path(G_ptr, n);
  pPi->unit(n);
}

void GraphIO::directed_path(DirectedGraph *G_ptr, int n) {
  G_ptr->clear();

  for (int u = 0; u < n - 1; u++) {
    G_ptr->add_arc(u, u + 1);
  }
}

void GraphIO::directed_path(DirectedGraph *G_ptr, PartitionNest *pPi, int n) {
  directed_path(G_ptr, n);
  pPi->unit(n);
}

}  // namespace nishe
