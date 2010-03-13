/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/BasicGraph.h>
#include <nishe/Refiner-inl.h>
#include <nishe/RefineTraceValue.h>
#include <nishe/GraphIO.h>

#include <gtest/gtest.h>

#include <iostream>
#include <utility>
#include <map>
#include <sstream>

using std::make_pair;
using std::map;
using std::stringstream;

namespace nishe {

class RefinerTest : public ::testing::Test
{
 public:
    BasicGraph basic_graph;
    DirectedGraph directed_graph;
    IntegerWeightedGraph integer_weighted_graph;
    PartitionNest pi;
};

TEST_F(RefinerTest, RefineTraceValueCompare)
{
    typedef RefineTraceValue<BasicGraph> BasicRefineTraceValue;
    BasicRefineTraceValue a;
    BasicRefineTraceValue b;

    EXPECT_EQ(0, a.cmp(b) );

    // ensure if 'b' is too short that 'a' is smaller
    a.active_indices.push_back(1);
    EXPECT_EQ(1, a.cmp(b) );
    EXPECT_EQ(-1, b.cmp(a) );

    // now change the indices
    b.active_indices.push_back(0);
    EXPECT_EQ(1, a.cmp(b) );
    EXPECT_EQ(-1, b.cmp(a) );

    // now add some nbhr sums
    a.adjacent_nbhr_sums.resize(1);
    a.adjacent_nbhr_sums[0].push_back(
            BasicRefineTraceValue::index_nbhr_sum(1, 1) );
    b.active_indices[0] = 1;
    b.adjacent_nbhr_sums.resize(1);

    // now we have
    // a - 1: 1,1
    // b - 1:
    EXPECT_EQ(1, a.cmp(b) );
    EXPECT_EQ(-1, b.cmp(a) );

    b.adjacent_nbhr_sums[0].push_back(
            BasicRefineTraceValue::index_nbhr_sum(1, 1) );
    EXPECT_EQ(0, a.cmp(b) );

    b.adjacent_nbhr_sums[0][0].first = 0;

    // a - 1: 1,1
    // b - 1: 0,1
    EXPECT_EQ(1, a.cmp(b) );
    EXPECT_EQ(-1, b.cmp(a) );

    b.adjacent_nbhr_sums[0][0].first = 1;
    b.adjacent_nbhr_sums[0].push_back(
            BasicRefineTraceValue::index_nbhr_sum(2, 1) );

    // a - 1: 1,1
    // b - 1: 1,1 2,1
    EXPECT_EQ(-1, a.cmp(b) );
    EXPECT_EQ(1, b.cmp(a) );

    a.adjacent_nbhr_sums[0].push_back(
            BasicRefineTraceValue::index_nbhr_sum(2, 1) );
    EXPECT_EQ(0, a.cmp(b) );
}

// test out is_equitable on different graphs
TEST_F(RefinerTest, IsEquitableBasicGraph)
{
    GraphIO::input_list_ascii("0 : ;\n[ 0 ]", &basic_graph, &pi);
    EXPECT_TRUE(is_equitable(basic_graph, pi) );

    GraphIO::input_list_ascii("0 : 1 ;\n1 : 2 ;\n[ 0:2 ]",
            &basic_graph, &pi);
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

TEST_F(RefinerTest, IsEquitableDirectedGraph)
{
    GraphIO::input_list_ascii("0 : 1 ;", &directed_graph, &pi);
    EXPECT_FALSE(is_equitable(directed_graph, pi) );

    pi.advance_level();
    pi.breakout(1);
    // pi == [ 1 | 0 ]
    EXPECT_TRUE(is_equitable(directed_graph, pi) );
}

TEST_F(RefinerTest, IsEquitableIntegerWeightedGraph)
{
    GraphIO::input_list_ascii("0 : 1,0 ;\n1 : 2,1 ;",
            &integer_weighted_graph, &pi);
    EXPECT_FALSE(is_equitable(integer_weighted_graph, pi) );

    GraphIO::output_list_ascii(std::cout, integer_weighted_graph);

    pi.advance_level();
    pi.breakout(1);
    // pi == [ 1 | 0 2 ]
    EXPECT_FALSE(is_equitable(integer_weighted_graph, pi) );

    pi.breakout(0);
    // pi == [ 1 | 0 | 2 ]
    EXPECT_TRUE(is_equitable(integer_weighted_graph, pi) );
}

}  // namespace nishe
