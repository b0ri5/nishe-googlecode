#ifndef _GRAPH_IO_INL_H_
#define _GRAPH_IO_INL_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/GraphIO.h>

#include <string>
#include <sstream>

using std::stringstream;

namespace nishe {

static bool is_whitespace(string s)
{
    string whitespace = " \t\n\r";

    for (int i = 0; i < s.size(); i++)
    {
        if (whitespace.find(s[i]) == string::npos)
        {
            return false;
        }
    }

    return true;
}

template <typename graph_t>
string GraphIO::output_list_ascii_string(ostream &out,
        const graph_t &G)
{
    stringstream ss;

    output_list_ascii(ss, G);

    return ss.str();
}

template<typename graph_t>
bool GraphIO::input_list_ascii(string s,
        graph_t *pG, PartitionNest *pPi)
{
    stringstream ss(s);

    return GraphIO::input_list_ascii(ss, pG, pPi);
}

template <typename graph_t>
bool GraphIO::input_list_ascii(istream &in,
        graph_t *pG, PartitionNest *pPi,
        bool (*add_nbhr)(graph_t *, vertex_t, string) )
{
    string line;
    string token;
    string partition;  // stores the line containing the partition

    // clear what might already be here
    pG->clear();

    // process line by line
    getline(in, line);

    // returns false if nothing can be read
    if (in.fail() || is_whitespace(line) )
    {
        return false;
    }

    while (!is_whitespace(line) )
    {
        stringstream ss(line);

        vertex_t u = 0;
        ss >> u;

        if (ss.fail() )  // don't need to check if negative
        {
            ss.clear();
            ss >> token;
            string err = "expected a vertex (nonnegative integer): ";
            err += token;
            fail(err);
        }

        pG->add_vertex(u);

        ss >> token;

        if (token != ":" || ss.fail() )
        {
            fail("expected ':' after vertex");
        }

        ss >> token;

        if (ss.fail() )
        {
            fail("nbhd lines must be terminated with \" ;\"");
        }

        // now read in the neighbors and add them until we get a semicolon
        while (token != ";")
        {
            add_nbhr(pG, u, token);

            ss >> token;

            if (ss.fail() )
            {
                fail("nbhd lines must be terminated with \" ;\"");
            }
        }

        getline(in, line);

        // if in fails and it's not at eof
        if (in.fail() && !in.eof() )
        {
            // not sure how to test this or if it can even happen
            fail("error reading line but not at eof");
        }
        else if (in.fail() && in.eof() )
        {
            line = "";
        }

        // otherwise look for a partition to read in
        // if the first non-whitespace character is a '['
        for (int i = 0; i < line.size(); i++)
        {
            if (line[i] == ' ' || line[i] == '\t')
            {
                continue;
            }

            if (line[i] == '[')
            {
                partition = line;
                line = "";  // will exit the outer while loop
                break;  // exit the for loop
            }
        }

        // stop at eof
        /*if (in.eof() )
        {
            line = "";
        }*/
    }

    if (partition.size() == 0)
    {
        pPi->unit(pG->vertex_count() );
    }
    else  // we must have a partition on our hands
    {
        stringstream ss(partition);

        ss >> *pPi;

        if (pPi->size() != pG->vertex_count() )
        {
            stringstream ss;
            ss << "partition size is ";
            ss << pPi->size() << " but the graph has ";
            ss << pG->vertex_count() << " vertices";
            fail(ss.str() );
        }
    }

    return true;
}


template<typename graph_t, typename nbhr_t>
void GraphIO::output_list_ascii(ostream &out,
            const graph_t &G,
            void (*output_nbhr)(ostream &out, const nbhr_t &nbhr) )
{
    vertex_t u = 0;

    for (vertex_t u = 0; u < G.vertex_count(); u++)
    {
        out << u << " : ";

        for (int i = 0; i < G.get_nbhd_size(u); i++)
        {
            int pos = out.tellp();
            output_nbhr(out, G.get_nbhd(u)[i]);

            if (out.tellp() > pos)
            {
                out << " ";
            }
        }

        out << ";";

        if (u < G.vertex_count() - 1)
        {
            out << "\n";
        }
    }
}

template <typename graph_t>
void GraphIO::null(graph_t *pG, int n)
{
    pG->add_vertex(n - 1);
}

template <typename graph_t>
void GraphIO::null(graph_t *pG, PartitionNest *pPi, int n)
{
    null(pG);
    pPi->unit(n);
}

}  // namespace nishe

#endif  // _GRAPH_IO_INL_H_
