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

static bool add_edge(BasicGraph *pG, vertex_t u, string token)
{
    std::stringstream ss(token);
    vertex_t v = 0;

    ss >> v;

    if (ss.fail() || token.size() != ss.tellg() )
    {
        fail("expected vertex (nonnegative integer) got " +
                token + " instead");
    }

    return pG->add_edge(u, v);
}

static bool add_arc(DirectedGraph *pG, vertex_t u, string token)
{
    std::stringstream ss(token);
    vertex_t v = 0;

    ss >> v;

    if (ss.fail() || token.size() != ss.tellg() )
    {
        fail("expected vertex (nonnegative integer) got " +
                token + " instead");
    }

    return pG->add_arc(u, v);
}

// token should be of the form %d,%d
static bool add_weighted_edge(IntegerWeightedGraph *pG,
        vertex_t u, string token)
{
    std::stringstream ss(token);
    vertex_t v = 0;
    int weight = 0;
    char comma = 0;

    string err = "expected <v>,<w> token for integer weighted edge, ";
    err += "got " + token + " instead";

    ss >> v;

    if (ss.fail() )
    {
        fail(err);
    }

    ss >> comma;

    if (comma != ',')
    {
        fail(err);
    }

    ss >> weight;

    if (ss.fail() || token.size() != ss.tellg() )
    {
        fail(err);
    }

    return pG->add_weighted_edge(u, v, weight);
}

void output_nbhr_basic_graph(ostream &out, const BasicGraph::nbhr &u)
{
    out << u;
}

void output_nbhr_directed_graph(ostream &out, const DirectedGraph::nbhr &nbhr)
{
    if (nbhr.second == DirectedGraph::OUT)
    {
        out << nbhr.first;
    }
}

void output_nbhr_integer_weighted_graph(ostream &out,
        const DirectedGraph::nbhr &nbhr)
{
    out << nbhr.first << "," << nbhr.second;
}

bool GraphIO::input_list_ascii(istream &in,
        BasicGraph *pG, PartitionNest *pPi)
{
    GraphIO::input_list_ascii(in, pG, pPi, add_edge);
}

bool GraphIO::input_list_ascii(istream &in,
        DirectedGraph *pG, PartitionNest *pPi)
{
    GraphIO::input_list_ascii(in, pG, pPi, add_arc);
}

bool GraphIO::input_list_ascii(istream &in,
        IntegerWeightedGraph *pG, PartitionNest *pPi)
{
    GraphIO::input_list_ascii(in, pG, pPi, add_weighted_edge);
}

void GraphIO::output_list_ascii(ostream &out, const BasicGraph &G)
{
    GraphIO::output_list_ascii(out, G, output_nbhr_basic_graph);
}

void GraphIO::output_list_ascii(ostream &out, const DirectedGraph &G)
{
    GraphIO::output_list_ascii(out, G, output_nbhr_directed_graph);
}

void GraphIO::output_list_ascii(ostream &out, const IntegerWeightedGraph &G)
{
    GraphIO::output_list_ascii(out, G, output_nbhr_integer_weighted_graph);
}

}  // namespace nishe
