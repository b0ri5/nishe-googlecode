/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/PartitionNest.h>
#include <nishe/Util.h>

#include <gtest/gtest.h>

#include <stdexcept>
#include <sstream>
#include <fstream>

using std::stringstream;
using std::ifstream;

namespace nishe {

static string escape_brackets(string s)
{
    return "[[]" + s.substr(1, s.length() - 2) + "[]]";
}

class PartitionNestTest : public ::testing::Test
{
 public:

    string input(string s)
    {
        stringstream ss;
        ss.str(s);
        ss >> pi;

        return pi.str();
    }

    void check_invalid_first_character(string s)
    {
        string err = "Error Error Examine: ";
        err += "expected [[] but got " + s + " instead";
        EXPECT_DEATH(input(s), err);
    }

    void check_read_error(string s)
    {
        string err = "Error Error Examine: ";
        err += "couldn't read character from stream";
        EXPECT_DEATH(input(s), err);
    }

    void check_colon_error(string s)
    {
        string err = "Error Error Examine: ";
        err += "expected <m>:<n> [(]increasing, nonnegative[)] ";
        err += "but got .* instead\n";
        EXPECT_DEATH(input(s), err);
    }

    void check_integer_error(string s)
    {
        string err = "Error Error Examine: ";
        err += "expected a nonnegative integer, got .* instead\n";
        EXPECT_DEATH(input(s), err);
    }

    void check_not_partition_error(string s, int size, int missing)
    {
        stringstream ss;

        ss << "Error Error Examine: ";
        ss << "element set should be [{]0, ..., ";
        ss << size - 1;
        ss << "[}] but ";
        ss << missing << " is missing";

        EXPECT_DEATH(input(s), ss.str() );
    }

    void check_breakout_invalid_size(string s, int u)
    {
        stringstream ss;

        ss << "Error Error Examine: ";
        ss << "attempting to breakout " << u << " ";
        ss << "from cell of size 1: ";
        ss << escape_brackets(s) << "\n";

        input(s);

        EXPECT_DEATH(pi.breakout(u), ss.str() );
    }

    void check_breakout_not_found(int u, int k)
    {
        stringstream ss;

        ss << "Error Error Examine: ";
        ss << u << " was not found at index " << k << " of ";
        ss << escape_brackets(pi.str() ) << "\n";

        EXPECT_DEATH(pi.breakout(u), ss.str() );
    }

    /*
     * Verify the integrity of a partition. This will be based on analyzing the string.
     * Assumes the string s is identical to its output (i.e. compressed when possible).
     * This will fail for example if s = [ 0 1 2 ] instead of [ 0:2 ], similarly for
     * [  0:2 ] because of the extra whitespace at the beginning.
     */
    void check_partition_integrity(string s)
    {
        stringstream ss(s);
        PartitionNest pi;

        ss >> pi;

        // ensure input matches output
        ASSERT_STREQ(s.c_str(), pi.str().c_str() );

        check_partition_integrity(pi);
    }

    void check_partition_integrity(const PartitionNest &pi)
    {
        // collect some info about the partition
        // and verify some of it
        vector<vector<int> > cells = string2cells(pi.str() );

        ASSERT_EQ(cells.size(), pi.length() );

        vector<int> indices;
        vector<int> nontrivial_indices;
        int index = 0;

        for (int i = 0; i < cells.size(); i++)
        {
            ASSERT_TRUE(pi.is_index(index) );
            ASSERT_EQ(cells[i].size(), pi.cell_size(index) );

            indices.push_back(index);

            if (cells[i].size() > 1)
            {
                ASSERT_TRUE(pi.is_nontrivial_index(index) );
                nontrivial_indices.push_back(index);
            }

            vector<int> cell = pi[index];
            EXPECT_EQ(cell.size(), cells[i].size() );

            // verify index_containing is correct
            // and the [] operator
            for (int j = 0; j < cells[i].size(); j++)
            {
                EXPECT_EQ(index, pi.index_containing(cells[i][j]) );

                vector<int>::iterator it = find(cell.begin(), cell.end(),
                        cells[i][j]);
                EXPECT_TRUE(it != cell.end() );
            }

            index += cells[i].size();
        }

        // if the partition is discrete
        if (nontrivial_indices.size() == 0)
        {
            EXPECT_TRUE(pi.is_discrete() );

            if (pi.size() > 0)
            {
                EXPECT_EQ(pi.terminal_index(), pi.first_nontrivial_index() );
            }
        }
        else  // otherwise it is not discrete
        {
            EXPECT_EQ(nontrivial_indices.front(), pi.first_nontrivial_index() );

            int k = pi.first_nontrivial_index();

            while (k != pi.terminal_index() )
            {
                EXPECT_TRUE(pi.is_nontrivial_index(k) );
                k = pi.next_nontrivial_index(k);
            }
        }
    }

