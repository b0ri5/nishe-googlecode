/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Graphs.h>
#include <nishe/Refiner-inl.h>
#include <nishe/RefineTraceValue.h>
#include <nishe/GraphIO-inl.h>
#include "BaseNisheTest.h"

#include <gtest/gtest.h>

#include <iostream>
#include <utility>
#include <map>
#include <sstream>

using std::make_pair;
using std::map;
using std::stringstream;

namespace nishe {

class RefinerTest : public BaseNisheTest
{

};

TEST_F(RefinerTest, RefineTraceValueCompare)
{
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

TEST_F(RefinerTest, IsEquitableDirectedGraph)
{
    GraphIO::directed_path(&directed_graph, &pi, 2);
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

    pi.advance_level();
    pi.breakout(1);
    // pi == [ 1 | 0 2 ]
    EXPECT_FALSE(is_equitable(integer_weighted_graph, pi) );

    pi.breakout(0);
    // pi == [ 1 | 0 | 2 ]
    EXPECT_TRUE(is_equitable(integer_weighted_graph, pi) );
}

TEST_F(RefinerTest, RefineSmallGreater)
{
    RefineTraceValue<BasicGraph> a;
    Refiner<BasicGraph> refiner;

    GraphIO::path(&basic_graph, &pi, 3);

    a.set_clear(false);

    EXPECT_EQ(1, refiner.refine(basic_graph, &pi, &a) );

    a.push_active_index(-1);

    EXPECT_EQ(1, refiner.refine(basic_graph, &pi, &a) );
}

}  // namespace nishe
