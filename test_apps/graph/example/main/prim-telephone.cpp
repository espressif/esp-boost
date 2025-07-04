//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

/*
   IMPORTANT!!!
   ~~~~~~~~~~~~
   This example uses interfaces that have been deprecated and removed from
   Boost.Grpah. Someone needs to update it, as it does NOT compile.
*/

#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
static int test_main()
{
//     using namespace boost;
//     GraphvizGraph g_dot;
//     read_graphviz("figs/telephone-network.dot", g_dot);

//     typedef adjacency_list< vecS, vecS, undirectedS, no_property,
//         property< edge_weight_t, int > >
//         Graph;
//     Graph g(num_vertices(g_dot));
//     auto edge_attr_map = get(edge_attribute, g_dot);
//     graph_traits< GraphvizGraph >::edge_iterator ei, ei_end;
//     for (boost::tie(ei, ei_end) = edges(g_dot); ei != ei_end; ++ei)
//     {
//         auto weight = lexical_cast< int >(edge_attr_map[*ei]["label"]);
//         property< edge_weight_t, int > edge_property(weight);
//         add_edge(source(*ei, g_dot), target(*ei, g_dot), edge_property, g);
//     }

//     typedef graph_traits< Graph >::vertex_descriptor Vertex;
//     std::vector< Vertex > parent(num_vertices(g));
//     auto weight = get(edge_weight, g);
// #if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
//     auto indexmap = get(vertex_index, g);
//     std::vector< std::size_t > distance(num_vertices(g));
//     prim_minimum_spanning_tree(g, *vertices(g).first, &parent[0], &distance[0],
//         weight, indexmap, default_dijkstra_visitor());
// #else
//     prim_minimum_spanning_tree(g, &parent[0]);
// #endif

//     int total_weight = 0;
//     for (std::size_t v = 0; v < num_vertices(g); ++v)
//         if (parent[v] != v)
//             total_weight += get(weight, edge(parent[v], v, g).first);
//     std::cout << "total weight: " << total_weight << std::endl;

//     for (std::size_t u = 0; u < num_vertices(g); ++u)
//         if (parent[u] != u)
//             edge_attr_map[edge(parent[u], u, g_dot).first]["color"] = "black";
//     std::ofstream out("figs/telephone-mst-prim.dot");
//     graph_property< GraphvizGraph, graph_edge_attribute_t >::type&
//         graph_edge_attr_map
//         = get_property(g_dot, graph_edge_attribute);
//     graph_edge_attr_map["color"] = "gray";
//     write_graphviz(out, g_dot);

    return EXIT_SUCCESS;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(prim-telephone)
{
    TEST_ASSERT(test_main() == 0);
    /* esp32: test fails to compile due to deprecated interface, GraphvizGraph */
    std::cout << std::endl;
    std::cout << "!!! THIS TEST IS SKIPPED, AS IT USES DEPRECATED INTERFACE. !!!" << std::endl;
    std::cout << std::endl;
}