 protected:
    PartitionNest pi;
};

typedef PartitionNestTest PartitionNestDeathTest;

// This test tests whether or not partition integrity holds
// (checks that output is correct,
// and that elements are in the proper cells, etc.)
TEST_F(PartitionNestTest, CheckPartitionIntegrityOneToSix)
{
    ifstream in;
    string filename = "test/data/partitions-1-6.txt";

    in.open(filename.c_str() );

    if (in.fail() )
    {
        fprintf(stderr, "couldn't open data file %s\n", filename.c_str() );
        exit(1);
    }

    string line;

    std::getline(in, line);

    while (!in.fail() )
    {
        check_partition_integrity(line);
        std::getline(in, line);
    }

    in.close();
}

/*
 * Checks the partition integrity for a large random partition constructed
 * via adding splits randomly. The seed is constant though
 */
TEST_F(PartitionNestTest, CheckPartitionIntegrityLargeRandom)
{
    int large_size = 1000;
    vector<int> unused_indices;
    PartitionNest pi;

    pi.unit(large_size);
    srand(42);

    for (int i = 1; i < large_size; i++)
    {
        unused_indices.push_back(i);
    }

    while (unused_indices.size() > 0)
    {
        // choose a random number of indices to select
        int new_indices = 1;

        if (unused_indices.size() > 1)
        {
            new_indices = 1 + rand() % (unused_indices.size() - 1);
        }

        vector<int> indices;

        for (int i = 0; i < new_indices; i++)
        {
            // select a random index
            int k = rand() % unused_indices.size();
            int index = unused_indices[k];
            indices.push_back(index);

            // remove this index
            unused_indices[k] = unused_indices.back();
            unused_indices.pop_back();
        }

        // indices need to be added in sorted order
        std::sort(indices.begin(), indices.end() );

        for (int i = 0; i < indices.size(); i++)
        {
            pi.enqueue_new_index(indices[i]);
        }

        pi.advance_level();
        pi.commit_pending_indices();

        check_partition_integrity(pi);
    }

    // now deconstruct
    while (pi.level() > 0)
    {
        int m = rand() % pi.level();
        pi.recover_level(m);
        check_partition_integrity(pi);
    }
}


TEST_F(PartitionNestTest, BreakoutSmall)
{
    input("[ 0 1 ]");
    pi.breakout(1);
    EXPECT_STREQ("[ 1 | 0 ]", pi.str().c_str() );
    check_partition_integrity(pi);
}

TEST_F(PartitionNestDeathTest, InputInvalidFirstCharacter)
{
    check_invalid_first_character("1");
    check_invalid_first_character(".");
    check_invalid_first_character("]");
}

TEST_F(PartitionNestDeathTest, InputReadError)
{
    check_read_error("\t");
    check_read_error("\n");
    check_read_error("\r");
    check_read_error(" ");
    check_read_error("");
}

TEST_F(PartitionNestDeathTest, InputGeneralError)
{
    EXPECT_DEATH(input("[ 0 |"),
        "Error Error Examine: error reading token\n");
    EXPECT_DEATH(input("[ |"),
        "Error Error Examine: cannot add 0 as an index\n");
    EXPECT_DEATH(input("[ 0 | |"),
        "Error Error Examine: cannot add an index twice [(]| | occurred[)]\n");
    EXPECT_DEATH(input("[ 0 | ]"),
        "Error Error Examine: n == .* cannot be an index\n");
}

TEST_F(PartitionNestDeathTest, InputColonError)
{
    check_colon_error("[ a:b");
    check_colon_error("[ -1:3");
    check_colon_error("[ 3a:4");
    check_colon_error("[ 2:0");
}

TEST_F(PartitionNestDeathTest, InputBadInteger)
{
    check_integer_error("[ a 1");
    check_integer_error("[ -1 0");
}

TEST_F(PartitionNestDeathTest, InputNotPartition)
{
    check_not_partition_error("[ 0 0 ]", 2, 1);
    check_not_partition_error("[ 1 1 ]", 2, 0);
}

TEST_F(PartitionNestDeathTest, BreakoutInvalidSize)
{
    check_breakout_invalid_size("[ 0 ]", 0);
    check_breakout_invalid_size("[ 1 | 0 ]", 1);
}

TEST_F(PartitionNestDeathTest, BreakoutNotFound)
{
    input("[ 0 1 ]");
    pi.elements()[0] = 1;

    check_breakout_not_found(0, 0);
}

}  // namespace nishe
