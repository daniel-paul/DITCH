#include "count_patterns.h"
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <tuple>
#include <limits>


// This function takes as input the intersection regions of three hyperedges and returns its type.
// Returns 0 if the pattern is not valid
int findType(TripleIntersections res) {
    int cgreen = (res.i1_not23 ? 1 : 0) + (res.i2_not13 ? 1 : 0) + (res.i3_not12 ? 1 : 0);
    int cblue= (res.i12_not3 ? 1 : 0) + (res.i23_not1 ? 1 : 0) + (res.i31_not2 ? 1 : 0);
    //No Red
    if (res.i123 == 0) {
        //3 Blue
        if(cblue==3){
            switch(cgreen){
                case 0: return 17;
                case 1: return 18;
                case 2: return 19;
                case 3: return 20;
            }
        }
        //Open patterns
        else {
            switch(cgreen){
                case 0: return 21;
                case 1: if((res.i1_not23 && res.i12_not3 && res.i31_not2) || (res.i2_not13 && res.i12_not3 && res.i23_not1) 
                                                                            ||(res.i3_not12 && res.i31_not2 && res.i31_not2))
                            return 22;
                        return 23;
                case 2: if((!res.i1_not23 && res.i12_not3 && res.i31_not2) || (!res.i2_not13 && res.i12_not3 && res.i23_not1) 
                                                                            ||(!res.i3_not12 && res.i31_not2 && res.i31_not2))
                            return 25;
                        return 24;
                case 3: return 26;
            }
        }
    } 
    //1 Red
    else {
        switch(cblue){
        //3 Blue
        case 3:
            switch(cgreen){
                case 0: return 13;
                case 1: return 14;
                case 2: return 15;
                case 3: return 16;
            }
        //2 Blue
        case 2:
            switch(cgreen){
                case 0: return 4;
                case 1:
                    if ((res.i1_not23 && res.i12_not3 && res.i31_not2) || (res.i2_not13 && res.i12_not3 && res.i23_not1) 
                                                                        || (res.i3_not12 && res.i31_not2 && res.i23_not1))
                        return 5;
                    return 7;
                case 2:
                    if ((!res.i1_not23 && !res.i23_not1) || (!res.i2_not13 && !res.i31_not2) || (!res.i3_not12 && !res.i12_not3))
                        return 11;
                    return 8;
                case 3: return 12;
            }
        //1 Blue
        case 1:
            switch(cgreen){
                case 0: return 0;
                case 1: return 1;
                case 2: 
                    if ((!res.i1_not23 && res.i23_not1) || (!res.i2_not13 && res.i31_not2) || (!res.i3_not12 && res.i12_not3))
                        return 3;
                    return 6;
                case 3: return 10;
            }
        //0 Blue
        case 0:
            switch(cgreen){
                case(0): return 0;
                case(1): return 0;
                case(2): return 2;
                case(3): return 9;
            }
        }
    }
    return 0;
}

//Computes the intersections of a triplet of edges e1,e2,e3 and returns the pattern type
int triple_intersections(const DirHypergraphCSR& dirH,
                                         EdgeId e1, EdgeId e2, EdgeId e3) {
    TripleIntersections res;

    auto p1 = dirH.edge_offsets[e1], p1_end = p1 + dirH.edge_sizes[e1];
    auto p2 = dirH.edge_offsets[e2], p2_end = p2 + dirH.edge_sizes[e2];
    auto p3 = dirH.edge_offsets[e3], p3_end = p3 + dirH.edge_sizes[e3];
    constexpr VertexId INF = std::numeric_limits<VertexId>::max();

    // Perform a linear search over the vertices of e1,e2,e3
    while (p1 < p1_end || p2 < p2_end || p3 < p3_end) {
        VertexId v1 = (p1 < p1_end) ? dirH.ed_vertices[p1] : INF;
        VertexId v2 = (p2 < p2_end) ? dirH.ed_vertices[p2] : INF;
        VertexId v3 = (p3 < p3_end) ? dirH.ed_vertices[p3] : INF;

        //Find the min of v1,v2,v3
        VertexId minv = v1;
        if (v2 < minv) minv = v2;
        if (v3 < minv) minv = v3;

        bool in1 = (v1 == minv);
        bool in2 = (v2 == minv);
        bool in3 = (v3 == minv);

        if (in1 && in2 && in3) {  // e1 ∩ e2 ∩ e3
            ++res.i123;
        } else if (in1 && in2) {  // e1 ∩ e2
            ++res.i12_not3;
        } else if (in2 && in3) {  // e2 ∩ e3
            ++res.i23_not1;
        } else if (in1 && in3) {  // e1 ∩ e3
            ++res.i31_not2;
        }

        //Increment the lowest
        if (in1) ++p1;
        if (in2) ++p2;
        if (in3) ++p3;
    }
    //Compute remaining regions
    res.i1_not23 = dirH.edge_sizes[e1] - res.i12_not3 - res.i31_not2 - res.i123;
    res.i2_not13 = dirH.edge_sizes[e2] - res.i12_not3 - res.i23_not1 - res.i123;
    res.i3_not12 = dirH.edge_sizes[e3] - res.i31_not2 - res.i23_not1 - res.i123;
    //Call findType to get the type
    int type = findType(res);
    return type;
}


