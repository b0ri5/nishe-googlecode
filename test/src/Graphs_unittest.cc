/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/Graphs.h>
#include <nishe/GraphIO.h>
#include <nishe/Util.h>
#include <gtest/gtest.h>

#include <utility>
#include <fstream>

using std::ifstream;
using std::make_pair;
using std::pair;

namespace nishe {

class GraphsTest : public ::testing::Test
{
 public:
    BasicGraph basic_graph;
    DirectedGraph directed_graph;
    IntegerWeightedGraph integer_weighted_graph;
    PartitionNest pi;

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
    bool load_graph(graph_t *pG, string s)
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

    template <typename graph_t>
    void check_invalid_vertex(graph_t *pG, string s, string line)
    {
        string err = "Error Error Examine: ";
        err += "expected a vertex [(]nonnegative integer[)]: ";
        err += line;

        EXPECT_DEATH(load_graph(pG, s), err);
    }

    template <typename graph_t>
    void check_missing_colon(graph_t *pG, string s)
    {
        string err = "Error Error Examine: ";
        err += "expected ':' after vertex";

        EXPECT_DEATH(load_graph(pG, s), err);
    }

    template <typename graph_t>
    void check_semicolon_fail(graph_t *pG, string s)
    {
        string err = "Error Error Examine: ";
        err += "nbhd lines must be terminated with \" ;\"";

        EXPECT_DEATH(load_graph(pG, s), err);
    }

    template <typename graph_t>
    void check_invalid_token(graph_t *pG, string s, string token)
    {
        string err = "Error Error Examine: ";
        err += "expected vertex [(]nonnegative integer[)] got ";
        err += token + " instead";

        EXPECT_DEATH(load_graph(pG, s), err);
    }

    void check_invalid_token(IntegerWeightedGraph *pG, string s, string token)
    {
        string err = "Error Error Examine: ";
        err += "expected <v>,<w> token for integer weighted edge, got ";
        err += token + " instead";

        EXPECT_DEATH(load_graph(pG, s), err);
    }

