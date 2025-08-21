#include "count_hypertriangles.h"
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cassert>
#include <algorithm>


// struct TripleHash {
//     size_t operator()(const std::tuple<EdgeId, EdgeId, EdgeId>& t) const {
//         EdgeId a = std::get<0>(t);
//         EdgeId b = std::get<1>(t);
//         EdgeId c = std::get<2>(t);
//         return std::hash<uint64_t>()(((uint64_t)a << 40) | ((uint64_t)b << 20) | (uint64_t)c);
//     }
// };  

struct TripleIntersections {
    int i12_not3 = 0;  // |e1 ∩ e2 \ e3|
    int i23_not1 = 0;  // |e2 ∩ e3 \ e1|
    int i31_not2 = 0;  // |e3 ∩ e1 \ e2|
};

EdgeId triple_intersections(const DirHypergraphCSR& H,
                                         EdgeId e1, EdgeId e2, EdgeId e3) {
    TripleIntersections res;

    auto p1 = H.edge_offsets[e1], p1_end = p1 + H.edge_sizes[e1];
    auto p2 = H.edge_offsets[e2], p2_end = p2 + H.edge_sizes[e2];
    auto p3 = H.edge_offsets[e3], p3_end = p3 + H.edge_sizes[e3];

    while (p1 < p1_end || p2 < p2_end || p3 < p3_end) {
        VertexId v1 = (p1 < p1_end) ? H.ed_vertices[p1] : std::numeric_limits<VertexId>::max();
        VertexId v2 = (p2 < p2_end) ? H.ed_vertices[p2] : std::numeric_limits<VertexId>::max();
        VertexId v3 = (p3 < p3_end) ? H.ed_vertices[p3] : std::numeric_limits<VertexId>::max();

        VertexId minv = std::min({v1, v2, v3});

        bool in1 = (v1 == minv);
        bool in2 = (v2 == minv);
        bool in3 = (v3 == minv);

        // Count memberships
        int count = in1 + in2 + in3;
        if (count == 2) {
            if (in1 && in2 && !in3) res.i12_not3++;
            if (in2 && in3 && !in1) res.i23_not1++;
            if (in3 && in1 && !in2) res.i31_not2++;
        }

        if (in1) ++p1;
        if (in2) ++p2;
        if (in3) ++p3;
    }
    return res.i12_not3 * res.i23_not1 * res.i31_not2;
}

void count_hypertriangles(DirHypergraphCSR& dirH, VertexId* ordering){
    // std::unordered_set<std::tuple<EdgeId,EdgeId,EdgeId>, TripleHash> uniqueTriplets;
    std::unordered_map<long long, long long> freq;

    //Loop for starting vertices
    for (VertexId v = 0; v < dirH.num_vertices; v++){
        //Loop for couple of neighboring hyperedges
        for(EdgeId ie1 = dirH.vertex_offset[v]; ie1 < dirH.vertex_offset[v]+dirH.outdegrees[v]-1; ie1++) {
            for (EdgeId ie2 = ie1+1; ie2 < dirH.vertex_offset[v]+dirH.outdegrees[v]; ie2++) {
                EdgeId e1 = dirH.ve_hyperedges[ie1];
                EdgeId e2 = dirH.ve_hyperedges[ie2];
                //Loop for each possible outneighbor in each neighboring hyperedge
                for(VertexId iw1 = dirH.edge_offsets[e1] + dirH.vertex_hyperedges_pos[ie1]+1; iw1 < dirH.edge_offsets[e1] + dirH.edge_sizes[e1]; iw1++){
                    //Check that w1 is not in e2
                    VertexId w1 = dirH.ed_vertices[iw1];
                    if (contains_vertex(dirH, e2, w1)){
                        continue;
                    }
                    for(VertexId iw2 = dirH.edge_offsets[e2] + dirH.vertex_hyperedges_pos[ie2]+1; iw2 < dirH.edge_offsets[e2] + dirH.edge_sizes[e2]; iw2++){

                        VertexId w2 = dirH.ed_vertices[iw2];
                        //check w2 is not w1
                        if(w1 == w2){
                            continue;
                        }

                        //Check that w2 is not in e1
                        if (contains_vertex(dirH, e1, w2)){
                            continue;
                        }
                        //let x1 be the lesser of w1,w2
                        VertexId x1 = w1, x2= w2;
                        if(w1 > w2) {
                            x1 = w2;
                            x2 = w1;
                        }
                        
                        //Final loop through outneighbors of w1
                        for (EdgeId ie3 = dirH.vertex_offset[x1]; ie3 < dirH.vertex_offset[x1]+dirH.outdegrees[x1]; ie3++){
                            EdgeId e3 = dirH.ve_hyperedges[ie3];
                            if (e3==e1 || e3==e2){
                                continue;
                            }
                            //check that e3 does not contain v and contains w2.
                            if (!contains_vertex(dirH, e3, x2) || contains_vertex(dirH, e3, v)){
                                continue;
                            }
                            //e1,e2,e3 is a valid hypertriangle, check if it has already been counted.
                            // if(e1==4 && e3==10 && e2==12){
                            //     int a = 0;
                            // }
                            // EdgeId a = e1, b = e2, c = e3;
                            // if (a > b) std::swap(a, b);
                            // if (a > c) std::swap(a, c);
                            // if (b > c) std::swap(b, c);
                            // uniqueTriplets.insert({a,b,c});
                            EdgeId mult = triple_intersections(dirH,e1,e2,e3);
                            freq[mult]++;
                        }
                    }
                }
            }
        }
    }
    VertexId count = 0;
    for(auto &kv : freq) {
        assert(kv.second % kv.first == 0);
        count += kv.second / kv.first;
    }
    std::cout << "Number of unique hypertriangles (13-20): " << count<< "\n";
    // std::cout << "Number of unique triplets: " << uniqueTriplets.size() << "\n";
}
