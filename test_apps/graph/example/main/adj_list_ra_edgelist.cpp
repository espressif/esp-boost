//=======================================================================
// Copyright 2001 Indiana University.
// Author: Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <iostream>

static int test_main()
{
    using namespace boost;
    typedef adjacency_list< vecS, vecS, bidirectionalS, no_property,
        property< int, edge_weight_t >, no_property, vecS >
        Graph;

    const std::size_t n = 3;
    typedef std::pair< std::size_t, std::size_t > E;
    E edge_array[] = { E(0, 1), E(0, 2), E(0, 1) };
    const std::size_t m = sizeof(edge_array) / sizeof(E);
    Graph g(edge_array, edge_array + m, n);

    graph_traits< Graph >::edge_iterator edge_iterator;
    for (std::size_t i = 0; i < m; ++i)
    {
        const graph_traits< Graph >::edge_iterator e = edges(g).first + i;
        std::cout << *e << " ";
    }
    std::cout << std::endl;

    return 0;
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(adj_list_ra_edgelist)
{
  TEST_ASSERT(test_main() == 0);
}