    template <typename graph_t>
    void check_wrong_partition_size(graph_t *pG, string s,
            int vertex_count, int partition_size)
    {
        stringstream ss;

        ss << "Error Error Examine: ";
        ss << "partition size is ";
        ss << partition_size << " but the graph has ";
        ss << vertex_count << " vertices";

        EXPECT_DEATH(load_graph(pG, s), ss.str() );
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

TEST_F(GraphsTest, InputListAsciiEmptyInput)
{
    EXPECT_FALSE(load_graph(&basic_graph, "") );
    EXPECT_FALSE(load_graph(&basic_graph, " ") );
}

TEST_F(GraphsTest, InputListAsciiBasicGraphNullOne)
{
    EXPECT_TRUE(load_graph(&basic_graph, "0: ;") );
    EXPECT_EQ(1, basic_graph.vertex_count() );
}

// input a small graph and make sure it checks out
TEST_F(GraphsTest, InputListAsciiBasicGraphSmall)
{
    EXPECT_TRUE(load_graph(&basic_graph, "0: 1 ;") );
    EXPECT_EQ(2, pi.size() );
    EXPECT_EQ(2, basic_graph.vertex_count() );
    EXPECT_EQ(1, basic_graph.get_nbhd_size(0) );
    EXPECT_EQ(1, basic_graph.get_nbhd_size(1) );
    EXPECT_EQ(1, basic_graph.get_nbhd(0)[0]);
    EXPECT_EQ(0, basic_graph.get_nbhd(1)[0]);
}

TEST_F(GraphsTest, InputListAsciiBasicGraphSmallPartition)
{
    EXPECT_TRUE(load_graph(&basic_graph, "0: 1 ;\n[ 0 1 ]") );
    EXPECT_STREQ("[ 0 1 ]", pi.str().c_str() );
}

TEST_F(GraphsTest, InputListAsciiBasicGraphOneToFive)
{
    ifstream in;
    string filename = "data/undirected-1-5.txt";

    in.open(filename.c_str() );

    if (in.fail() )
    {
        fprintf(stderr, "couldn't open data file %s\n", filename.c_str() );
        exit(1);
    }

    bool successful_read = GraphIO::input_list_ascii(in, &basic_graph, &pi);

    while (successful_read)
    {
        successful_read = GraphIO::input_list_ascii(in, &basic_graph, &pi);
    }

    in.close();
}

// input a small graph and make sure it checks out
TEST_F(GraphsTest, InputListAsciiDirectedGraphSmall)
{
    EXPECT_TRUE(load_graph(&directed_graph, "0 : 1 ;") );
    EXPECT_EQ(2, pi.size() );
    EXPECT_EQ(2, directed_graph.vertex_count() );
    EXPECT_EQ(1, directed_graph.get_nbhd_size(0) );
    EXPECT_EQ(1, directed_graph.get_nbhd_size(1) );

    EXPECT_EQ(DirectedGraph::nbhr(1, DirectedGraph::OUT),
            directed_graph.get_nbhd(0)[0]);
    EXPECT_EQ(DirectedGraph::nbhr(0, DirectedGraph::IN),
            directed_graph.get_nbhd(1)[0]);
}

// input a small graph and make sure it checks out
TEST_F(GraphsTest, InputListAsciiIntegerWeightedGraphSmall)
{
    EXPECT_TRUE(load_graph(&integer_weighted_graph, "0 : 1,0 ;") );
    EXPECT_EQ(2, pi.size() );
    EXPECT_EQ(2, integer_weighted_graph.vertex_count() );
    EXPECT_EQ(1, integer_weighted_graph.get_nbhd_size(0) );
    EXPECT_EQ(1, integer_weighted_graph.get_nbhd_size(1) );

    EXPECT_EQ(IntegerWeightedGraph::nbhr(1, 0),
            integer_weighted_graph.get_nbhd(0)[0]);
    EXPECT_EQ(IntegerWeightedGraph::nbhr(0, 0),
            integer_weighted_graph.get_nbhd(1)[0]);
}

// input a small multi-line graph and make sure it checks out
TEST_F(GraphsTest, InputListAsciiIntegerWeightedGraphMultiLine)
{
    string graph = "0 : 1,0 ;\n1 : 2,1 ;";
    EXPECT_TRUE(load_graph(&integer_weighted_graph, graph) );
    EXPECT_EQ(3, pi.size() );
    EXPECT_EQ(3, integer_weighted_graph.vertex_count() );
    EXPECT_EQ(1, integer_weighted_graph.get_nbhd_size(0) );
    EXPECT_EQ(2, integer_weighted_graph.get_nbhd_size(1) );
    EXPECT_EQ(1, integer_weighted_graph.get_nbhd_size(2) );

    EXPECT_EQ(IntegerWeightedGraph::nbhr(1, 0),
            integer_weighted_graph.get_nbhd(0)[0]);
    EXPECT_EQ(IntegerWeightedGraph::nbhr(0, 0),
            integer_weighted_graph.get_nbhd(1)[0]);
    EXPECT_EQ(IntegerWeightedGraph::nbhr(2, 1),
            integer_weighted_graph.get_nbhd(1)[1]);
}

TEST_F(GraphsTest, OutputListAsciiBasicGraphSmall)
{
    load_graph(&basic_graph, "0 : 1 ;");

    EXPECT_STREQ("0 : 1 ;\n1 : 0 ;",
            output_graph(basic_graph).c_str() );
}

TEST_F(GraphsTest, OutputListAsciiDirectedGraphSmall)
{
    load_graph(&directed_graph, "0 : 1 ;");

    EXPECT_STREQ("0 : 1 ;\n1 : ;",
            output_graph(directed_graph).c_str() );
}

TEST_F(GraphsTest, OutputListAsciiIntegerWeightedGraphSmall)
{
    load_graph(&integer_weighted_graph, "0 : 1,0 ;");

    EXPECT_STREQ("0 : 1,0 ;\n1 : 0,0 ;",
            output_graph(integer_weighted_graph).c_str() );
}

typedef GraphsTest GraphsDeathTest;

/*
 * The first few input deathtests need only be tested with BasicGraph
 */

TEST_F(GraphsDeathTest, InputListAsciiInvalidVertex)
{
    check_invalid_vertex(&basic_graph, "a", "a");
    check_invalid_vertex(&basic_graph, "-1", "-1");
}

TEST_F(GraphsDeathTest, InputListAsciiMisingColon)
{
    check_missing_colon(&basic_graph, "0");
    check_missing_colon(&basic_graph, "0 1");
}

TEST_F(GraphsDeathTest, InputListAsciiEndWithSemicolon)
{
    check_semicolon_fail(&basic_graph, "0 :");
    check_semicolon_fail(&basic_graph, "0 : 1");
}

TEST_F(GraphsDeathTest, InputListAsciiBasicGraphInvalidToken)
{
    check_invalid_token(&basic_graph, "0 : a", "a");
    check_invalid_token(&basic_graph, "0 : -1", "-1");
    check_invalid_token(&basic_graph, "0 : 1,", "1,");
}

// test the extra characters for directed graphs to have full coverage
TEST_F(GraphsDeathTest, InputListAsciiDirectedGraphInvalidToken)
{
    check_invalid_token(&basic_graph, "0 : 1,", "1,");
}

TEST_F(GraphsDeathTest, InputListAsciiIntegerWeightedGraphInvalidToken)
{
    check_invalid_token(&integer_weighted_graph, "0 : 1", "1");
    check_invalid_token(&integer_weighted_graph, "0 : 1.", "1.");
    check_invalid_token(&integer_weighted_graph, "0 : 1,", "1,");
    check_invalid_token(&integer_weighted_graph, "0 : -1,0", "-1,0");
    check_invalid_token(&integer_weighted_graph, "0 : 1,0,", "1,0,");
}

TEST_F(GraphsDeathTest, InputListAsciiColoredGraphWrongPartitionSize)
{
    check_wrong_partition_size(&basic_graph, "0 : 1 ;\n[ ]", 2, 0);
    check_wrong_partition_size(&basic_graph, "0 : 1 ;\n[ 0 ]", 2, 1);
    check_wrong_partition_size(&basic_graph, "0 : 1 ;\n[ 0:2 ]", 2, 3);
}

}  // namespace nishe


