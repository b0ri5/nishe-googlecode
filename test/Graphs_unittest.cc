/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/test/BaseNisheTest.h>

#include <nishe/Graphs.h>
#include <nishe/Graph-inl.h>
#include <nishe/GraphIO.h>
#include <nishe/Util.h>

#include <gtest/gtest.h>

#include <utility>
#include <fstream>
#include <algorithm>

using std::ifstream;
using std::make_pair;
using std::pair;
using std::next_permutation;

namespace nishe {

class GraphsTest : public BaseNisheTest
{
 public:


    template <typename graph_t>
    void check_add_vertex(graph_t *pG)
    {
        graph_t &G = *pG;

        EXPECT_EQ(0, G.vertex_count() );

        // adding vertex 'n' inserts all vertices 0-n
        G.add_vertex(1);
        EXPECT_EQ(2, G.vertex_count() );
        // this vertex was already in the graph
        G.add_vertex(0);
        EXPECT_EQ(2, G.vertex_count() );
        // new vertex
        G.add_vertex(2);
        EXPECT_EQ(3, G.vertex_count() );
    }

    template <typename graph_t>
    void check_is_rigid(const graph_t &G)
    {
        vector<int> x(G.vertex_count() );

        for (int i = 0; i < G.vertex_count(); i++)
        {
            x[i] = i;
        }

        while (next_permutation(x.begin(), x.end() ) )
        {
            EXPECT_FALSE(is_automorphism(G, &x[0]) );
        }
    }
};

TEST_F(GraphsTest, AddVertices)
{
    check_add_vertex(&basic_graph);
    check_add_vertex(&directed_graph);
    check_add_vertex(&integer_weighted_graph);
}

/*
 * Add the edge {0, 1} and make sure it shows up
 */
TEST_F(GraphsTest, BasicGraphAddEdge)
{
    EXPECT_TRUE(basic_graph.add_edge(0, 1) );

    EXPECT_EQ(2, basic_graph.vertex_count() );

    EXPECT_FALSE(basic_graph.add_edge(1, 0) );
    EXPECT_FALSE(basic_graph.add_edge(0, 1) );
    EXPECT_EQ(1, basic_graph.get_nbhd_size(0) );
    EXPECT_EQ(1, basic_graph.get_nbhd_size(1) );

    EXPECT_EQ(1, basic_graph.get_nbhd(0)[0]);
    EXPECT_EQ(0, basic_graph.get_nbhd(1)[0]);
}


/*
 * Add the arc (0, 1) and make sure it shows up
 */
TEST_F(GraphsTest, DirectedGraphAddArc)
{
    EXPECT_TRUE(directed_graph.add_arc(0, 1) );
    EXPECT_EQ(2, directed_graph.vertex_count() );

    // try adding the arc again
    EXPECT_FALSE(directed_graph.add_arc(0, 1) );

    // test neighborhood
    EXPECT_EQ(1, directed_graph.get_nbhd_size(0) );
    EXPECT_EQ(1, directed_graph.get_nbhd_size(1) );

    EXPECT_EQ(DirectedGraph::nbhr(1, DirectedGraph::OUT),
            directed_graph.get_nbhd(0)[0]);
    EXPECT_EQ(DirectedGraph::nbhr(0, DirectedGraph::IN),
            directed_graph.get_nbhd(1)[0]);

    // make symmetric arc
    EXPECT_TRUE(directed_graph.add_arc(1, 0) );

    // test neighborhood
    EXPECT_EQ(DirectedGraph::nbhr(1, DirectedGraph::BOTH),
            directed_graph.get_nbhd(0)[0]);
    EXPECT_EQ(DirectedGraph::nbhr(0, DirectedGraph::BOTH),
            directed_graph.get_nbhd(1)[0]);
}

/*
 * Add the weighted edge (0, 1, 2) and make sure it shows up
 */
TEST_F(GraphsTest, IntegerWeightedGraphAddArc)
{
    EXPECT_TRUE(integer_weighted_graph.add_weighted_edge(0, 1, 2) );
    EXPECT_EQ(2, integer_weighted_graph.vertex_count() );

    // try adding the arc again
    EXPECT_FALSE(integer_weighted_graph.add_weighted_edge(0, 1, 2) );

    // test neighborhood
    EXPECT_EQ(1, integer_weighted_graph.get_nbhd_size(0) );
    EXPECT_EQ(1, integer_weighted_graph.get_nbhd_size(1) );

    EXPECT_EQ(IntegerWeightedGraph::nbhr(1, 2),
            integer_weighted_graph.get_nbhd(0)[0]);
    EXPECT_EQ(IntegerWeightedGraph::nbhr(0, 2),
            integer_weighted_graph.get_nbhd(1)[0]);
}

/*
 * Test the is_automorphism function
 */

TEST_F(GraphsTest, IsAutomorphismPath3)
{
    GraphIO::path(&basic_graph, 3);
    vector<int> x(basic_graph.vertex_count() );

    x[0] = 1;
    x[1] = 0;

    // x = (0 1)
    EXPECT_FALSE(is_automorphism(basic_graph, &x[0]) );

    x[0] = 1;
    x[1] = 2;
    x[2] = 0;

    // x = (0 1 2)
    EXPECT_FALSE(is_automorphism(basic_graph, &x[0]) );

    x[0] = 2;
    x[1] = 1;
    x[2] = 0;

    // x = (0 2)
    EXPECT_TRUE(is_automorphism(basic_graph, &x[0]) );
}

TEST_F(GraphsTest, IsAutomorphismDirectedPath3)
{
    GraphIO::directed_path(&directed_graph, 3);

    check_is_rigid(directed_graph);
}

}  // namespace nishe


