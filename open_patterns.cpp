#include "count_patterns.h"
#include <tuple>
#include <limits>

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

    //Linear search through e1,e2,e3
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

        if (in1 && in2 && in3) { // all three
            return -1;
        }
        if (in1 && !in2 && !in3) { //only e1
            return -1;
        }
        if(first_v12 && in1 && in2) { //check if vertex1 is the common source of e1 e2
            if (v1 != vertex1) 
                return -1;
            first_v12 = false;
        }
        else if(first_v13 && in1 && in3) {//check if vertex2 is the common source of e1 e3
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

//Checks if e2 contains all the vertices of e1, except exactly one, which will be stored in missing_vertex
//Simultaneously checks that vertex1 is the first source in the intersection of e1 and e2
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

//--------------------------------------------------------------------------------------------------------
//MAIN FUNCTION
//--------------------------------------------------------------------------------------------------------
//This function takes a directed hypergraphs and the counts 1-20 and returns the counts of patterns 21-26
void compute_open_patterns(DirHypergraphCSR& dirH, EdgeId* counts) {

    //Compute the complement counts
    VertexId complementCounts[3] = {0, 0, 0};
    bool* singletonmapped = new bool[dirH.num_vertices]();
    EdgeId* indegrees = new EdgeId[dirH.num_vertices]();

    //Compute Indegrees
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
        //Loop through two vertices
        for (VertexId iv1 = dirH.edge_offsets[e1]; iv1 <  e1end; iv1++){
            for (VertexId iv2 = iv1+1; iv2 <  e1end; iv2++){
                VertexId v1 = dirH.ed_vertices[iv1];
                VertexId v2 = dirH.ed_vertices[iv2];
                EdgeId v1end = dirH.vertex_offset[v1] + dirH.outdegrees[v1];
                EdgeId v2end = dirH.vertex_offset[v2] + dirH.outdegrees[v2]; 
                //Loop through one out-neighbor of each vertex
                for(EdgeId ie2 = dirH.vertex_offset[v1]; ie2 < v1end; ie2++){
                    EdgeId e2 = dirH.ve_hyperedges[ie2];
                    if (e2 == e1 || dirH.contains_vertex(e2,v2)) continue;
                    for(EdgeId ie3 = dirH.vertex_offset[v2]; ie3 < v2end; ie3++){
                        EdgeId e3 = dirH.ve_hyperedges[ie3];
                        if (e3 == e1 || e3 == e2) continue;
                        //Check if e2 and e3 cover e1 without all inersecting
                        int res = check_is_complement(dirH,e1,e2,e3,v1,v2);
                        if(res==-1) continue;
                        complementCounts[res]++;
                    }
                }
            }
        }
    }
    //Initialize the local indegrees array
    VertexId total_size = dirH.edge_sizes[dirH.num_hyperedges-1] + dirH.edge_offsets[dirH.num_hyperedges-1];
    VertexId* local_indegrees = new VertexId[total_size]();
    //For every hyperedge e1
    for (EdgeId e1 = 0; e1 < dirH.num_hyperedges; e1++){
        if (dirH.edge_sizes[e1] < 2) continue;

        //Compute the local indegrees for e1
        VertexId offset = dirH.edge_offsets[e1];
        VertexId e1end = dirH.edge_offsets[e1] + dirH.edge_sizes[e1];
        //Loop through its vertices
        for (VertexId iv1 = dirH.edge_offsets[e1]; iv1 <  e1end-1; iv1++){

            VertexId v1 = dirH.ed_vertices[iv1];
            EdgeId v1end = dirH.vertex_offset[v1] + dirH.outdegrees[v1];
            //For every out neighbor
            for(EdgeId ie2 = dirH.vertex_offset[v1]; ie2 < v1end; ie2++){
                EdgeId e2 = dirH.ve_hyperedges[ie2];
                //If v1 = s(e1,e2)
                if (dirH.isCommonSource(v1,e1,e2)){
                    VertexId laste2 = dirH.ed_vertices[dirH.edge_offsets[e2]+dirH.edge_sizes[e2]-1];
                    VertexId ilaste2 = contains_vertex_return_pos(dirH,e1,laste2);
                    if(ilaste2 == -1) continue;
                    //Store the last vertex of e2 as a local indegree for e1
                    local_indegrees[offset+ilaste2]++;
                }
            }
        }
        //If there are two vertices in e2
        if(dirH.edge_sizes[e1] == 2){
            VertexId iv1 = dirH.edge_offsets[e1];
            VertexId iv2 = dirH.edge_offsets[e1]+1;
            VertexId v1 = dirH.ed_vertices[iv1];
            VertexId v2 = dirH.ed_vertices[iv2];
            //Check for hyperedges ending in each of its vertices
            if(singletonmapped[v1] && singletonmapped[v2]){
                complementCounts[2]++;
            }
            if(singletonmapped[v1]){
                complementCounts[1] += (indegrees[v2]-local_indegrees[offset+1]);
            }
            if(singletonmapped[v2]){
                complementCounts[1] += (indegrees[v1]- local_indegrees[offset+0]);
            }
            complementCounts[0] += (indegrees[v1]- local_indegrees[offset+0])*(indegrees[v2]-local_indegrees[offset+1]);
        }

        //Iterate through each vertex in e1
        for (VertexId iv1 = dirH.edge_offsets[e1]; iv1 <  e1end; iv1++){
            VertexId v1 = dirH.ed_vertices[iv1];
            EdgeId v1end = dirH.vertex_offset[v1] + dirH.outdegrees[v1];
            //For each outneighbor
            for(EdgeId ie2 = dirH.vertex_offset[v1]; ie2 < v1end; ie2++){
                EdgeId e2 = dirH.ve_hyperedges[ie2];
                VertexId iv3;
                //If |e1 \ e2| = 1
                if(check_one_off(dirH,e1,e2,v1, iv3)){
                    //Check hyperedges ending in that vertex
                    VertexId v3 = dirH.ed_vertices[iv3+dirH.edge_offsets[e1]];
                    int e2_contained =  dirH.edge_sizes[e1] == dirH.edge_sizes[e2] + 1 ? 1 : 0;

                    if(singletonmapped[v3]){
                        complementCounts[e2_contained+1]++;
                    }
                    complementCounts[e2_contained] += indegrees[v3] - local_indegrees[offset+iv3];
                }
            }
        }
    }
    delete[] local_indegrees;


    //Compute the counts of the open patterns using the complement Counts, hyperedge degrees and number of children and parents
    counts[20] = complementCounts[2];
    counts[22] = complementCounts[1];
    counts[24] = complementCounts[0] - counts[16]*3 - counts[17]*2 - counts[18];

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