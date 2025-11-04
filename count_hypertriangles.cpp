#include "count_hypertriangles.h"
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <tuple>
#include <limits>

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
    int cblue= (res.i12_not3 ? 1 : 0) + (res.i23_not1 ? 1 : 0) + (res.i31_not2 ? 1 : 0);
    if (res.i123 == 0) {
        if(cblue==3){
            switch(cgreen){
                case 0: return 17;
                case 1: return 18;
                case 2: return 19;
                case 3: return 20;
            }
        } else {
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
    } else {
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

//Computes the intersections of a triplet of edges e1,e2,e3 and returns the pattern type
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

        if (in1 && in2 && in3) {                // all three
            ++res.i123;
        } else if (in1 && in2) {               // e1 ∩ e2
            ++res.i12_not3;
        } else if (in2 && in3) {               // e2 ∩ e3
            ++res.i23_not1;
        } else if (in1 && in3) {               // e1 ∩ e3
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
                    if (dirH.contains_vertex(e2, w1)){
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
                        if (dirH.contains_vertex(e1, w2)){
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
                            //check that e3 does not contain v and contains w2
                            if (!dirH.contains_vertex(e3, x2) || dirH.contains_vertex(e3, v)){
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

struct Containment {
    bool source_intersect;
    bool e1contained;
    bool e2contained;
    bool e3contained;
};

std::pair<Containment,TripleIntersections> triple_intersection_source(const DirHypergraphCSR& H, EdgeId e1, EdgeId e2, EdgeId e3, VertexId v) {
    Containment c;
    c.e1contained = false;
    c.e2contained = false;
    c.e3contained = false;
    TripleIntersections res;
    c.source_intersect = false;
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

        if (in1 && in2 && in3) {                // all three
            ++res.i123;
            if(first){
                first = false;
                if(v1 == v) {
                    c.source_intersect = true;
                } else {
                    break;
                }
            }
            if(p1 == p1_end -1) c.e1contained = true;
            if(p2 == p2_end -1) c.e2contained = true;
            if(p3 == p3_end -1) c.e3contained = true;
        } else if (in1 && in2) {               // e1 ∩ e2
            ++res.i12_not3;
        } else if (in2 && in3) {               // e2 ∩ e3
            ++res.i23_not1;
        } else if (in1 && in3) {               // e1 ∩ e3
            ++res.i31_not2;
        }

        if (in1) ++p1;
        if (in2) ++p2;
        if (in3) ++p3;
    }
    return {c, res};
}

std::tuple<EdgeId, EdgeId> getTotalStarCount(DirHypergraphCSR& dirH) {
    //Compute the indegrees of each vertex
    EdgeId* indegrees = new EdgeId[dirH.num_vertices]();
    for(EdgeId e =0; e < dirH.num_hyperedges; e++){
        indegrees[dirH.ed_vertices[dirH.edge_offsets[e]+dirH.edge_sizes[e]-1]]++;
    }

    EdgeId stars = 0;
    EdgeId extended_stars = 0;

    //Compute stars that contain an endpoint
    for (VertexId v=0; v < dirH.num_vertices; v++) {
        stars += (indegrees[v] > 2 ? indegrees[v] * (indegrees[v]-1) * (indegrees[v]-2) / 6: 0);
        stars += (indegrees[v] > 1 ? indegrees[v] * (indegrees[v]-1) * dirH.outdegrees[v] / 2: 0);
        stars += (dirH.outdegrees[v] > 1 ? indegrees[v] * dirH.outdegrees[v] * (dirH.outdegrees[v]-1) / 2: 0);
    }

    //Compute extended-stars that contain an endpoint
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
                //Add the indegrees of all vertices in the intersection
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
                        if (!dirH.contains_vertex(e3,intersection[l])){
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

                    auto pint = 0, pint_end = size - 1;
                    auto p3 = dirH.edge_offsets[e3], p3_end = p3 + dirH.edge_sizes[e3];
                    while (pint < pint_end && p3 < p3_end){
                        VertexId v3 = dirH.ed_vertices[p3];
                        VertexId vint = intersection[pint];
                        if (v3 == vint){
                            contains_any = true;
                            break;
                        }
                        if(v3 < vint){
                            p3++;
                        } else {
                            pint++;
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

    //Count the stars and extended stars that do not contain an endpoint
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

                    //Check that v is the first common vertex in e1,e2,e3
                    auto result = triple_intersection_source(dirH, e1,e2,e3, v);
                    Containment c = result.first;
                    TripleIntersections res = result.second;
                    if(!c.source_intersect){
                        continue;
                    }

                    if(res.i12_not3 != 0 && (!c.e3contained)) {
                        extended_stars += 1;
                    }
                    if(res.i23_not1 != 0 && (!c.e1contained)) {
                        extended_stars += 1;
                    }
                    if(res.i31_not2 != 0 && (!c.e2contained)) {
                        extended_stars += 1;
                    }
                    //check that the last vertex of each edge is not in the common zone.
                    if ((!c.e1contained) && (!c.e2contained) && (!c.e3contained)){
                        stars += 1;
                    }
                }
            }
        }
    }

    delete[] indegrees;
    return {stars, extended_stars};
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


void count_contained_triangles(DirHypergraphCSR& dirH, EdgeId* counts){

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

    for(EdgeId e = 0; e < dirH.num_hyperedges; e++){
        dirH.n_parents[e] = parents[e].size();
        dirH.n_children[e] = children[e].size();
    }


    //Type 1
    for (EdgeId e = 0; e < dirH.num_hyperedges; e++){
        counts[0] += children[e].size() * parents[e].size();
    }

    //Type 2-3
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
    }

    //type 6
    int total_count = 0;
    for (EdgeId e1 = 0; e1 < dirH.num_hyperedges; e1++){
        total_count += (dirH.edge_intersections[e1]-1)*dirH.n_parents[e1];
    }
    counts[5] = total_count - counts[6] - counts[7] - 2*(counts[1] + counts[2] + counts[3]+counts[4]) - 3* (counts[0]);
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



void count_hypertriangles_flexible(DirHypergraphCSR& dirH, EdgeId* counts){

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

//Checks if e1 is completely covered by e2 and e3, but no vertex belongs to the intersection of e1,e2,e3
//Returns -1 if false, returns 0,1,2 to indicate how many of e2,e3 are completely covered by e1.
int check_is_complement(DirHypergraphCSR& dirH, EdgeId e1, EdgeId e2, EdgeId e3, VertexId vertex1, VertexId vertex2){
    bool first_v12 = true;
    bool first_v13 = true;
    bool e2_contained = true;
    bool e3_contained = true;

    auto p1 = dirH.edge_offsets[e1], p1_end = p1 + dirH.edge_sizes[e1];
    auto p2 = dirH.edge_offsets[e2], p2_end = p2 + dirH.edge_sizes[e2];
    auto p3 = dirH.edge_offsets[e3], p3_end = p3 + dirH.edge_sizes[e3];
    constexpr VertexId INF = std::numeric_limits<VertexId>::max();


    while (p1 < p1_end) {
        VertexId v1 = (p1 < p1_end) ? dirH.ed_vertices[p1] : INF;
        VertexId v2 = (p2 < p2_end) ? dirH.ed_vertices[p2] : INF;
        VertexId v3 = (p3 < p3_end) ? dirH.ed_vertices[p3] : INF;

        VertexId minv = v1;
        if (v2 < minv) minv = v2;
        if (v3 < minv) minv = v3;

        bool in1 = (v1 == minv);
        bool in2 = (v2 == minv);
        bool in3 = (v3 == minv);

        if (in1 && in2 && in3) {                // all three
            return -1;
        }
        if (in1 && !in2 && !in3) {              //only e1
            return -1;
        }
        if(first_v12 && in1 && in2) {
            if (v1 != vertex1) 
                return -1;
            first_v12 = false;
        }
        else if(first_v13 && in1 && in3) {
            if (v1 != vertex2) 
                return -1;
            first_v13 = false;
        }
        if(e2_contained && !in1 && in2){
            e2_contained = false;
        }
        if(e3_contained && !in1 && in3){
            e3_contained = false;
        }

        if (in1) ++p1;
        if (in2) ++p2;
        if (in3) ++p3;
    }
    int res = 0;
    if(e2_contained && p2 == p2_end) res++;
    if(e3_contained && p3 == p3_end) res++;
    return res;
}

//We want to check if e2 contains all the vertices of e1, except exactly one, which will be stored in missing_vertex
//Simultaneously we make sure that vertex1 is the first source in the intersection of e1 and e2
bool check_one_off(DirHypergraphCSR& dirH, EdgeId e1, EdgeId e2, VertexId vertex1, VertexId& missing_vertex){
    auto p1 = dirH.edge_offsets[e1], p1_end = p1 + dirH.edge_sizes[e1];
    auto p2 = dirH.edge_offsets[e2], p2_end = p2 + dirH.edge_sizes[e2];
    bool one_off = false;
    bool first = true;
    while (p1 < p1_end && p2 < p2_end) {
        VertexId v1 = dirH.ed_vertices[p1];
        VertexId v2 = dirH.ed_vertices[p2];

        VertexId minv = v1;
        if (v2 < minv) minv = v2;

        bool in1 = (v1 == minv);
        bool in2 = (v2 == minv);

        //We hit this block when we find the first vertex in the intersection of e1 and e2
        if(first && in1 && in2) {
            //If v1 is not that vertex we return false
            if (v1 != vertex1) return false;
            first = false;
        }
        //If we find a vertex in e1 that is not in e2
        else if (in1 && !in2) {
            //If we already had find one, we return false
            if(one_off) return false;

            //Else, it is the first one we find
            one_off = true;
            missing_vertex = p1-dirH.edge_offsets[e1];
        }
        if (in1) ++p1;
        if (in2) ++p2;
    }
    //If we are in the last position of p1 and we still did not find the last
    if(p1 == p1_end-1 && !one_off) {
        missing_vertex = p1-dirH.edge_offsets[e1];
        return true;
    }
    //Else, if we have not find a vertex missing or we have not explore the entire e1 we return false
    if (!one_off || p1 < p1_end) return false;
    //Else we return true
    return true;
}

void compute_non_hypertriangles(DirHypergraphCSR& dirH, EdgeId* counts) {

    VertexId count_contained[3] = {0, 0, 0};
    bool* singletonmapped = new bool[dirH.num_vertices]();
    EdgeId* indegrees = new EdgeId[dirH.num_vertices]();

    for(EdgeId e = 0; e < dirH.num_hyperedges; e++){
        VertexId v = dirH.ed_vertices[dirH.edge_offsets[e] + dirH.edge_sizes[e]-1];
        if(dirH.edge_sizes[e]==1){
            singletonmapped[v] = true;
        } else {
            indegrees[v]++;
        }
    }

    //For every hyperedge e1
    for (EdgeId e1 = 0; e1 < dirH.num_hyperedges; e1++){
        VertexId e1end = dirH.edge_offsets[e1] + dirH.edge_sizes[e1];
        for (VertexId iv1 = dirH.edge_offsets[e1]; iv1 <  e1end; iv1++){
            for (VertexId iv2 = iv1+1; iv2 <  e1end; iv2++){
                VertexId v1 = dirH.ed_vertices[iv1];
                VertexId v2 = dirH.ed_vertices[iv2];
                EdgeId v1end = dirH.vertex_offset[v1] + dirH.outdegrees[v1];
                EdgeId v2end = dirH.vertex_offset[v2] + dirH.outdegrees[v2]; 
                for(EdgeId ie2 = dirH.vertex_offset[v1]; ie2 < v1end; ie2++){
                    EdgeId e2 = dirH.ve_hyperedges[ie2];
                    if (e2 == e1 || dirH.contains_vertex(e2,v2)) continue;
                    for(EdgeId ie3 = dirH.vertex_offset[v2]; ie3 < v2end; ie3++){
                        EdgeId e3 = dirH.ve_hyperedges[ie3];
                        if (e3 == e1 || e3 == e2) continue;
                        //if (contains_vertex(dirH,e3,v1)) continue;
                        int res = check_is_complement(dirH,e1,e2,e3,v1,v2);
                        if(res==-1) continue;
                        count_contained[res]++;
                    }
                }
            }
        }
    }
    
    for (EdgeId e1 = 0; e1 < dirH.num_hyperedges; e1++){
        if (dirH.edge_sizes[e1] < 2) continue;
        //std::fill(internal_indegrees, internal_indegrees + rank, 0);
        VertexId* internal_indegrees = new VertexId[dirH.edge_sizes[e1]]();

        VertexId e1end = dirH.edge_offsets[e1] + dirH.edge_sizes[e1];

        for (VertexId iv1 = dirH.edge_offsets[e1]; iv1 <  e1end-1; iv1++){
            VertexId v1 = dirH.ed_vertices[iv1];
            EdgeId v1end = dirH.vertex_offset[v1] + dirH.outdegrees[v1];
            for(EdgeId ie2 = dirH.vertex_offset[v1]; ie2 < v1end; ie2++){
                EdgeId e2 = dirH.ve_hyperedges[ie2];
                if (dirH.isCommonSource(v1,e1,e2)){
                    VertexId laste2 = dirH.ed_vertices[dirH.edge_offsets[e2]+dirH.edge_sizes[e2]-1];
                    VertexId ilaste2 = contains_vertex_return_pos(dirH,e1,laste2);
                    if(ilaste2 == -1) continue;
                    internal_indegrees[ilaste2]++;
                }
            }
        }

        if(dirH.edge_sizes[e1] == 2){
            VertexId iv1 = dirH.edge_offsets[e1];
            VertexId iv2 = dirH.edge_offsets[e1]+1;
            VertexId v1 = dirH.ed_vertices[iv1];
            VertexId v2 = dirH.ed_vertices[iv2];
            if(singletonmapped[v1] && singletonmapped[v2]){
                count_contained[2]++;
            }
            if(singletonmapped[v1]){
                count_contained[1] += (indegrees[v2]-internal_indegrees[1]);
            }
            if(singletonmapped[v2]){
                count_contained[1] += (indegrees[v1]- internal_indegrees[0]);
            }
            count_contained[0] += (indegrees[v1]- internal_indegrees[0])*(indegrees[v2]-internal_indegrees[1]);
        }

        for (VertexId iv1 = dirH.edge_offsets[e1]; iv1 <  e1end; iv1++){
            VertexId v1 = dirH.ed_vertices[iv1];
            EdgeId v1end = dirH.vertex_offset[v1] + dirH.outdegrees[v1];
            for(EdgeId ie2 = dirH.vertex_offset[v1]; ie2 < v1end; ie2++){
                EdgeId e2 = dirH.ve_hyperedges[ie2];
                VertexId iv3;
                if(check_one_off(dirH,e1,e2,v1, iv3)){
                    VertexId v3 = dirH.ed_vertices[iv3+dirH.edge_offsets[e1]];
                    int e2_contained =  dirH.edge_sizes[e1] == dirH.edge_sizes[e2] + 1 ? 1 : 0;

                    if(singletonmapped[v3]){
                        count_contained[e2_contained+1]++;
                    }
                    count_contained[e2_contained] += indegrees[v3] - internal_indegrees[iv3];
                }
            }
        }
        delete[] internal_indegrees;
    }
    counts[20] = count_contained[2];
    counts[22] = count_contained[1];
    counts[24] = count_contained[0] - counts[16]*3 - counts[17]*2 - counts[18];

    VertexId n_double_children = 0;
    VertexId n_paths = 0;
    VertexId n_children_intersect = 0;

    for(EdgeId e = 0; e < dirH.num_hyperedges; e++){
        n_paths += (dirH.edge_intersections[e]*(dirH.edge_intersections[e]-1))/2;
        n_double_children += (dirH.n_children[e]*(dirH.n_children[e]-1))/2;
        n_children_intersect += dirH.n_children[e]*(dirH.edge_intersections[e]-dirH.n_children[e]-dirH.n_parents[e]);
    }

    counts[21] = n_double_children - counts[20] - counts[0] - counts[3] - counts[4];
    counts[23] = n_children_intersect - counts[23] - counts[22] - counts[5] - counts[6] - counts[7] - 2*(counts[2] + counts[1]);
    
    VertexId paths_accounted = 0;
    for(int i = 0; i < 20; i++){
        paths_accounted += 3*counts[i];
    }
    for(int i=20; i <26; i++){
        paths_accounted += counts[i];
    }
    counts[25] = n_paths-paths_accounted;
    delete[] indegrees;
}