#include "degeneracy.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

void compute_degeneracy_ordering(DirHypergraphCSR& dirH, HypergraphCSR& H, VertexId* ordering) {
    //Filter singleton hyperedges and recalculate degrees
    bool* deletedEdges = new bool[H.num_hyperedges]();
    bool* deletedVertices = new bool[H.num_vertices]();


    EdgeId* degrees = new EdgeId[H.num_vertices];
    VertexId* sizes = new VertexId[H.num_hyperedges];

    for (VertexId i = 0; i < H.num_vertices; i++){
        degrees[i] = H.degrees[i];
    }
    for (EdgeId e = 0; e < H.num_hyperedges; e++){
        if(H.edge_sizes[e] < 2) {
            deletedEdges[e] = true;
            if (H.edge_sizes[e] == 1) {
                VertexId v = H.ed_vertices[H.edge_offsets[e]];
                assert(0 <= v && v < H.num_vertices);
                degrees[v]--;
            }
        }
        sizes[e] = H.edge_sizes[e];
    }

    //Filter vertices with no degree
    VertexId ndeleted = 0;
    VertexId* map = new VertexId[H.num_vertices]();

    EdgeId max_degree = 0;
    for (VertexId i = 0; i < H.num_vertices; i++){
        if (degrees[i] == 0){
            map[i] = H.num_vertices;
            deletedVertices[i] = true;
            ndeleted++;
        }
        if(degrees[i] > max_degree){
            max_degree = degrees[i];
        }
    }

    dirH.num_vertices = H.num_vertices - ndeleted;
    dirH.num_hyperedges = H.num_hyperedges;

    //Create degeneracy ordering

    VertexId** buckets = new VertexId*[max_degree+1];
    VertexId* bucket_sizes = new VertexId[max_degree+1]();
    for(EdgeId d = 0; d <= max_degree; ++d) {
        buckets[d] = new VertexId[H.num_vertices]; // overallocate, at most n vertices per bucket
        bucket_sizes[d] = 0;
    }

    for(VertexId v = 0; v < H.num_vertices; ++v) {
        EdgeId d = degrees[v];
        assert(d <= max_degree);
        buckets[d][bucket_sizes[d]++] = v;
    }
    VertexId ordering_pos = 0;
    EdgeId current_min_degree = 0;
    EdgeId* outdegrees = new EdgeId[H.num_vertices]();

    while(ordering_pos < H.num_vertices - ndeleted) {
        // find the next non-empty bucket
        while(current_min_degree <= max_degree && bucket_sizes[current_min_degree] == 0) {
            current_min_degree++;
        }
        if(current_min_degree > max_degree) break;
        
        // take a vertex
        VertexId idx = --bucket_sizes[current_min_degree];
        assert(bucket_sizes[current_min_degree] >= 0);
        VertexId v = buckets[current_min_degree][idx];
        assert(0 <= v && v < H.num_vertices);
        if(deletedVertices[v]) continue;

        outdegrees[v] = current_min_degree;
        deletedVertices[v] = true;
        map[v] = ordering_pos;
        ordering[ordering_pos] = v;
        ordering_pos++;

        // update neighbors
        EdgeId start = H.vertex_offset[v];
        EdgeId end   = H.vertex_offset[v]+H.degrees[v];
        for(EdgeId i = start; i < end; ++i) {
            EdgeId e = H.ve_hyperedges[i];
            assert(0 <= e && e < H.num_hyperedges);
            if(deletedEdges[e]) continue;
            sizes[e]--;
            if (sizes[e] < 2) {
                deletedEdges[e] =true;
                for(VertexId j=0; j< H.edge_sizes[e]; j++){
                    VertexId u = H.ed_vertices[H.edge_offsets[e]+j];
                    assert(0 <= u && u < H.num_vertices);
                    if(!deletedVertices[u]) {
                        degrees[u]--;
                        buckets[degrees[u]][bucket_sizes[degrees[u]]++] = u;
                        if (bucket_sizes[degrees[u]] > H.num_vertices){
                            std::cout<<"ERROR";
                        }
                        if (degrees[u]< current_min_degree){
                            current_min_degree = degrees[u];
                        }
                    }
                }
            }
        }
    }

    //some cleanup
    delete[] bucket_sizes;
    for (EdgeId d = 0; d <= max_degree; ++d) {
        delete[] buckets[d];   // free each inner array
    }
    delete[] buckets;
    delete[] deletedEdges;
    delete[] deletedVertices;
    delete[] degrees;
    delete[] sizes;



    //Create the hyperedge sizes and offsets arrays
    dirH.edge_offsets = new EdgeId[dirH.num_hyperedges];
    dirH.edge_sizes   = new EdgeId[dirH.num_hyperedges];
    EdgeId edge_offset = 0;
    for(EdgeId e=0; e< dirH.num_hyperedges; e++){
        dirH.edge_offsets[e]=edge_offset;
        if (H.edge_sizes[e] < 2) {
            dirH.edge_sizes[e]=0;
        }
        else {
            dirH.edge_sizes[e]=H.edge_sizes[e];
        }
        edge_offset += dirH.edge_sizes[e];
    }
    dirH.ed_vertices = new VertexId[edge_offset];
    
    //Create the vertex outdegree and offset arrays
    EdgeId maxoutdegree = 0;
    dirH.outdegrees = new EdgeId[dirH.num_vertices];
    dirH.vertex_offset = new EdgeId[dirH.num_vertices];
    EdgeId* vertex_current = new EdgeId[dirH.num_vertices]();
    EdgeId vertexoffset = 0;
    for(VertexId v=0; v< dirH.num_vertices; v++){
        dirH.vertex_offset[v]= vertexoffset;
        dirH.outdegrees[v] = outdegrees[ordering[v]];
        vertexoffset += dirH.outdegrees[v];
        if (outdegrees[v] > maxoutdegree){
            maxoutdegree = outdegrees[v];
        }
    }
    dirH.ve_hyperedges = new EdgeId[vertexoffset];
    dirH.vertex_hyperedges_pos = new EdgeId[vertexoffset];

    //Populate the CSR's

    for(EdgeId e=0; e< dirH.num_hyperedges; e++){
        if (dirH.edge_sizes[e]==0) {
            continue;
        }
        std::vector<VertexId> indices;
        for (VertexId i = H.edge_offsets[e]; i < H.edge_offsets[e]+H.edge_sizes[e]; i++) {
            VertexId v = map[H.ed_vertices[i]];
            if(v == H.num_vertices){
                std::cout << "error, non-mapped vertex\n";
            }
            indices.push_back(v);
        }
        std::sort(indices.begin(),indices.end());
        for(VertexId i = 0; i < indices.size(); i++){
            VertexId v = indices[i];
            assert(i + dirH.edge_offsets[e] < edge_offset);
            dirH.ed_vertices[i+dirH.edge_offsets[e]] = v;
            if(i < indices.size()-1){
                assert(dirH.vertex_offset[v]+vertex_current[v] < vertexoffset);
                dirH.ve_hyperedges[dirH.vertex_offset[v]+vertex_current[v]] = e;
                dirH.vertex_hyperedges_pos[dirH.vertex_offset[v]+vertex_current[v]] = i;
                vertex_current[v]++;
            }
        }
    }
    std::cout<< "max degree of undirected hypergraph: "<< max_degree << "\n";
    std::cout << "max outdegree of directed hypergraph: "<< maxoutdegree << "\n";



    delete[] outdegrees;
    delete[] map;
    delete[] vertex_current;


    // print degeneracy ordering
    // std::cout << "Degeneracy ordering:" << std::endl;
    // for (size_t i = 0; i < H.num_vertices; i++) {
    //     std::cout << ordering[i] << " ";
    //     std::cout << outdegrees[ordering[i]] << "\n"; 
    // }
    // std::cout << std::endl;
}
