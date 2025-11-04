#include "HypergraphCSR.h"
#include <algorithm>
#include <vector>
#include <iostream>

VertexId contains_vertex_return_pos(const DirHypergraphCSR& H, EdgeId e, VertexId v) {
    for(VertexId i = 0; i < H.edge_sizes[e]; i++){
        if(H.ed_vertices[H.edge_offsets[e]+i] == v)
            return i;
    }
    return -1;
}

void print_edge(const DirHypergraphCSR& dirH, EdgeId e){
    for(int i = dirH.edge_offsets[e]; i < dirH.edge_offsets[e]+dirH.edge_sizes[e]; i++) {
        std::cout<<dirH.ed_vertices[i]<<" ";
    }
        std::cout<<"\n";
}

void HypergraphCSR::compute_sum_degrees() {
    EdgeId count = 0;
    for(VertexId v = 0; v < num_vertices; v++){
        count += degrees[v]*degrees[v]*degrees[v];
    }
    std::cout<<"sum of degrees cube: "<<count<<"\n";
}

void DirHypergraphCSR::compute_sum_outdegrees() {
    EdgeId count = 0;
    for(VertexId v = 0; v < num_vertices; v++){
        count += outdegrees[v]*outdegrees[v]*outdegrees[v];
    }
    std::cout<<"sum of outdegrees cube: "<<count<<"\n";
}

//Gets the number of edges intersecting 
void DirHypergraphCSR::compute_edge_degrees() {
    //Compute indegrees
    EdgeId* indegrees = new EdgeId[num_vertices]();
    for(EdgeId e = 0; e < num_hyperedges; e++){
        VertexId v = ed_vertices[edge_offsets[e] + edge_sizes[e]-1];
        indegrees[v]++;
    }

    for (EdgeId e1 = 0; e1 < num_hyperedges; e1++){
        VertexId degree_sum = 0;
        for(VertexId iv1 = edge_offsets[e1]; iv1 < edge_offsets[e1] +  edge_sizes[e1]; iv1++){
            VertexId v1 = ed_vertices[iv1];
            degree_sum += indegrees[v1];

            for(EdgeId ie2 = vertex_offset[v1]; ie2 < vertex_offset[v1] + outdegrees[v1]; ie2++){
                EdgeId e2 = ve_hyperedges[ie2];
                //check that v1 is the first vertex in the intersection of e1-e2 and e2 does not end in e1
                if(!isCommonSource(v1,e1,e2)) continue;

                VertexId laste2 = ed_vertices[edge_offsets[e2] + edge_sizes[e2] - 1];
                if(contains_vertex(e1,laste2)) continue;
                degree_sum += 1;
            }
        }
        //We substract 1 because e1 is counting itself
        edge_intersections[e1] = degree_sum - 1;
    }
    delete[] indegrees;
}

//Checks if v is the first vertex in the intersection of e1 and e2
bool DirHypergraphCSR::isCommonSource(VertexId v,EdgeId e1, EdgeId e2) {
    VertexId p1 = edge_offsets[e1];
    VertexId p2 = edge_offsets[e2];
    VertexId p1end = edge_offsets[e1] + edge_sizes[e1];
    VertexId p2end = edge_offsets[e2] + edge_sizes[e2];
    while(p1<p1end && p2<p2end){
        VertexId v1 = ed_vertices[p1];
        VertexId v2 = ed_vertices[p2];
        if(v1==v2){
            if(v1 == v) return true;
            else return false;
        }
        if(v1 < v2) p1++;
        else p2++;
    }
    return false;
}

bool DirHypergraphCSR::contains_vertex(EdgeId e, VertexId v) {
    VertexId* begin = &ed_vertices[edge_offsets[e]];
    VertexId* end   = begin + edge_sizes[e];
    return std::binary_search(begin, end, v);
}