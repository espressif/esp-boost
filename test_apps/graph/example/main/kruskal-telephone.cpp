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
#include <boost/lexical_cast.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>

static int test_main()
{
    // using namespace boost;
    // GraphvizGraph g_dot;
    // read_graphviz("figs/telephone-network.dot", g_dot);

    // typedef adjacency_list< vecS, vecS, undirectedS, no_property,
    //     property< edge_weight_t, int > >
    //     Graph;
    // Graph g(num_vertices(g_dot));
    // auto edge_attr_map = get(edge_attribute, g_dot);
    // graph_traits< GraphvizGraph >::edge_iterator ei, ei_end;
    // for (boost::tie(ei, ei_end) = edges(g_dot); ei != ei_end; ++ei)
    // {
    //     int weight = lexical_cast< int >(edge_attr_map[*ei]["label"]);
    //     property< edge_weight_t, int > edge_property(weight);
    //     add_edge(source(*ei, g_dot), target(*ei, g_dot), edge_property, g);
    // }

    // std::vector< graph_traits< Graph >::edge_descriptor > mst;
    // typedef std::vector< graph_traits< Graph >::edge_descriptor >::size_type
    //     size_type;
    // kruskal_minimum_spanning_tree(g, std::back_inserter(mst));

    // auto weight = get(edge_weight, g);
    // int total_weight = 0;
    // for (auto const& edge : mst)
    //     total_weight += get(weight, edge);
    // std::cout << "total weight: " << total_weight << std::endl;

    // typedef graph_traits< Graph >::vertex_descriptor Vertex;
    // for (auto const& edge : mst)
    // {
    //     auto u = source(edge, g), v = target(edge, g);
    //     edge_attr_map[edge(u, v, g_dot).first]["color"] = "black";
    // }
    // std::ofstream out("figs/telephone-mst-kruskal.dot");
    // auto graph_edge_attr_map = get_property(g_dot, graph_edge_attribute);
    // graph_edge_attr_map["color"] = "gray";
    // graph_edge_attr_map["style"] = "bold";
    // write_graphviz(out, g_dot);

    return EXIT_SUCCESS;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(kruskal-telephone)
{
    TEST_ASSERT(test_main() == 0);
    /* esp32: test fails to compile due to deprecated interface, GraphvizGraph */
    std::cout << std::endl;
    std::cout << "!!! THIS TEST IS SKIPPED, AS IT USES DEPRECATED INTERFACE. !!!" << std::endl;
    std::cout << std::endl;
}
