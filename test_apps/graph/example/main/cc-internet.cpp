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
#include <fstream>
#include <vector>
#include <string>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/graphviz.hpp>

static int test_main()
{
    // using namespace boost;
    // GraphvizGraph g;
    // read_graphviz("figs/cc-internet.dot", g);

    // std::vector< int > component(num_vertices(g));

    // connected_components(g,
    //     make_iterator_property_map(
    //         component.begin(), get(vertex_index, g), component[0]));

    // auto vertex_attr_map = get(vertex_attribute, g);
    // std::string color[] = { "white", "gray", "black", "lightgray" };
    // graph_traits< GraphvizGraph >::vertex_iterator vi, vi_end;
    // for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    // {
    //     vertex_attr_map[*vi]["color"] = color[component[*vi]];
    //     vertex_attr_map[*vi]["style"] = "filled";
    //     if (vertex_attr_map[*vi]["color"] == "black")
    //         vertex_attr_map[*vi]["fontcolor"] = "white";
    // }
    // write_graphviz("figs/cc-internet-out.dot", g);
    return 0;
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(cc_internet)
{
    TEST_ASSERT(test_main() == 0);
    /* esp32: test fails to compile due to deprecated interface, GraphvizGraph */
    std::cout << std::endl;
    std::cout << "!!! THIS TEST IS SKIPPED, AS IT USES DEPRECATED INTERFACE. !!!" << std::endl;
    std::cout << std::endl;
}
