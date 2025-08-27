#include "count_hypertriangles.h"
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <tuple>

struct TripleIntersections {
    int i12_not3 = 0;  // |e1 ∩ e2 \ e3|
    int i23_not1 = 0;  // |e2 ∩ e3 \ e1|
    int i31_not2 = 0;  // |e3 ∩ e1 \ e2|
    int i123 = 0;      // |e1 ∩ e2 ∩ e3|
    int i1_not23 = 0;  // |e1 \ e2 \ e3|
    int i2_not13 = 0;  // |e2 \ e1 \ e3|
    int i3_not12 = 0;  // |e3 \ e1 \ e2|
};

int findType(TripleIntersections res) {
    int cgreen = (res.i1_not23 ? 1 : 0) + (res.i2_not13 ? 1 : 0) + (res.i3_not12 ? 1 : 0);
    if (res.i123 == 0) {
        switch(cgreen){
            case 0: return 17;
            case 1: return 18;
            case 2: return 19;
            case 3: return 20;
        }
    } else {
        int cblue= (res.i12_not3 ? 1 : 0) + (res.i23_not1 ? 1 : 0) + (res.i31_not2 ? 1 : 0);
        switch(cblue){
        case 3:
            switch(cgreen){
                case 0: return 13;
                case 1: return 14;
                case 2: return 15;
                case 3: return 16;
            }
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


std::pair<int,TripleIntersections> triple_intersections(const DirHypergraphCSR& H,
                                         EdgeId e1, EdgeId e2, EdgeId e3) {
    TripleIntersections res;

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

        if (in1 & in2 & in3) {                // all three
            ++res.i123;
        } else if (in1 & in2) {               // e1 ∩ e2
            ++res.i12_not3;
        } else if (in2 & in3) {               // e2 ∩ e3
            ++res.i23_not1;
        } else if (in1 & in3) {               // e1 ∩ e3
            ++res.i31_not2;
        }

        if (in1) ++p1;
        if (in2) ++p2;
        if (in3) ++p3;
    }
    res.i1_not23 = H.edge_sizes[e1] - res.i12_not3 - res.i31_not2 - res.i123;
    res.i2_not13 = H.edge_sizes[e2] - res.i12_not3 - res.i23_not1 - res.i123;
    res.i3_not12 = H.edge_sizes[e3] - res.i31_not2 - res.i23_not1 - res.i123;
    int type = findType(res);

    return {type, res};
    //return {type, res.i12_not3 * res.i23_not1 * res.i31_not2};
}

std::pair<bool,TripleIntersections> triple_intersection_source(const DirHypergraphCSR& H, EdgeId e1, EdgeId e2, EdgeId e3, VertexId v) {
    TripleIntersections res;
    bool source_intersect = false;
    bool first = true;

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

        if (in1 & in2 & in3) {                // all three
            ++res.i123;
            if(first){
                first = false;
                if(v1 == v) {
                    source_intersect = true;
                } else {
                    break;
                }
            }
        } else if (in1 & in2) {               // e1 ∩ e2
            ++res.i12_not3;
        } else if (in2 & in3) {               // e2 ∩ e3
            ++res.i23_not1;
        } else if (in1 & in3) {               // e1 ∩ e3
            ++res.i31_not2;
        }

        if (in1) ++p1;
        if (in2) ++p2;
        if (in3) ++p3;
    }
    return {source_intersect, res};
}

void count_hypertriangles(DirHypergraphCSR& dirH, EdgeId* counts){

    std::unordered_map<long long, long long> freq[20];

    //Loop for starting vertices
    for (VertexId v = 0; v < dirH.num_vertices; v++){
        //Loop for couple of neighboring hyperedges
        EdgeId ie2end = dirH.vertex_offset[v]+dirH.outdegrees[v];
        EdgeId ie1end = ie2end-1;
        for(EdgeId ie1 = dirH.vertex_offset[v]; ie1 < ie1end; ie1++) {
            for (EdgeId ie2 = ie1+1; ie2 < ie2end; ie2++) {
                EdgeId e1 = dirH.ve_hyperedges[ie1];
                EdgeId e2 = dirH.ve_hyperedges[ie2];
                //Loop for each possible outneighbor in each neighboring hyperedge
                VertexId iw1end = dirH.edge_offsets[e1] + dirH.edge_sizes[e1];
                for(VertexId iw1 = dirH.edge_offsets[e1] + dirH.vertex_hyperedges_pos[ie1]+1; iw1 < iw1end; iw1++){
                    //Check that w1 is not in e2
                    VertexId w1 = dirH.ed_vertices[iw1];
                    if (contains_vertex(dirH, e2, w1)){
                        continue;
                    }
                    VertexId iw2end = dirH.edge_offsets[e2] + dirH.edge_sizes[e2];
                    for(VertexId iw2 = dirH.edge_offsets[e2] + dirH.vertex_hyperedges_pos[ie2]+1; iw2 < iw2end; iw2++){

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
                        EdgeId ie3end = dirH.vertex_offset[x1]+dirH.outdegrees[x1];
                        for (EdgeId ie3 = dirH.vertex_offset[x1]; ie3 < ie3end; ie3++){
                            EdgeId e3 = dirH.ve_hyperedges[ie3];
                            if (e3==e1 || e3==e2){
                                continue;
                            }
                            //check that e3 does not contain v and contains w2.
                            if (!contains_vertex(dirH, e3, x2) || contains_vertex(dirH, e3, v)){
                                continue;
                            }

                            auto result = triple_intersections(dirH,e1,e2,e3);
                            int type = result.first-1;
                            int mult = result.second.i31_not2 * result.second.i23_not1 * result.second.i12_not3;
                            freq[type][mult]++;
                        }
                    }
                }
            }
        }
    }
    for (int i = 13; i <= 20; i++){
        VertexId count = 0;
        for(auto &kv : freq[i-1]) {
            count += kv.second / kv.first;
        }
        counts[i-1] = count; 
    }
}

void count_hypertriangles_dense(HypergraphCSR& H, DirHypergraphCSR& dirH, EdgeId* counts){
    std::unordered_map<long long, long long> freq[20];
    for (VertexId v = 0 ; v < H.num_vertices; v++) {
        for (EdgeId i = H.vertex_offset[v]; i < H.vertex_offset[v] + H.degrees[v]; i++){
            for (EdgeId j = i+1; j < H.vertex_offset[v] + H.degrees[v]; j++){
                for (EdgeId k = j+1; k < H.vertex_offset[v] + H.degrees[v]; k++){
                    EdgeId e1 = H.ve_hyperedges[i];
                    EdgeId e2 = H.ve_hyperedges[j];
                    EdgeId e3 = H.ve_hyperedges[k];
                    auto result = triple_intersections(dirH, e1,e2,e3);
                    int type = result.first;
                    int mult = result.second.i123;
                    freq[type-1][mult]++;
                }
            }
        }
    }
    for(int i = 1; i <= 12; i++) {
        VertexId count = 0;
        for(auto &kv : freq[i-1]) {
            count += kv.second / kv.first;
        }
        counts[i-1] = count; 
    }
}
int edge_intersection(DirHypergraphCSR& dirH, EdgeId e1, EdgeId e2, VertexId* out){
    int size = 0;
    VertexId p1 = dirH.edge_offsets[e1];
    VertexId p2 = dirH.edge_offsets[e2];
    VertexId p1end = dirH.edge_offsets[e1] + dirH.edge_sizes[e1];
    VertexId p2end = dirH.edge_offsets[e2] + dirH.edge_sizes[e2];
    while (p1 < p1end && p2 < p2end){
        VertexId v1 = dirH.ed_vertices[p1];
        VertexId v2 = dirH.ed_vertices[p2];
        if(v1 == v2){
            out[size++] = v1;
            p1++;
            p2++;
        } else if(v1 < v2){
            p1++;
        } else {
            p2++;
        }
    }
    return size;
}

std::tuple<EdgeId, EdgeId> getTotalStarCount(DirHypergraphCSR& dirH) {
    EdgeId* indegrees = new EdgeId[dirH.num_vertices]();
    for(EdgeId e =0; e < dirH.num_hyperedges; e++){
        indegrees[dirH.ed_vertices[dirH.edge_offsets[e]+dirH.edge_sizes[e]-1]]++;
    }

    EdgeId stars = 0;
    EdgeId extended_stars = 0;
    for (VertexId v=0; v < dirH.num_vertices; v++) {
        stars += (indegrees[v] > 2 ? indegrees[v] * (indegrees[v]-1) * (indegrees[v]-2) / 6: 0);
        stars += (indegrees[v] > 1 ? indegrees[v] * (indegrees[v]-1) * dirH.outdegrees[v] / 2: 0);
        stars += (dirH.outdegrees[v] > 1 ? indegrees[v] * dirH.outdegrees[v] * (dirH.outdegrees[v]-1) / 2: 0);
    }

    for (VertexId v = 0 ; v < dirH.num_vertices; v++) {
        for (EdgeId i = dirH.vertex_offset[v]; i < dirH.vertex_offset[v] + dirH.outdegrees[v]; i++){
            for (EdgeId j = i+1; j < dirH.vertex_offset[v] + dirH.outdegrees[v]; j++){
                for (EdgeId k = j+1; k < dirH.vertex_offset[v] + dirH.outdegrees[v]; k++){
                    EdgeId e1 = dirH.ve_hyperedges[i];
                    EdgeId e2 = dirH.ve_hyperedges[j];
                    EdgeId e3 = dirH.ve_hyperedges[k];

                    VertexId laste1 = dirH.ed_vertices[dirH.edge_offsets[e1] + dirH.edge_sizes[e1] - 1];
                    VertexId laste2 = dirH.ed_vertices[dirH.edge_offsets[e2]+ dirH.edge_sizes[e2] - 1];
                    VertexId laste3 = dirH.ed_vertices[dirH.edge_offsets[e3] + dirH.edge_sizes[e3] - 1];

                    //int mult = triple_intersection_source(dirH, e1,e2,e3);
                    auto result = triple_intersection_source(dirH, e1,e2,e3, v);
                    bool valid_source = result.first;
                    TripleIntersections res = result.second;
                    if(!valid_source){
                        continue;
                    }
                    bool e1contained = contains_vertex(dirH, e2, laste1) && contains_vertex(dirH, e3, laste1);
                    bool e2contained = contains_vertex(dirH, e1, laste2) && contains_vertex(dirH, e3, laste2);                    
                    bool e3contained = contains_vertex(dirH, e1, laste3) && contains_vertex(dirH, e2, laste3);

                    if(res.i12_not3 != 0 && (!e3contained)) {
                        extended_stars += 1;
                    }
                    if(res.i23_not1 != 0 && (!e1contained)) {
                        extended_stars += 1;
                    }
                    if(res.i31_not2 != 0 && (!e2contained)) {
                        extended_stars += 1;
                    }
                    if ((!e1contained) && (!e2contained) && (!e3contained)){
                        stars += 1;
                    }
                }
            }
        }
    }

    for (VertexId v = 0 ; v < dirH.num_vertices; v++) {
        for (EdgeId i = dirH.vertex_offset[v]; i < dirH.vertex_offset[v] + dirH.outdegrees[v]; i++){
            EdgeId e1 = dirH.ve_hyperedges[i];
            for (EdgeId j = i+1; j < dirH.vertex_offset[v] + dirH.outdegrees[v]; j++){
                    EdgeId e2 = dirH.ve_hyperedges[j];
                    int min = std::min(dirH.edge_sizes[e1], dirH.edge_sizes[e2]);
                    VertexId* intersection = new VertexId[min];
                    int size = edge_intersection(dirH,e1,e2, intersection);
                    //Verify it is the ``source'' of the intersection and has more than 1 vertex in the intersection
                    if(intersection[0] != v || size==1){
                        continue;
                    }
                    //Add the indegrees of all vertices
                    for (int k = 0; k < size; k++){
                        extended_stars += indegrees[intersection[k]];
                    }
                    //For the last  vertex we need to compute the number of edges ending on it that contain the intersection and substract them from the count.
                    for (EdgeId k = dirH.vertex_offset[v]; k < dirH.vertex_offset[v] + dirH.outdegrees[v]; k++){
                        EdgeId e3 = dirH.ve_hyperedges[k];
                        VertexId laste3 = dirH.ed_vertices[dirH.edge_offsets[e3] + dirH.edge_sizes[e3] - 1];
                        // Check edge ends in the the last vertex of the intersection
                        if (laste3 != intersection[size-1]) {
                            continue;
                        }
                        // Check if contains all the intersection
                        bool contains_all = true;
                        for (int l = 1; l < size - 1; l++){
                            //probably optimizable with a linear search
                            if (!contains_vertex(dirH, e3,intersection[l])){
                                contains_all = false;
                                break;
                            }
                        }
                        if (contains_all) {
                            extended_stars -= 1;
                        }
                    }

                    //Finally, If the last in the intersection is also last for e1 or e2 we need to consider the hyperedges starting from it
                    //Those hyperedges must not intersect with intersect
                    VertexId laste1 = dirH.ed_vertices[dirH.edge_offsets[e1] + dirH.edge_sizes[e1] - 1];
                    VertexId laste2 = dirH.ed_vertices[dirH.edge_offsets[e2]+ dirH.edge_sizes[e2] - 1];
                    VertexId lastint = intersection[size-1];
                    if(laste1!=lastint && laste2 != lastint) {
                        continue;
                    }
                    for (EdgeId k = dirH.vertex_offset[lastint]; k < dirH.vertex_offset[lastint] + dirH.outdegrees[lastint]; k++){
                        EdgeId e3 = dirH.ve_hyperedges[k];
                        if(e3 ==  e1 || e3==e2){
                            continue;
                        }
                        // Check if does not intersect with intersect
                        bool contains_any = false;
                        for (int l = 0; l < size - 1; l++){
                            //probably optimizable with a linear search
                            if (contains_vertex(dirH, e3,intersection[l])){
                                contains_any = true;
                                break;
                            }
                        }
                        if(!contains_any){
                            extended_stars++;
                        }
                    }
                    delete[] intersection;
            }
        }
    }
    delete[] indegrees;
    return {stars, extended_stars};
}

//Check if e1 contains e2
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

bool isCommonSource(DirHypergraphCSR& dirH,VertexId v,EdgeId e1, EdgeId e2) {
    VertexId p1 = dirH.edge_offsets[e1];
    VertexId p2 = dirH.edge_offsets[e2];
    VertexId p1end = dirH.edge_offsets[e1] + dirH.edge_sizes[e1];
    VertexId p2end = dirH.edge_offsets[e2] + dirH.edge_sizes[e2];
    while(p1<p1end && p2<p2end){
        VertexId v1 = dirH.ed_vertices[p1];
        VertexId v2 = dirH.ed_vertices[p2];
        if(v1==v2){
            if(v1 == v) return true;
            else return false;
        }
        if(v1 < v2) p1++;
        else p2++;
    }
    return false;
}

void count_contained_triangles(DirHypergraphCSR& dirH, EdgeId* counts){



    //First we reorder the edges in buckets depending on their size
    // int max_size = 0;
    // for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
    //     if(dirH.edge_sizes[e]>max_size) max_size = dirH.edge_sizes[e];
    // }
    // int* bucket_sizes = new int[max_size+1]();
    // for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
    //     bucket_sizes[dirH.edge_sizes[e]]++;
    // }
    // int* bucket_offsets = new int[max_size+1]();
    // int* bucket_counter = new int[max_size+1]();
    // EdgeId* edges = new EdgeId[dirH.num_hyperedges];

    // int total_offset = 0;
    // for (int i = 0; i <= max_size; i++){
    //     bucket_offsets[i] = total_offset;
    //     total_offset += bucket_sizes[i];
    // }

    // for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
    //     int size = dirH.edge_sizes[e];
    //     edges[bucket_offsets[size]+bucket_counter[size]] = e;
    //     bucket_counter[size]++;
    // }


    EdgeId* singletonmap = new EdgeId[dirH.num_vertices]();
    bool* singletonmapped = new bool[dirH.num_vertices]();

    //Now we build the vectors to store parents and children
    std::vector<std::vector<EdgeId>> children(dirH.num_hyperedges);
    std::vector<std::vector<EdgeId>> parents(dirH.num_hyperedges);

    for(EdgeId e1 = 0; e1 < dirH.num_hyperedges; e1++){
        int size1 = dirH.edge_sizes[e1];
        VertexId v = dirH.ed_vertices[dirH.edge_offsets[e1]];
        if(size1==1){
            singletonmap[v] = e1;
            singletonmapped[v] = true;
        }
        for(EdgeId ie2 = dirH.vertex_offset[v]; ie2 < dirH.vertex_offset[v] + dirH.outdegrees[v]; ie2++){
            EdgeId e2 = dirH.ve_hyperedges[ie2];
            int size2 = dirH.edge_sizes[e2];
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

    // for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
    //     int size = dirH.edge_sizes[e];
    //     for(EdgeId i = 0; i < bucket_offsets[size]; i++){
    //         if(edge_contains_edge(dirH,e,edges[i])){
    //             children[e].push_back(edges[i]);
    //             parents[edges[i]].push_back(e);
    //         }
    //     }
    // }

    //Type 1
    for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
        counts[0] += children[e].size() * parents[e].size();
    }

    //Types 2-5
    for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
        for(int i = 0; i < parents[e].size(); i++) {
            for(int j= i+1; j < parents[e].size(); j++){
                EdgeId e2 = parents[e][i];
                EdgeId e3 = parents[e][j];
                auto res = triple_intersections(dirH,e,e2,e3);
                int type = res.first;
                if (type>1){
                    counts[type-1]++;
                }
            }
        }
        for(int i = 0; i < children[e].size(); i++) {
            for(int j= i+1; j < children[e].size(); j++){
                EdgeId e2 = children[e][i];
                EdgeId e3 = children[e][j];
                auto res = triple_intersections(dirH,e,e2,e3);
                int type = res.first;
                if (type>1){
                    counts[type-1]++;
                }
            }
        }
    }
    //types 6-8
    
    for (EdgeId e1 = 0; e1 < dirH.num_hyperedges; e1++){
        if(dirH.edge_sizes[e1]== 1){
            continue;
        }


        VertexId outdegree_sum = 0; 
        for(VertexId iv1 = dirH.edge_offsets[e1]; iv1 < dirH.edge_offsets[e1] +  dirH.edge_sizes[e1]; iv1++){
            VertexId v1 = dirH.ed_vertices[iv1];
            outdegree_sum += indegrees[v1];
        }


        for(int ie2 = 0; ie2 < parents[e1].size(); ie2++) {
            VertexId redundant = 0;
            EdgeId e2 = parents[e1][ie2];
            VertexId e2end = dirH.ed_vertices[dirH.edge_offsets[e2]+ dirH.edge_sizes[e2] - 1];

            for(VertexId iv1 = dirH.edge_offsets[e2]; iv1 < dirH.edge_offsets[e2] +  dirH.edge_sizes[e2]; iv1++){
                VertexId v1 = dirH.ed_vertices[iv1];

                for(EdgeId ie3= dirH.vertex_offset[v1]; ie3 < dirH.vertex_offset[v1] + dirH.outdegrees[v1]; ie3++) {
                    EdgeId e3 = dirH.ve_hyperedges[ie3];
                    if(e3 == e1 || e3==e2) {
                        continue;
                    }

                    //Check that v1 is the first vertex in the intersection of e2 and e3
                    //This ensures e3 is only counted once with e1 and e2
                    if(!isCommonSource(dirH,v1,e2,e3)){
                        continue;
                    }
                    auto res = triple_intersections(dirH,e1,e2,e3);
                    int type = res.first;
                    if(type>5) {
                        counts[type-1]++;
                    }
                    VertexId e3end = dirH.ed_vertices[dirH.edge_offsets[e3]+ dirH.edge_sizes[e3] - 1];
                    if (contains_vertex(dirH, e1, e3end)) {
                        redundant += 1;
                    }
                }
            } 

             if (contains_vertex(dirH, e1, e2end)) {
                redundant += 1;
            }
            counts[5] += outdegree_sum - redundant - 1;
        }
    }


    // delete[] bucket_sizes;
    // delete[] bucket_offsets;
    // delete[] bucket_counter;
    // delete[] edges;
    delete[] indegrees;
}

std::tuple<bool, int,TripleIntersections> triple_intersections_check(const DirHypergraphCSR& H,
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

        if (in1 & in2 & in3) {                // all three
            ++res.i123;
            if(first_v123){
                first_v123 = false;
                if(which_edge == 1 && minv!=v23) return {false,type, res};
                if(which_edge == 2 && minv!=v13) return {false,type, res};
                if(which_edge == 3 && minv!=v12) return {false,type, res};
            }
        } else if (in1 & in2) {               // e1 ∩ e2
            ++res.i12_not3;
            if(first_v12){
                first_v12 = false;
                if(which_edge != 3 && minv != v12) return {false,type, res};
            }
        } else if (in2 & in3) {               // e2 ∩ e3
            ++res.i23_not1;
            if(first_v23){
                first_v23 = false;
                if(which_edge != 1 && minv != v23) return {false,type, res};
            }
        } else if (in1 & in3) {               // e1 ∩ e3
            ++res.i31_not2;
            if(first_v13){
                first_v13 = false;
                if(which_edge != 2 && minv != v13) return {false,type, res};
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

    return {true, type, res};
}



void count_hypertriangles_flexible(DirHypergraphCSR& dirH, EdgeId* counts){

    std::unordered_map<long long, long long> freq[20];
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

                    if (contains_vertex(dirH, e2, w1)){
                        e2_extends = true;
                    } else {
                        e2_extends = false;
                    }

                    VertexId iw2end = dirH.edge_offsets[e2] + dirH.edge_sizes[e2];
                    for(VertexId iw2 = dirH.edge_offsets[e2] + dirH.vertex_hyperedges_pos[ie2]+1; iw2 < iw2end; iw2++){

                        VertexId w2 = dirH.ed_vertices[iw2];
                        //check w2 is not w1
                        if(w1 == w2){
                            continue;
                        }

                        //Check that w2 is not in e1
                        if (contains_vertex(dirH, e1, w2)){
                            if (e2_extends){
                                continue;
                            }
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
                            if (!contains_vertex(dirH, e3, x2)){
                                continue;
                            }


                            if(contains_vertex(dirH, e3, v)){
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

                            std::tuple<bool,int,TripleIntersections> out = triple_intersections_check(dirH,e1,e2,e3,v,w1,w2,which_edge);
                            bool valid = std::get<0>(out);
                            int type = std::get<1>(out);
                            // TripleIntersections res = std::get<2>(out);
                            if(!valid){
                                continue;
                            }
                            int mult=0;
                            if(tot==3) {
                                mult = 1;
                            } else if(tot==2){
                                if(type != 11 && type != 12) continue;
                                mult = 1;
                            }
                            freq[type-1][mult]++;
                        }
                    }
                }
            }
        }
    }
    for (int i = 10; i <= 20; i++){
        VertexId count = 0;
        for(auto &kv : freq[i-1]) {
            count += kv.second / kv.first;
        }
        counts[i-1] = count; 
    }
}


void compute_final_counts(std::tuple<EdgeId, EdgeId> stars,  EdgeId* counts) {
    EdgeId extended_stars_accounted = counts[0] + counts[2] + counts[3]*2 + counts[4]*2 + counts[5] + counts[6]*2 + counts[7]*2
        + counts[9] + counts[10]*2+counts[11]*2 + counts[12]*3 + counts[13]*3 + counts[14] *3 + counts[15]*3;
    counts[9] = std::get<1>(stars) - extended_stars_accounted;
    EdgeId stars_accounted = 0;
    for(int i = 0; i < 16; i++){
        stars_accounted += counts[i];
    }
    counts[8] = std::get<0>(stars) - stars_accounted;
}