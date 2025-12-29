#include "read_hypergraph.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

//Reads the hypergraph from the file
void read_hypergraph(HypergraphCSR& H, const std::string& filename) {

    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << filename << "\n";
        return;
    }

    // First pass: count number of hyperedges and total vertices
    VertexId total_vertices = 0;
    std::string line;

    H.num_vertices = 0;
    H.num_hyperedges = 0;
    int rank = 0;

    while (std::getline(infile, line)) {
        if (!line.empty()) {
            std::replace(line.begin(), line.end(), ',', ' ');
            std::istringstream ss(line);
            VertexId v;
            VertexId count = 0;
            while (ss >> v) count++;
            total_vertices += count;
            H.num_hyperedges++;
            if (v + 1 > H.num_vertices) H.num_vertices = v + 1;
            if (rank < count) rank = count;
        }
    }
    std::cout << "Rank: " << rank << "\n";
    std::cout << "Average arity: "<< total_vertices/(H.num_hyperedges+0.0) << "\n";
    
    // Allocate arrays
    H.ed_vertices = new VertexId[total_vertices];
    H.edge_offsets  = new VertexId[H.num_hyperedges];
    H.edge_sizes    = new VertexId[H.num_hyperedges];
    H.degrees = new EdgeId[H.num_vertices]();
    H.vertex_offset = new EdgeId[H.num_vertices];
    H.ve_hyperedges = new EdgeId[total_vertices];

    // Second pass: fill arrays
    infile.clear();
    infile.seekg(0);

    VertexId offset = 0;
    EdgeId eid = 0;

    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        std::replace(line.begin(), line.end(), ',', ' ');
        std::istringstream ss(line);
        VertexId v;
        H.edge_offsets[eid] = offset;
        VertexId edge_size = 0;
        while (ss >> v) {
            H.ed_vertices[offset++] = v;
            edge_size++;
            H.degrees[v]++;
        }
        H.edge_sizes[eid] = edge_size;
        eid++;
    }
    infile.close();

    // vertex offsets
    EdgeId total = 0;
        for (EdgeId v = 0; v < H.num_vertices; v++) {
            H.vertex_offset[v] = total;
            total += H.degrees[v];
    }

    //Complete vertex CSR
    EdgeId* counts = new EdgeId[H.num_vertices]();
    for (EdgeId e = 0; e < H.num_hyperedges; e++) {
        for (VertexId i = 0; i < H.edge_sizes[e]; i++) {
            VertexId v = H.ed_vertices[H.edge_offsets[e] + i];
            VertexId pos = H.vertex_offset[v] + counts[v];
            if (pos >= total_vertices) {
                std::cerr << "Error: pos out of bounds! e=" << e << " i=" << i
                        << " v=" << v << " pos=" << pos << "\n";
            }
            H.ve_hyperedges[pos] = e;
            counts[v]++;
        }
    }
    delete[] counts;
}