//Check if e1 contains all e2
bool edge_contains_edge(DirHypergraphCSR& dirH, EdgeId e1, EdgeId e2){
    VertexId p1 = dirH.edge_offsets[e1];
    VertexId p2 = dirH.edge_offsets[e2];
    VertexId p1end = dirH.edge_offsets[e1]+dirH.edge_sizes[e1];
    VertexId p2end = dirH.edge_offsets[e2]+dirH.edge_sizes[e2];

    while(p1 < p1end && p2 < p2end) {
        VertexId v1 = dirH.ed_vertices[p1];
        VertexId v2 = dirH.ed_vertices[p2];
        if(v1==v2){
            p1++;
            p2++;
        }
        else if(v1 > v2) return false;
        else p1++;
    }
    if (p2!= p2end) return false;
    return true;
}

std::tuple<bool, int> triple_intersections_check(const DirHypergraphCSR& H,
                                        EdgeId e1, EdgeId e2, EdgeId e3,
                                        VertexId v12, VertexId v13, VertexId v23,
                                        int which_edge) {
    TripleIntersections res;
    int type;
    bool first_v12 = true;
    bool first_v13 = true;
    bool first_v23 = true;
    bool first_v123 = true;

    auto p1 = H.edge_offsets[e1], p1_end = p1 + H.edge_sizes[e1];
    auto p2 = H.edge_offsets[e2], p2_end = p2 + H.edge_sizes[e2];
    auto p3 = H.edge_offsets[e3], p3_end = p3 + H.edge_sizes[e3];
    constexpr VertexId INF = std::numeric_limits<VertexId>::max();

    while (p1 < p1_end || p2 < p2_end || p3 < p3_end) {
        VertexId v1 = (p1 < p1_end) ? H.ed_vertices[p1] : INF;
        VertexId v2 = (p2 < p2_end) ? H.ed_vertices[p2] : INF;
        VertexId v3 = (p3 < p3_end) ? H.ed_vertices[p3] : INF;

        VertexId minv = v1;
        if (v2 < minv) minv = v2;
        if (v3 < minv) minv = v3;

        bool in1 = (v1 == minv);
        bool in2 = (v2 == minv);
        bool in3 = (v3 == minv);

        if (in1 && in2 && in3) {                // all three
            ++res.i123;
            if(first_v123){
                first_v123 = false;
                if(which_edge == 1 && minv!=v23) return {false,type};
                if(which_edge == 2 && minv!=v13) return {false,type};
                if(which_edge == 3 && minv!=v12) return {false,type};
            }
        } else if (in1 && in2) {               // e1 ∩ e2
            ++res.i12_not3;
            if(first_v12){
                first_v12 = false;
                if(which_edge == 3 || minv != v12) return {false,type};
            }
        } else if (in2 && in3) {               // e2 ∩ e3
            ++res.i23_not1;
            if(first_v23){
                first_v23 = false;
                if(which_edge == 1 || minv != v23) return {false,type};
            }
        } else if (in1 && in3) {               // e1 ∩ e3
            ++res.i31_not2;
            if(first_v13){
                first_v13 = false;
                if (which_edge == 2 || minv != v13) return {false,type};
            }
        }

        if (in1) ++p1;
        if (in2) ++p2;
        if (in3) ++p3;
    }
    res.i1_not23 = H.edge_sizes[e1] - res.i12_not3 - res.i31_not2 - res.i123;
    res.i2_not13 = H.edge_sizes[e2] - res.i12_not3 - res.i23_not1 - res.i123;
    res.i3_not12 = H.edge_sizes[e3] - res.i31_not2 - res.i23_not1 - res.i123;
    type = findType(res);

    return {true, type};
}



