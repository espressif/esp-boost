//=======================================================================
// Copyright 2001 Indiana University.
// Author: Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/config.hpp>

#ifdef BOOST_MSVC
// Without this we get hard errors, not warnings:
#pragma warning(disable : 4703)
#endif

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <iostream>

enum family
{
    Jeanie,
    Debbie,
    Rick,
    John,
    Amanda,
    Margaret,
    Benjamin,
    N
};

static int test_main()
{
    using namespace boost;
    const char* name[] = { "Jeanie", "Debbie", "Rick", "John", "Amanda",
        "Margaret", "Benjamin" };

    adjacency_list<> g(N);
    add_edge(Jeanie, Debbie, g);
    add_edge(Jeanie, Rick, g);
    add_edge(Jeanie, John, g);
    add_edge(Debbie, Amanda, g);
    add_edge(Rick, Margaret, g);
    add_edge(John, Benjamin, g);

    graph_traits< adjacency_list<> >::vertex_iterator i, end;
    graph_traits< adjacency_list<> >::adjacency_iterator ai, a_end;
    auto index_map = get(vertex_index, g);

    BGL_FORALL_VERTICES(i, g, adjacency_list<>)
    {
        std::cout << name[get(index_map, i)];

        if (out_degree(i, g) == 0)
            std::cout << " has no children";
        else
            std::cout << " is the parent of ";

        BGL_FORALL_ADJ(i, j, g, adjacency_list<>)
        std::cout << name[get(index_map, j)] << ", ";
        std::cout << std::endl;
    }
    return EXIT_SUCCESS;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(iteration_macros)
{
    TEST_ASSERT(test_main() == 0);
}
