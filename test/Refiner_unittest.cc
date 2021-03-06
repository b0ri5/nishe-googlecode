/*
 Copyright 2010 Greg Tener
 Released under the Lesser General Public License v3.
 */

#include <nishe/test/BaseNisheTest.h>

#include <nishe/Graphs.h>
#include <nishe/Refiner-inl.h>
#include <nishe/RefineTraceValue.h>
#include <nishe/GraphIO-inl.h>

#include <gtest/gtest.h>

#include <fstream>
#include <utility>
#include <map>
#include <sstream>

using std::make_pair;
using std::map;
using std::stringstream;
using std::ifstream;

namespace nishe {

class RefinerTest: public BaseNisheTest {
 public:

  template <typename graph_a, typename graph_b>
  void verify_converted_equitibility(string graphfile,
    void (*convert)(const graph_a &, graph_b *) = NULL )
  {
    ifstream in;
    ifstream in_equitable;

    int dotloc = graphfile.rfind('.');

    string graphfile_equitable = graphfile.substr(0, graphfile.rfind('.') )
      + "_equitable.txt";

    in.open(graphfile.c_str() );

    if (in.fail() )
    {
      fprintf(stderr, "couldn't open data file %s\n", graphfile.c_str() );
      exit(1);
    }

    in_equitable.open(graphfile_equitable.c_str() );

    if (in_equitable.fail() )
    {
      fprintf(stderr, "couldn't open data file %s\n",
        graphfile_equitable.c_str() );
      exit(1);
    }

    PartitionNest pi;
    PartitionNest pi_equitable;

    graph_a G_a;
    RefineTraceValue<graph_a> trace_a;
    Refiner<graph_a> refiner_a;

    graph_b G_b;
    RefineTraceValue<graph_b> trace_b;
    Refiner<graph_b> refiner_b;

    bool successful_read = GraphIO::input_list_ascii(in, &G_a, &pi);

    while (successful_read)
    {
      in_equitable >> pi_equitable;

      if (convert != NULL)
      {
        convert(G_a, &G_b);
        refiner_b.refine(G_b, &pi, &trace_b);
        trace_b.clear();
      }
      else
      {
        refiner_a.refine(G_a, &pi, &trace_a);
        trace_a.clear();

      }

      ASSERT_TRUE(pi.is_equal_unordered(pi_equitable) );

      successful_read = GraphIO::input_list_ascii(in, &G_a, &pi);
    }

    in.close();
    in_equitable.close();
  }

