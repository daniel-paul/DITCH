#pragma once
#include "types.h"
#include <vector>
#include <string>

struct HypergraphCSR {
    // Number of vertices and hyperedges
    VertexId num_vertices = 0;
    EdgeId   num_hyperedges = 0;

    // CSR for hyperedges -> vertices
    VertexId*   edge_offsets = nullptr;   // Starting index of each hyperedge in ed_vertices
    VertexId*   edge_sizes   = nullptr;   // Number of vertices in each hyperedge
    VertexId* ed_vertices  = nullptr;   // Concatenated vertices of all hyperedges

    // CSR for vertices -> hyperedges
    EdgeId* vertex_offset = nullptr;   // Starting index of each vertex in ve_hyperedges
    EdgeId* degrees  = nullptr;        // Number of hyperedges per vertex
    EdgeId*   ve_hyperedges = nullptr;   // Concatenated hyperedges for all vertices

    // Default constructor
    HypergraphCSR() = default;

    // Destructor
    ~HypergraphCSR() {
        delete[] edge_offsets;
        delete[] edge_sizes;
        delete[] ed_vertices;
        delete[] vertex_offset;
        delete[] degrees;
        delete[] ve_hyperedges;
    }

    void compute_degrees(const std::string& output);
};

struct DirHypergraphCSR {
    // Number of vertices and hyperedges
    VertexId num_vertices = 0;
    EdgeId   num_hyperedges = 0;

    // CSR for hyperedges -> vertices
    VertexId*   edge_offsets = nullptr;   // Starting index of each hyperedge in ed_vertices
    VertexId*   edge_sizes   = nullptr;   // Number of vertices in each hyperedge
    VertexId* ed_vertices  = nullptr;   // Concatenated vertices of all hyperedges

    // CSR for vertices -> hyperedges
    EdgeId* vertex_offset = nullptr;   // Starting index of each vertex in ve_hyperedges
    EdgeId* outdegrees  = nullptr;        // Number of hyperedges per vertex
    EdgeId*   ve_hyperedges = nullptr;   // Concatenated hyperedges for all vertices
    VertexId* vertex_hyperedges_pos = nullptr; //vertex v takes the vertex_hyperedges_pos[i] of the hyperedge in ve_hyperedges[i] for i \in [vertex_offset[v],vertex_offset[v+1]]

    //Auxiliary arrays to store edge-degrees
    EdgeId* edge_intersections = nullptr;
    EdgeId* n_parents = nullptr;
    EdgeId* n_children = nullptr;

    // Default constructor
    DirHypergraphCSR() = default;

    // Destructor
    ~DirHypergraphCSR() {
        delete[] edge_sizes;
        delete[] edge_offsets;
        delete[] ed_vertices;
        delete[] vertex_offset;
        delete[] outdegrees;
        delete[] ve_hyperedges;
        delete[] vertex_hyperedges_pos;
        delete[] edge_intersections;
        delete[] n_parents;
        delete[] n_children;
    }

    void compute_edge_degrees();
    void compute_outdegrees(const std::string& output);
    bool isCommonSource(VertexId v,EdgeId e1, EdgeId e2);
    bool contains_vertex(EdgeId e, VertexId v);
};


VertexId contains_vertex_return_pos(const DirHypergraphCSR& H, EdgeId e, VertexId v);
void print_edge(const DirHypergraphCSR& dirH, EdgeId e);