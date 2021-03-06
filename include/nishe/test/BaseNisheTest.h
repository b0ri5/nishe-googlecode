#ifndef _BASE_NISHE_TEST_H_
#define _BASE_NISHE_TEST_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/PartitionNest.h>
#include <nishe/Graphs.h>
#include <nishe/GraphIO.h>

#include <gtest/gtest.h>

#include <string>
#include <sstream>

using std::stringstream;

namespace nishe {

class BaseNisheTest : public ::testing::Test
{
 protected:
     BasicGraph basic_graph;
     DirectedGraph directed_graph;
     IntegerWeightedGraph integer_weighted_graph;
     PartitionNest pi;

     template <typename graph_t>
     bool input_graph(graph_t *pG, string s)
     {
         stringstream ss(s);

         return GraphIO::input_list_ascii(ss, pG, &pi);
     }

     template <typename graph_t>
     string output_graph(const graph_t &G)
     {
         stringstream ss;

         GraphIO::output_list_ascii(ss, G);

         return ss.str();
     }
};

}  // namespace nishe

#endif  // _BASE_NISHE_TEST_H_
