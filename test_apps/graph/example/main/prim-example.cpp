//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/config.hpp>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>

static int test_main()
{
    using namespace boost;
    typedef adjacency_list< vecS, vecS, undirectedS,
        property< vertex_distance_t, int >, property< edge_weight_t, int > >
        Graph;
    typedef std::pair< int, int > E;
    const int num_nodes = 5;
    const auto edges
        = { E(0, 2), E(1, 3), E(1, 4), E(2, 1), E(2, 3), E(3, 4), E(4, 0) };
    int weights[] = { 1, 1, 2, 7, 3, 1, 1 };
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
    Graph g(num_nodes);
    auto weightmap = get(edge_weight, g);
    for (std::size_t j = 0; j < sizeof(edges) / sizeof(E); ++j)
    {
        graph_traits< Graph >::edge_descriptor e;
        bool inserted;
        boost::tie(e, inserted) = add_edge(edges[j].first, edges[j].second, g);
        weightmap[e] = weights[j];
    }
#else
    Graph g(std::begin(edges), std::end(edges), weights, num_nodes);
#endif
    std::vector< graph_traits< Graph >::vertex_descriptor > p(num_vertices(g));

#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
    auto distance = get(vertex_distance, g);
    auto indexmap = get(vertex_index, g);
    prim_minimum_spanning_tree(g, *vertices(g).first, &p[0], distance,
        weightmap, indexmap, default_dijkstra_visitor());
#else
    prim_minimum_spanning_tree(g, &p[0]);
#endif

    for (std::size_t i = 0; i != p.size(); ++i)
        if (p[i] != i)
            std::cout << "parent[" << i << "] = " << p[i] << std::endl;
        else
            std::cout << "parent[" << i << "] = no parent" << std::endl;

    return EXIT_SUCCESS;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(prim-example)
{
    TEST_ASSERT(test_main() == 0);
}
