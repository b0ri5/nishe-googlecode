/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/test/BaseNisheTest.h>

#include <nishe/Graphs.h>
#include <nishe/GraphIO-inl.h>
#include <nishe/Util.h>

#include <gtest/gtest.h>

#include <utility>
#include <fstream>

using std::ifstream;
using std::make_pair;
using std::pair;

namespace nishe {

class GraphIOTest : public BaseNisheTest
{
 public:

    template <typename graph_t>
    void check_invalid_vertex(graph_t *pG, string s, string line)
    {
        string err = "Error Error Examine: ";
        err += "expected a vertex [(]nonnegative integer[)]: ";
        err += line;

        EXPECT_DEATH(input_graph(pG, s), err);
    }

    template <typename graph_t>
    void check_missing_colon(graph_t *pG, string s)
    {
        string err = "Error Error Examine: ";
        err += "expected ':' after vertex";

        EXPECT_DEATH(input_graph(pG, s), err);
    }

    template <typename graph_t>
    void check_semicolon_fail(graph_t *pG, string s)
    {
        string err = "Error Error Examine: ";
        err += "nbhd lines must be terminated with \" ;\"";

        EXPECT_DEATH(input_graph(pG, s), err);
    }

    template <typename graph_t>
    void check_invalid_token(graph_t *pG, string s, string token)
    {
        string err = "Error Error Examine: ";
        err += "expected vertex [(]nonnegative integer[)] got ";
        err += token + " instead";

        EXPECT_DEATH(input_graph(pG, s), err);
    }

    void check_invalid_token(IntegerWeightedGraph *pG, string s, string token)
    {
        string err = "Error Error Examine: ";
        err += "expected <v>,<w> token for integer weighted edge, got ";
        err += token + " instead";

        EXPECT_DEATH(input_graph(pG, s), err);
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

        EXPECT_DEATH(input_graph(pG, s), ss.str() );
    }

