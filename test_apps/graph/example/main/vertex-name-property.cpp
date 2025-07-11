//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <common_components.hpp>
#include <embedded_files_lut.hpp>

using namespace boost;

template < typename Graph, typename VertexNamePropertyMap >
void read_graph_file(std::istream& graph_in, std::istream& name_in, Graph& g,
    VertexNamePropertyMap name_map)
{
    typedef typename graph_traits< Graph >::vertices_size_type size_type;
    size_type n_vertices;
    typename graph_traits< Graph >::vertex_descriptor u;
    typename property_traits< VertexNamePropertyMap >::value_type name;

    graph_in >> n_vertices; // read in number of vertices
    for (size_type i = 0; i < n_vertices; ++i)
    { // Add n vertices to the graph
        u = add_vertex(g);
        name_in >> name;
        put(name_map, u, name); // ** Attach name property to vertex u **
    }
    size_type src, targ;
    while (graph_in >> src) // Read in edges
        if (graph_in >> targ)
            add_edge(src, targ, g); // add an edge to the graph
        else
            break;
}

static int test_main(int argc, const char** argv)
{
    typedef adjacency_list< listS, // Store out-edges of each vertex in a
                                   // std::list
        vecS, // Store vertex set in a std::vector
        directedS, // The graph is directed
        property< vertex_name_t, std::string > // Add a vertex property
        >
        graph_type;

    graph_type g; // use default constructor to create empty graph
    const char* dep_file_name
        = argc >= 2 ? argv[1] : "makefile-dependencies.dat";
    const char* target_file_name
        = argc >= 3 ? argv[2] : "makefile-target-names.dat";
    auto file_in_ptr = open_input_stream(embedded_files, dep_file_name);
    std::istream& file_in = *file_in_ptr;
    auto name_in_ptr = open_input_stream(embedded_files, target_file_name);
    std::istream& name_in = *name_in_ptr;
    if (!file_in)
    {
        std::cerr << "** Error: could not open file " 
                  << std::endl;
        return -1;
    }
    if (!name_in)
    {
        std::cerr << "** Error: could not open file "
                  << std::endl;
        return -1;
    }

    // Obtain internal property map from the graph
    auto name_map = get(vertex_name, g);
    read_graph_file(file_in, name_in, g, name_map);

    // Create storage for last modified times
    std::vector< time_t > last_mod_vec(num_vertices(g));
    // Create nickname for the property map type
    typedef iterator_property_map< std::vector< time_t >::iterator,
        property_map< graph_type, vertex_index_t >::type, time_t, time_t& >
        iter_map_t;
    // Create last modified time property map
    iter_map_t mod_time_map(last_mod_vec.begin(), get(vertex_index, g));

    assert(num_vertices(g) == 15);
    assert(num_edges(g) == 19);
    return 0;
}


#include "common.hpp"

BOOST_AUTO_TEST_CASE(vertex-name-property)
{
    TEST_ASSERT(test_main(0, nullptr) == 0);
}
