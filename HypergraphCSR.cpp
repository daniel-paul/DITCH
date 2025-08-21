#include "HypergraphCSR.h"
#include <algorithm>
#include <vector>

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