  template <typename graph_t>
  void verify_equitibility(string graphfile)
  {
    return verify_converted_equitibility<graph_t, BasicGraph>(graphfile);
  }
};

TEST_F(RefinerTest, RefineTraceValueCompare) {
  typedef RefineTraceValue<BasicGraph> BasicRefineTraceValue;
  BasicRefineTraceValue a;
  BasicRefineTraceValue b;

  EXPECT_EQ(0, a.cmp(b) );

  // ensure if 'b' is too short that 'a' is larger
  a.active_indices.push_back(1);
  EXPECT_EQ(1, a.cmp(b) );
  EXPECT_EQ(-1, b.cmp(a) );

  // now change the indices
  b.active_indices.push_back(0);
  EXPECT_EQ(1, a.cmp(b) );
  EXPECT_EQ(-1, b.cmp(a) );

  // now add some nbhr sums
  a.adjacent_attr_sums.resize(1);
  a.adjacent_attr_sums[0].push_back(
      BasicRefineTraceValue::index_attr_sum(1, 1) );
  b.active_indices[0] = 1;
  b.adjacent_attr_sums.resize(1);

  // now we have
  // a - 1: 1,1
  // b - 1:
  EXPECT_EQ(1, a.cmp(b) );
  EXPECT_EQ(-1, b.cmp(a) );

  b.adjacent_attr_sums[0].push_back(
      BasicRefineTraceValue::index_attr_sum(1, 1) );
  EXPECT_EQ(0, a.cmp(b) );

  b.adjacent_attr_sums[0][0].first = 0;

  // a - 1: 1,1
  // b - 1: 0,1
  EXPECT_EQ(1, a.cmp(b) );
  EXPECT_EQ(-1, b.cmp(a) );

  b.adjacent_attr_sums[0][0].first = 1;
  b.adjacent_attr_sums[0].push_back(
      BasicRefineTraceValue::index_attr_sum(2, 1) );

  // a - 1: 1,1
  // b - 1: 1,1 2,1
  EXPECT_EQ(-1, a.cmp(b) );
  EXPECT_EQ(1, b.cmp(a) );

  a.adjacent_attr_sums[0].push_back(
      BasicRefineTraceValue::index_attr_sum(2, 1) );
  EXPECT_EQ(0, a.cmp(b) );
}

// test out is_equitable on different graphs
TEST_F(RefinerTest, IsEquitableBasicGraph) {
  GraphIO::null(&basic_graph, 1);
  EXPECT_TRUE(is_equitable(basic_graph, pi) );

  GraphIO::path(&basic_graph, &pi, 3);

  EXPECT_FALSE(is_equitable(basic_graph, pi) );

  pi.advance_level();
  pi.breakout(1);

  // pi == [ 1 | 0 2 ]
  EXPECT_TRUE(is_equitable(basic_graph, pi) );

  pi.recover_level(0);
  pi.advance_level();
  pi.breakout(2);

  // pi == [ 2 | 0 1 ]
  EXPECT_FALSE(is_equitable(basic_graph, pi) );

  pi.breakout(1);

  // pi == [ 2 | 1 | 0 ]
  EXPECT_TRUE(is_equitable(basic_graph, pi) );
}

TEST_F(RefinerTest, IsEquitableDirectedGraph) {
  GraphIO::directed_path(&directed_graph, &pi, 2);
  EXPECT_FALSE(is_equitable(directed_graph, pi) );

  pi.advance_level();
  pi.breakout(1);
  // pi == [ 1 | 0 ]
  EXPECT_TRUE(is_equitable(directed_graph, pi) );
}

TEST_F(RefinerTest, IsEquitableIntegerWeightedGraph) {
  GraphIO::input_list_ascii("0 : 1,0 ;\n1 : 2,1 ;",
      &integer_weighted_graph, &pi);
  EXPECT_FALSE(is_equitable(integer_weighted_graph, pi) );

  pi.advance_level();
  pi.breakout(1);
  // pi == [ 1 | 0 2 ]
  EXPECT_FALSE(is_equitable(integer_weighted_graph, pi) );

  pi.breakout(0);
  // pi == [ 1 | 0 | 2 ]
  EXPECT_TRUE(is_equitable(integer_weighted_graph, pi) );
}

TEST_F(RefinerTest, RefineSmallGreater) {
  RefineTraceValue<BasicGraph> a;
  Refiner<BasicGraph> refiner;

  GraphIO::path(&basic_graph, &pi, 3);

  a.set_clear(false);

  EXPECT_EQ(1, refiner.refine(basic_graph, &pi, &a) );

  a.push_active_index(-1);

  EXPECT_EQ(1, refiner.refine(basic_graph, &pi, &a) );
}

TEST_F(RefinerTest, RefineP3) {
  RefineTraceValue<BasicGraph> a;
  Refiner<BasicGraph> refiner;
  PartitionNest pi2;

  GraphIO::path(&basic_graph, &pi, 3);

  refiner.refine(basic_graph, &pi, &a);
  pi2.input_string("[ 0 2 | 1 ]");

  EXPECT_TRUE(pi.is_equal_unordered(pi2) );
}

TEST_F(RefinerTest, RefineP4) {
  RefineTraceValue<BasicGraph> a;
  Refiner<BasicGraph> refiner;
  PartitionNest pi2;

  GraphIO::path(&basic_graph, &pi, 4);

  refiner.refine(basic_graph, &pi, &a);
  pi2.input_string("[ 0 3 | 1 2 ]");

  EXPECT_TRUE(pi.is_equal_unordered(pi2) );
}

TEST_F(RefinerTest, RefineBasicSmall) {
  verify_equitibility<BasicGraph>("test/data/undirected-1-7.txt");
}

TEST_F(RefinerTest, RefineDirectedSmall) {
  verify_equitibility<DirectedGraph>("test/data/directed-1-5.txt");
}

TEST_F(RefinerTest, RefineIntegerWeightedSmall) {
  verify_converted_equitibility<DirectedGraph, IntegerWeightedGraph>
    ("test/data/directed-1-5.txt", GraphIO::convert);
}

}  // namespace nishe
