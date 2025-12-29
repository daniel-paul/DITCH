#include "count_patterns.h"
#include <tuple>
#include <limits>

//AUXILIARY FUNCTIONS

//Computes the intersection between two hyperedges which gets written into out, return the size of the intersection
int edge_intersection(DirHypergraphCSR& dirH, EdgeId e1, EdgeId e2, VertexId* out){
    int size = 0;
    VertexId p1 = dirH.edge_offsets[e1];
    VertexId p2 = dirH.edge_offsets[e2];
    VertexId p1end = dirH.edge_offsets[e1] + dirH.edge_sizes[e1];
    VertexId p2end = dirH.edge_offsets[e2] + dirH.edge_sizes[e2];
    //Linear search through e1 and e2
    while (p1 < p1end && p2 < p2end){
        VertexId v1 = dirH.ed_vertices[p1];
        VertexId v2 = dirH.ed_vertices[p2];
        if(v1 == v2){ //If a vertex is present in both e1 and e2 we add it to out and increase size.
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

//This function takes three hyperedges and one vertex. It returns the containment struct that indicates:
//-source_intersect: true if v is the first vertex in the intersection of e1,e2 and e3
//-e1contained: true if the endpoint of e1 is contained in the intersection of e1, e2 and e3
//-similarly for e2contained and e3contained
//Also returns the TripleIntersections struct indicating the blue and red regions present.
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


    //Performs a linear search through e1, e2 and e3
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

//--------------------------------------------------------------------------------------------------------
//MAIN FUNCTION
//--------------------------------------------------------------------------------------------------------
//This function takes a directed hypergraph and returns the number of Stars and Extended Stars as a tuple.
std::tuple<EdgeId, EdgeId> count_stars(DirHypergraphCSR& dirH) {

    //Compute the indegrees of each vertex
    EdgeId* indegrees = new EdgeId[dirH.num_vertices]();
    for(EdgeId e =0; e < dirH.num_hyperedges; e++){
        indegrees[dirH.ed_vertices[dirH.edge_offsets[e]+dirH.edge_sizes[e]-1]]++;
    }

    EdgeId stars = 0;
    EdgeId extended_stars = 0;

    //Compute stars that contain an endpoint in the intersection of the three hyperedges.
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
                //Compute the intersection of e1 and e2
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
                //For the last vertex we need to compute the number of edges ending on it that contain the intersection and substract them from the count.
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
                        if (!dirH.contains_vertex(e3,intersection[l])){
                            contains_all = false;
                            break;
                        }
                    }
                    //Subtract in that case
                    if (contains_all) {
                        extended_stars -= 1;
                    }
                }

                //Finally, If the last in the intersection is also last for e1 or e2 we need to consider the hyperedges starting from it
                //Those hyperedges must not intersect with the intersection of e1 and e2
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
                    // Check if does not intersect with the intersection of e1 and e2
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
                    //If it does not then increase the counter
                    if(!contains_any){
                        extended_stars++;
                    }
                }
                delete[] intersection;
            }
        }
    }


    //Count the stars and extended stars that do not contain an endpoint
    //Fix a vertex v
    for (VertexId v = 0 ; v < dirH.num_vertices; v++) {
        //Loop over each triplet of out-neighbors
        for (EdgeId i = dirH.vertex_offset[v]; i < dirH.vertex_offset[v] + dirH.outdegrees[v]; i++){
            for (EdgeId j = i+1; j < dirH.vertex_offset[v] + dirH.outdegrees[v]; j++){
                for (EdgeId k = j+1; k < dirH.vertex_offset[v] + dirH.outdegrees[v]; k++){
                    EdgeId e1 = dirH.ve_hyperedges[i];
                    EdgeId e2 = dirH.ve_hyperedges[j];
                    EdgeId e3 = dirH.ve_hyperedges[k];

                    //Check that v is the first common vertex in the intersection of e1 e2 and e3
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

//Computes the counts of pattern 9 and 10 from the stars and the rest of the counts
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