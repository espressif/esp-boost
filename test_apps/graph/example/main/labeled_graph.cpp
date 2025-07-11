// (C) Copyright 2009 Andrew Sutton
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0 (See accompanying file
// LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

/*
   IMPORTANT!!!
   ~~~~~~~~~~~~

   This example does not compile, see
   https://github.com/boostorg/graph/issues/147

*/

#include <iostream>
#include <string>

#include <boost/graph/directed_graph.hpp>
#include <boost/graph/labeled_graph.hpp>

using namespace boost;
using namespace std;

static int test_main()
{

    using namespace boost::graph_detail;

    typedef directed_graph<> Digraph;

    {
        typedef labeled_graph< Digraph, unsigned > Graph;
        Graph g;
        add_vertex(1, g);
        add_vertex(2, g);

        Graph h(12);
    }

    {
        typedef labeled_graph< Digraph, string > Graph;
        Graph g;
        add_vertex("foo", g);
        add_vertex("bar", g);
    }

    {
        typedef labeled_graph< Digraph, string, mapS > Graph;
        Graph g;
        add_vertex("foo", g);
        add_vertex("bar", g);
        add_vertex("foo", g);
    }

    {
        typedef labeled_graph< Digraph*, int > TempGraph;
        Digraph g;
        TempGraph h(&g);
        add_vertex(12, h);
    }

    // {
    //     // This is actually a fairly complicated specialization.
    //     typedef adjacency_list< vecS, vecS, bidirectionalS > G;
    //     typedef labeled_graph< G, size_t > Graph;
    //     Graph g;
    //     add_vertex(0, g);
    //     add_vertex(1, g);
    //     g.add_edge(0, 1); // this line fails to compile
    // }

    return 0;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(labeled_graph)
{
    TEST_ASSERT(test_main() == 0);
}
