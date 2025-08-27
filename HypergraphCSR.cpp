#include "HypergraphCSR.h"
#include <algorithm>
#include <vector>
#include <iostream>

bool contains_vertex(const DirHypergraphCSR& H, EdgeId e, VertexId v) {
    VertexId* begin = &H.ed_vertices[H.edge_offsets[e]];
    VertexId* end   = begin + H.edge_sizes[e];
    return std::binary_search(begin, end, v);
}

// bool contains_vertex(const DirHypergraphCSR& H, EdgeId e, VertexId v) {
//     for(VertexId i = H.edge_offsets[e]; i < H.edge_offsets[e]+H.edge_sizes[e]; i++){
//         if(H.ed_vertices[i] == v)
//             return true;
//     }
//     return false;
// }

void print_edge(const DirHypergraphCSR& dirH, EdgeId e){
    for(int i = dirH.edge_offsets[e]; i < dirH.edge_offsets[e]+dirH.edge_sizes[e]; i++) {
        std::cout<<dirH.ed_vertices[i]<<" ";
    }
        std::cout<<"\n";
}