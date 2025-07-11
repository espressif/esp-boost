//=======================================================================
// Copyright 2002 Indiana University.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <boost/graph/graph_as_tree.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/cstdlib.hpp>
#include <iostream>

class tree_printer
{
public:
    template < typename Node, typename Tree > void preorder(Node, Tree&)
    {
        std::cout << "(";
    }
    template < typename Node, typename Tree > void inorder(Node n, Tree& t)
    {
        std::cout << get(boost::vertex_name, t)[n];
    }
    template < typename Node, typename Tree > void postorder(Node, Tree&)
    {
        std::cout << ")";
    }
};

static int test_main()
{
    using namespace boost;
    typedef adjacency_list< vecS, vecS, directedS,
        property< vertex_name_t, std::string > >
        graph_t;
    typedef graph_traits< graph_t >::vertex_descriptor vertex_t;

    graph_t g;

    auto a = add_vertex(g), b = add_vertex(g), c = add_vertex(g);

    add_edge(a, b, g);
    add_edge(a, c, g);

    auto name = get(vertex_name, g);
    name[a] = "A";
    name[b] = "B";
    name[c] = "C";

    typedef iterator_property_map< std::vector< vertex_t >::iterator,
        property_map< graph_t, vertex_index_t >::type >
        parent_map_t;
    std::vector< vertex_t > parent(num_vertices(g));
    typedef graph_as_tree< graph_t, parent_map_t > tree_t;
    tree_t t(
        g, a, make_iterator_property_map(parent.begin(), get(vertex_index, g)));

    tree_printer vis;
    traverse_tree(a, t, vis);

    return exit_success;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(graph_as_tree)
{
    TEST_ASSERT(test_main() == 0);
}
