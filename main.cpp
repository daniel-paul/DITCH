#include <iostream>
#include <vector>
#include <string>
#include "read_hypergraph.h"
#include "degeneracy.h"
#include "types.h"
#include "HypergraphCSR.h"
#include "count_hypertriangles.h"
#include <ctime>
#include <tuple>


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <hypergraph_file> <c(ount)/e(numerate)/(p)aths>\n";
        return 1;
    }
    bool enumerate = false;
    bool paths = false;
    std::string mode = argv[2];
    if (mode == "c") {
      enumerate = false;
    } else if (mode == "e"){
      enumerate = true;
    } else if (mode == "p") {
      enumerate = false;
      paths = true;
    } else {
      std::cerr << "Usage: " << argv[0] << " <hypergraph_file> <c(ount)/e(numerate)>\n";
        return 1;
    }


    clock_t start;
    clock_t stop;
    clock_t prev;

    std::string filename = argv[1];

    // Read hypergraph
    start = clock();

    HypergraphCSR H;
    DirHypergraphCSR dirH;
    
    read_hypergraph(H, filename);
    std::cout << "Number of vertices: " << H.num_vertices << "\n";
    std::cout << "Number of hyperedges: " << H.num_hyperedges << "\n";

    stop = clock();
    std::cout << "Read hypergraph and created adjacency lists (s): "
		<< (double)(stop - start) / CLOCKS_PER_SEC <<  std::endl;

    VertexId* ordering = new VertexId[H.num_vertices]();
    compute_degeneracy_ordering(dirH, H, ordering);
    prev = stop;
    stop = clock();
    std::cout << "Computed degeneracy ordering (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

    EdgeId* counts = new EdgeId[26]();

    H.compute_sum_degrees();
    dirH.compute_sum_outdegrees();

    if(enumerate){
    count_hypertriangles(dirH, counts);
    // prev = stop;
    // stop = clock();
    // std::cout << "Computed trimmed-triangle based hypertriangles (13-20): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
    // std::cout << "------------------------------------------------------" << std::endl << std::endl;
    

    count_hypertriangles_dense(H, dirH, counts);
      // prev = stop;
      // stop = clock();
      // std::cout << "Computed star-based hypertriangles (1-16): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
      // std::cout << "------------------------------------------------------" << std::endl << std::endl;
    }else{
      dirH.compute_edge_degrees();
      prev = stop;
      stop = clock();
      std::cout << "Computed edge_degrees (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;
      count_hypertriangles_flexible(dirH,counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed trimmed-triangles and quasi-trimmed-triangles (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

      count_contained_triangles(dirH,counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed contained hypertriangles (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

      std::tuple<EdgeId, EdgeId> stars = getTotalStarCount(dirH);
      compute_final_counts(stars, counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed total weighted count of stars (1-16) and extended stars (1,3-8,10-16) (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

      if(paths) {
        compute_non_hypertriangles(dirH, counts);
        prev = stop;
        stop = clock();
        std::cout << "Computed non-triangles (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;
      }
    }
    stop = clock();
    std::cout <<"Counts: \n";
    if(!paths){
      for (int i = 1; i <= 20; i++){
        std::cout << i<<"\t" << counts[i-1]<< "\n";
      }
    } else {
      for (int i = 1; i <= 26; i++){
        std::cout << i<<"\t" << counts[i-1]<< "\n";
      }
    }
    std::cout << "\n" << "Time (s):\t" << (double)(stop - start) / CLOCKS_PER_SEC << std::endl;

    delete[] ordering;
    delete[] counts;
    return 0;
}