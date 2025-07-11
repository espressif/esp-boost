//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/config.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include "common_components.hpp"
#include "embedded_files_lut.hpp"

using namespace boost;

namespace std
{
template < typename T >
std::istream& operator>>(std::istream& in, std::pair< T, T >& p)
{
    in >> p.first >> p.second;
    return in;
}
}

typedef adjacency_list< listS, // Store out-edges of each vertex in a std::list
    vecS, // Store vertex set in a std::vector
    directedS // The file dependency graph is directed
    >
    file_dep_graph;

typedef graph_traits< file_dep_graph >::vertex_descriptor vertex_t;
typedef graph_traits< file_dep_graph >::edge_descriptor edge_t;

template < typename Visitor >
void dfs_v1(
    const file_dep_graph& g, vertex_t u, default_color_type* color, Visitor vis)
{
    color[u] = gray_color;
    vis.discover_vertex(u, g);
    graph_traits< file_dep_graph >::out_edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = out_edges(u, g); ei != ei_end; ++ei)
    {
        if (color[target(*ei, g)] == white_color)
        {
            vis.tree_edge(*ei, g);
            dfs_v1(g, target(*ei, g), color, vis);
        }
        else if (color[target(*ei, g)] == gray_color)
            vis.back_edge(*ei, g);
        else
            vis.forward_or_cross_edge(*ei, g);
    }
    color[u] = black_color;
    vis.finish_vertex(u, g);
}

template < typename Visitor >
void generic_dfs_v1(const file_dep_graph& g, Visitor vis)
{
    std::vector< default_color_type > color(num_vertices(g), white_color);
    graph_traits< file_dep_graph >::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
        if (color[*vi] == white_color)
            dfs_v1(g, *vi, &color[0], vis);
    }
}

struct dfs_visitor_default
{
    template < typename V, typename G > void discover_vertex(V, const G&) {}

    template < typename E, typename G > void tree_edge(E, const G&) {}

    template < typename E, typename G > void back_edge(E, const G&) {}

    template < typename E, typename G > void forward_or_cross_edge(E, const G&)
    {
    }

    template < typename V, typename G > void finish_vertex(V, const G&) {}
};

struct topo_visitor : public dfs_visitor_default
{
    topo_visitor(vertex_t*& order) : topo_order(order) {}
    void finish_vertex(vertex_t u, const file_dep_graph&) { *--topo_order = u; }
    vertex_t*& topo_order;
};

static void topo_sort(const file_dep_graph& g, vertex_t* topo_order)
{
    topo_visitor vis(topo_order);
    generic_dfs_v1(g, vis);
}

static int test_main(int argc, const char** argv)
{
    // std::ifstream file_in(argc >= 2 ? argv[1] : "makefile-dependencies.dat");
    const char* dep_file_name =
        argc >= 2 ? argv[1] : "makefile-dependencies.dat";
    auto file_in_ptr = open_input_stream(embedded_files, dep_file_name);
    std::istream& file_in = *file_in_ptr;
    typedef graph_traits< file_dep_graph >::vertices_size_type size_type;
    size_type n_vertices;
    file_in >> n_vertices; // read in number of vertices
    std::istream_iterator< std::pair< size_type, size_type > > input_begin(
        file_in),
        input_end;

#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
    // VC++ can't handle the iterator constructor
    file_dep_graph g(n_vertices);
    while (input_begin != input_end)
    {
        size_type i, j;
        boost::tie(i, j) = *input_begin++;
        add_edge(i, j, g);
    }
#else
    file_dep_graph g(input_begin, input_end, n_vertices);
#endif

    std::vector< std::string > name(num_vertices(g));
    // std::ifstream name_in(argc >= 3 ? argv[2] : "makefile-target-names.dat");
    const char* target_file_name
        = argc >= 3 ? argv[2] : "makefile-target-names.dat";
    auto name_in_ptr = open_input_stream(embedded_files, target_file_name);
    std::istream& name_in = *name_in_ptr;
    graph_traits< file_dep_graph >::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
        name_in >> name[*vi];

    std::vector< vertex_t > order(num_vertices(g));
    topo_sort(g, &order[0] + num_vertices(g));
    for (size_type i = 0; i < num_vertices(g); ++i)
        std::cout << name[order[i]] << std::endl;

    return 0;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(topo-sort-file-dep2)
{
    TEST_ASSERT(test_main(0, nullptr) == 0);
}
