//  Copyright (c) 2006, Stephan Diederich
//
//  This code may be used under either of the following two licences:
//
//    Permission is hereby granted, free of charge, to any person
//    obtaining a copy of this software and associated documentation
//    files (the "Software"), to deal in the Software without
//    restriction, including without limitation the rights to use,
//    copy, modify, merge, publish, distribute, sublicense, and/or
//    sell copies of the Software, and to permit persons to whom the
//    Software is furnished to do so, subject to the following
//    conditions:
//
//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//    OTHER DEALINGS IN THE SOFTWARE. OF SUCH DAMAGE.
//
//  Or:
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/write_dimacs.hpp>
#include "common_components.hpp"
#include "embedded_files_lut.hpp"

/*************************************
 *
 * example which reads in a max-flow problem from std::cin, augments all paths
 *from source->NODE->sink and writes the graph back to std::cout
 *
 **************************************/

template < typename EdgeCapacityMap > struct zero_edge_capacity
{

    zero_edge_capacity() {}
    zero_edge_capacity(EdgeCapacityMap cap_map) : m_cap_map(cap_map) {};

    template < typename Edge > bool operator()(const Edge& e) const
    {
        return get(m_cap_map, e) == 0;
    }

    EdgeCapacityMap m_cap_map;
};

static int test_main()
{
    using namespace boost;
    typedef adjacency_list_traits< vecS, vecS, directedS > Traits;
    typedef adjacency_list< vecS, vecS, directedS, no_property,
        property< edge_capacity_t, long,
            property< edge_reverse_t, Traits::edge_descriptor > > >
        Graph;

    typedef graph_traits< Graph >::out_edge_iterator out_edge_iterator;
    typedef graph_traits< Graph >::edge_descriptor edge_descriptor;
    typedef graph_traits< Graph >::vertex_descriptor vertex_descriptor;

    Graph g;

    typedef property_map< Graph, edge_capacity_t >::type tCapMap;
    typedef tCapMap::value_type tCapMapValue;

    auto capacity = get(edge_capacity, g);
    auto rev = get(edge_reverse, g);

    vertex_descriptor s, t;
    // /*reading the graph from stdin*/
    // read_dimacs_max_flow(g, capacity, rev, s, t, std::cin);
    // mock stdin
    auto file_in_ptr = open_input_stream(embedded_files, "max_flow.dat");
    read_dimacs_max_flow(g, capacity, rev, s, t, *file_in_ptr);

    /*process graph*/
    tCapMapValue augmented_flow = 0;

    // we take the source node and check for each outgoing edge e which has a
    // target(p) if we can augment that path
    out_edge_iterator oei, oe_end;
    for (boost::tie(oei, oe_end) = out_edges(s, g); oei != oe_end; ++oei)
    {
        auto from_source = *oei;
        auto v = target(from_source, g);
        edge_descriptor to_sink;
        bool is_there;
        boost::tie(to_sink, is_there) = edge(v, t, g);
        if (is_there)
        {
            if (get(capacity, to_sink) > get(capacity, from_source))
            {
                auto to_augment = get(capacity, from_source);
                capacity[from_source] = 0;
                capacity[to_sink] -= to_augment;
                augmented_flow += to_augment;
            }
            else
            {
                tCapMapValue to_augment = get(capacity, to_sink);
                capacity[to_sink] = 0;
                capacity[from_source] -= to_augment;
                augmented_flow += to_augment;
            }
        }
    }

    // remove edges with zero capacity (most of them are the reverse edges)
    zero_edge_capacity< tCapMap > filter(capacity);
    remove_edge_if(filter, g);

    /*write the graph back to stdout */
    write_dimacs_max_flow(
        g, capacity, identity_property_map(), s, t, std::cout);
    // print flow we augmented to std::cerr
    std::cerr << "removed " << augmented_flow
              << " from SOURCE->NODE->SINK connects" << std::endl;
    return 0;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(read_write_dimacs-eg)
{
    TEST_ASSERT(test_main() == 0);
    /* esp32: test failed due to memory leak */
}
