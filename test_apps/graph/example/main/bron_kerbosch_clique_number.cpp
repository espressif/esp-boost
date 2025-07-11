// (C) Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0 (See accompanying file
// LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

//[code_bron_kerbosch_clique_number
#include <iostream>
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>

#include "helper.hpp"

using namespace std;
using namespace boost;

// Declare the graph type and its vertex and edge types.
typedef undirected_graph<> Graph;
typedef graph_traits< Graph >::vertex_descriptor Vertex;
typedef graph_traits< Graph >::edge_descriptor Edge;

static int test_main(int argc, char* argv[])
{
    // Create the graph and read it from standard input.
    Graph g;
    read_graph(g, cin);

    // Use the Bron-Kerbosch algorithm to find all cliques, and
    auto c = bron_kerbosch_clique_number(g);
    cout << "clique number: " << c << endl;

    return 0;
}
//]

#include "common.hpp"

BOOST_AUTO_TEST_CASE(bron_kerbosch_clique_number)
{
    TEST_ASSERT(test_main(0, nullptr) == 0);
    /* esp32: test failed due to no input data */
}
