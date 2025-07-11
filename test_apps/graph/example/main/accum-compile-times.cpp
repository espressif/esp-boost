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
#include <numeric>
#include <iterator>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/property_iter_range.hpp>
#include "common_components.hpp"
#include "embedded_files_lut.hpp"

namespace std
{
template < typename T >
std::istream& operator>>(std::istream& in, std::pair< T, T >& p)
{
    in >> p.first >> p.second;
    return in;
}
}

namespace boost
{
enum vertex_compile_cost_t
{
    vertex_compile_cost
};
BOOST_INSTALL_PROPERTY(vertex, compile_cost);
}

using namespace boost;

typedef adjacency_list< listS, // Store out-edges of each vertex in a std::list
    listS, // Store vertex set in a std::list
    directedS, // The file dependency graph is directed
    // vertex properties
    property< vertex_name_t, std::string,
        property< vertex_compile_cost_t, float,
            property< vertex_distance_t, float,
                property< vertex_color_t, default_color_type > > > >,
    // an edge property
    property< edge_weight_t, float > >
    file_dep_graph2;

typedef graph_traits< file_dep_graph2 >::vertex_descriptor vertex_t;
typedef graph_traits< file_dep_graph2 >::edge_descriptor edge_t;

static int test_main(int argc, const char** argv)
{
    // std::ifstream file_in(argc >= 2 ? argv[1] : "makefile-dependencies.dat");
    const char* dep_file_name =
        argc >= 2 ? argv[1] : "makefile-dependencies.dat";
    auto file_in_ptr = open_input_stream(embedded_files, dep_file_name);
    std::istream& file_in = *file_in_ptr;
    typedef graph_traits< file_dep_graph2 >::vertices_size_type size_type;
    size_type n_vertices;
    file_in >> n_vertices; // read in number of vertices
#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
    // std::istream_iterator causes trouble with VC++
    std::vector< vertex_t > id2vertex;
    file_dep_graph2 g;
    for (std::size_t i = 0; i < n_vertices; ++i)
        id2vertex.push_back(add_vertex(g));
    std::pair< size_type, size_type > p;
    while (file_in >> p)
        add_edge(id2vertex[p.first], id2vertex[p.second], g);
#else
    std::istream_iterator< std::pair< size_type, size_type > > input_begin(
        file_in),
        input_end;
    file_dep_graph2 g(input_begin, input_end, n_vertices);
#endif

    auto name_map = get(vertex_name, g);
    auto compile_cost_map = get(vertex_compile_cost, g);

    // std::ifstream name_in(argc >= 3 ? argv[2] : "makefile-target-names.dat");
    const char* target_file_name
        = argc >= 3 ? argv[2] : "makefile-target-names.dat";
    auto name_in_ptr = open_input_stream(embedded_files, target_file_name);
    std::istream& name_in = *name_in_ptr;
    // std::ifstream compile_cost_in(
    //     argc >= 4 ? argv[3] : "target-compile-costs.dat");
    const char* compile_cost_file_name
        = argc >= 4 ? argv[3] : "target-compile-costs.dat";
    auto compile_cost_in_ptr = open_input_stream(embedded_files, compile_cost_file_name);
    std::istream& compile_cost_in = *compile_cost_in_ptr;
    graph_traits< file_dep_graph2 >::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    {
        name_in >> name_map[*vi];
        compile_cost_in >> compile_cost_map[*vi];
    }

    graph_property_iter_range< file_dep_graph2,
        vertex_compile_cost_t >::iterator ci,
        ci_end;
    boost::tie(ci, ci_end) = get_property_iter_range(g, vertex_compile_cost);
    std::cout << "total (sequential) compile time: "
              << std::accumulate(ci, ci_end, 0.0) << std::endl;

    return 0;
}

#include "common.hpp"

BOOST_AUTO_TEST_CASE(accum_compile_times)
{
    TEST_ASSERT(test_main(0, nullptr) == 0);
}