    template <typename graph_t>
    void verify_input_list_ascii(graph_t *G_ptr, string filename)
    {
        ifstream in;

        in.open(filename.c_str() );

        if (in.fail() )
        {
            fprintf(stderr, "couldn't open data file %s\n", filename.c_str() );
            exit(1);
        }

        bool successful_read = GraphIO::input_list_ascii(in, G_ptr, &pi);

        while (successful_read)
        {
            successful_read = GraphIO::input_list_ascii(in, G_ptr, &pi);
        }

        in.close();
    }
};


TEST_F(GraphIOTest, InputListAsciiEmptyInput)
{
    EXPECT_FALSE(input_graph(&basic_graph, "") );
    EXPECT_FALSE(input_graph(&basic_graph, " ") );
}

TEST_F(GraphIOTest, InputListAsciiBasicGraphNullOne)
{
    EXPECT_TRUE(input_graph(&basic_graph, "0: ;") );
    EXPECT_EQ(1, basic_graph.vertex_count() );
}

// input a small graph and make sure it checks out
TEST_F(GraphIOTest, InputListAsciiBasicGraphSmall)
{
    EXPECT_TRUE(input_graph(&basic_graph, "0: 1 ;") );
    EXPECT_EQ(2, pi.size() );
    EXPECT_EQ(2, basic_graph.vertex_count() );
    EXPECT_EQ(1, basic_graph.get_nbhd_size(0) );
    EXPECT_EQ(1, basic_graph.get_nbhd_size(1) );
    EXPECT_EQ(1, basic_graph.get_nbhd(0)[0]);
    EXPECT_EQ(0, basic_graph.get_nbhd(1)[0]);
}

TEST_F(GraphIOTest, InputListAsciiBasicGraphSmallPartition)
{
    EXPECT_TRUE(input_graph(&basic_graph, "0: 1 ;\n[ 0 1 ]") );
    EXPECT_STREQ("[ 0 1 ]", pi.str().c_str() );
}

TEST_F(GraphIOTest, InputListAsciiBasicGraphOneToFive)
{
    verify_input_list_ascii(&basic_graph, "src/test/data/undirected-1-5.txt");
}

TEST_F(GraphIOTest, InputListAsciiDirectedGraphOneToFive)
{
    verify_input_list_ascii(&directed_graph, "src/test/data/directed-1-5.txt");
}

// input a small graph and make sure it checks out
TEST_F(GraphIOTest, InputListAsciiDirectedGraphSmall)
{
    EXPECT_TRUE(input_graph(&directed_graph, "0 : 1 ;") );
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
TEST_F(GraphIOTest, InputListAsciiIntegerWeightedGraphSmall)
{
    EXPECT_TRUE(input_graph(&integer_weighted_graph, "0 : 1,0 ;") );
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
TEST_F(GraphIOTest, InputListAsciiIntegerWeightedGraphMultiLine)
{
    string graph = "0 : 1,0 ;\n1 : 2,1 ;";
    EXPECT_TRUE(input_graph(&integer_weighted_graph, graph) );
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

TEST_F(GraphIOTest, OutputListAsciiBasicGraphSmall)
{
    input_graph(&basic_graph, "0 : 1 ;");

    EXPECT_STREQ("0 : 1 ;\n1 : 0 ;",
            output_graph(basic_graph).c_str() );
}

TEST_F(GraphIOTest, OutputListAsciiDirectedGraphSmall)
{
    input_graph(&directed_graph, "0 : 1 ;");

    EXPECT_STREQ("0 : 1 ;\n1 : ;",
            output_graph(directed_graph).c_str() );
}

TEST_F(GraphIOTest, OutputListAsciiIntegerWeightedGraphSmall)
{
    input_graph(&integer_weighted_graph, "0 : 1,0 ;");

    EXPECT_STREQ("0 : 1,0 ;\n1 : 0,0 ;",
            output_graph(integer_weighted_graph).c_str() );
}

typedef GraphIOTest GraphIODeathTest;

/*
 * The first few input deathtests need only be tested with BasicGraph
 */

TEST_F(GraphIODeathTest, InputListAsciiInvalidVertex)
{
    check_invalid_vertex(&basic_graph, "a", "a");
    check_invalid_vertex(&basic_graph, "-1", "-1");
}

TEST_F(GraphIODeathTest, InputListAsciiMisingColon)
{
    check_missing_colon(&basic_graph, "0");
    check_missing_colon(&basic_graph, "0 1");
}

TEST_F(GraphIODeathTest, InputListAsciiEndWithSemicolon)
{
    check_semicolon_fail(&basic_graph, "0 :");
    check_semicolon_fail(&basic_graph, "0 : 1");
}

TEST_F(GraphIODeathTest, InputListAsciiBasicGraphInvalidToken)
{
    check_invalid_token(&basic_graph, "0 : a", "a");
    check_invalid_token(&basic_graph, "0 : -1", "-1");
    check_invalid_token(&basic_graph, "0 : 1,", "1,");
}

// test the extra characters for directed graphs to have full coverage
TEST_F(GraphIODeathTest, InputListAsciiDirectedGraphInvalidToken)
{
    check_invalid_token(&basic_graph, "0 : 1,", "1,");
}

TEST_F(GraphIODeathTest, InputListAsciiIntegerWeightedGraphInvalidToken)
{
    check_invalid_token(&integer_weighted_graph, "0 : 1", "1");
    check_invalid_token(&integer_weighted_graph, "0 : 1.", "1.");
    check_invalid_token(&integer_weighted_graph, "0 : 1,", "1,");
    check_invalid_token(&integer_weighted_graph, "0 : -1,0", "-1,0");
    check_invalid_token(&integer_weighted_graph, "0 : 1,0,", "1,0,");
}

TEST_F(GraphIODeathTest, InputListAsciiColoredGraphWrongPartitionSize)
{
    check_wrong_partition_size(&basic_graph, "0 : 1 ;\n[ ]", 2, 0);
    check_wrong_partition_size(&basic_graph, "0 : 1 ;\n[ 0 ]", 2, 1);
    check_wrong_partition_size(&basic_graph, "0 : 1 ;\n[ 0:2 ]", 2, 3);
}

}  // namespace nishe