void count_triangle_based_patterns(DirHypergraphCSR& dirH, EdgeId* counts){

    bool e1_extends, e2_extends, e3_extends;
    //Loop for starting vertices
    for (VertexId v = 0; v < dirH.num_vertices; v++){
        //Loop for couple of neighboring hyperedges
        EdgeId ie2end = dirH.vertex_offset[v]+dirH.outdegrees[v];
        EdgeId ie1end = ie2end-1;
        for(EdgeId ie1 = dirH.vertex_offset[v]; ie1 < ie1end; ie1++) {
            EdgeId e1 = dirH.ve_hyperedges[ie1];
            for (EdgeId ie2 = ie1+1; ie2 < ie2end; ie2++) {
                EdgeId e2 = dirH.ve_hyperedges[ie2];

                //Loop for each possible outneighbor in each neighboring hyperedge
                VertexId iw1end = dirH.edge_offsets[e1] + dirH.edge_sizes[e1];
                for(VertexId iw1 = dirH.edge_offsets[e1] + dirH.vertex_hyperedges_pos[ie1]+1; iw1 < iw1end; iw1++){
                    VertexId w1 = dirH.ed_vertices[iw1];

                    if (dirH.contains_vertex(e2, w1)){
                        e2_extends = true;
                    } else {
                        e2_extends = false;
                    }

                    VertexId iw2end = dirH.edge_offsets[e2] + dirH.edge_sizes[e2];
                    for(VertexId iw2 = dirH.edge_offsets[e2] + dirH.vertex_hyperedges_pos[ie2]+1; iw2 < iw2end; iw2++){

                        VertexId w2 = dirH.ed_vertices[iw2];
                        //check w2 is not w1
                        if(w1 == w2) continue;

                        //Check that w2 is not in e1
                        if (dirH.contains_vertex(e1, w2)){
                            if (e2_extends) continue;
                            e1_extends = true;
                        } else {
                            e1_extends = false;
                        }

                        //let x1 be the lesser of w1,w2
                        VertexId x1 = w1, x2= w2;
                        if(w1 > w2) {
                            x1 = w2;
                            x2 = w1;
                        }
                        
                        //Final loop through outneighbors of w1
                        EdgeId ie3end = dirH.vertex_offset[x1]+dirH.outdegrees[x1];
                        for (EdgeId ie3 = dirH.vertex_offset[x1]; ie3 < ie3end; ie3++){
                            EdgeId e3 = dirH.ve_hyperedges[ie3];
                            if (e3==e1 || e3==e2){
                                continue;
                            }

                            //check that e3 contains x2.
                            if (!dirH.contains_vertex(e3, x2)) continue;

                            if(dirH.contains_vertex(e3, v)){
                                e3_extends = true;
                            } else {
                                e3_extends = false;
                            }

                            int tot = (e1_extends? 0 : 1) + (e2_extends? 0 : 1) + (e3_extends? 0 : 1);
                            if (tot < 2) continue;
                            int which_edge = 0;
                            if (tot==2) {
                                if(e1_extends) which_edge = 1;
                                else if (e2_extends) which_edge = 2;
                                else which_edge = 3;
                            }

                            std::tuple<bool,int> out = triple_intersections_check(dirH,e1,e2,e3,v,w1,w2,which_edge);
                            bool valid = std::get<0>(out);
                            int type = std::get<1>(out);

                            if(!valid) continue;
                            counts[type-1] += 1;
                        }
                    }
                }
            }
        }
    }
}


void count_closed_contained_patterns(DirHypergraphCSR& dirH, EdgeId* counts){

    EdgeId* singletonmap = new EdgeId[dirH.num_vertices]();
    bool* singletonmapped = new bool[dirH.num_vertices]();

    //Now we build the vectors to store parents and children
    std::vector<std::vector<EdgeId>> children(dirH.num_hyperedges);
    std::vector<std::vector<EdgeId>> parents(dirH.num_hyperedges);

    for(EdgeId e1 = 0; e1 < dirH.num_hyperedges; e1++){
        VertexId size1 = dirH.edge_sizes[e1];
        VertexId v = dirH.ed_vertices[dirH.edge_offsets[e1]];
        if(size1==1){
            singletonmap[v] = e1;
            singletonmapped[v] = true;
        }
        for(EdgeId ie2 = dirH.vertex_offset[v]; ie2 < dirH.vertex_offset[v] + dirH.outdegrees[v]; ie2++){
            EdgeId e2 = dirH.ve_hyperedges[ie2];
            VertexId size2 = dirH.edge_sizes[e2];
            if(size2 <= size1){
                continue;
            }
            if(edge_contains_edge(dirH,e2,e1)){
                children[e2].push_back(e1);
                parents[e1].push_back(e2);
            }
        }
    }

    EdgeId* indegrees = new EdgeId[dirH.num_vertices]();
    for(EdgeId e =0; e < dirH.num_hyperedges; e++){
        if(dirH.edge_sizes[e]==1){
            continue;
        }
        VertexId laste = dirH.ed_vertices[dirH.edge_offsets[e]+dirH.edge_sizes[e]-1];
        indegrees[laste]++;
        if(singletonmapped[laste]){
            children[e].push_back(singletonmap[laste]);
            parents[singletonmap[laste]].push_back(e);
        }
    }

    for(EdgeId e = 0; e < dirH.num_hyperedges; e++){
        dirH.n_parents[e] = parents[e].size();
        dirH.n_children[e] = children[e].size();
    }


    //Type 1
    for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
        counts[0] += dirH.n_children[e] * dirH.n_parents[e];
    }

    //Type 2-3
    for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
        for(int i = 0; i < parents[e].size(); i++) {
            for(int j= i+1; j < parents[e].size(); j++){
                EdgeId e2 = parents[e][i];
                EdgeId e3 = parents[e][j];
                int type = triple_intersections(dirH,e,e2,e3);
                if (type>1){
                    counts[type-1]++;
                }
            }
        }
    }

    //type 6
    EdgeId total_count = 0;
    for (EdgeId e1 = 0; e1 < dirH.num_hyperedges; e1++){
        total_count += (dirH.edge_intersections[e1]-1)*dirH.n_parents[e1];
    }
    counts[5] = total_count - counts[6] - counts[7] - 2*(counts[1] + counts[2] + counts[3]+counts[4]) - 3* (counts[0]);
}