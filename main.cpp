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
        std::cerr << "Usage: " << argv[0] << " <hypergraph_file> <c(ount)/e(numerate)>\n";
        return 1;
    }
    bool enumerate = false;
    std::string mode = argv[2];
    if (mode == "c") {
      enumerate = false;
    } else if (mode == "e"){
      enumerate = true;
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
    std::cout << "Read hypergraph and created adjacency lists: "
		<< (double)(stop - start) / CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl << std::endl;

    VertexId* ordering = new VertexId[H.num_vertices]();
    compute_degeneracy_ordering(dirH, H, ordering);
    prev = stop;
    stop = clock();
    std::cout << "Computed degeneracy ordering: " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl << std::endl;

    EdgeId* counts = new EdgeId[20]();

    if(enumerate){
    count_hypertriangles(dirH, counts);
    prev = stop;
    stop = clock();
    std::cout << "Computed trimmed-triangle based hypertriangles (13-20): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl << std::endl;
    

    count_hypertriangles_dense(H, dirH, counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed star-based hypertriangles (1-16): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
      std::cout << "------------------------------------------------------" << std::endl << std::endl;
    }else{
      count_hypertriangles_flexible(dirH,counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed trimmed-triangle and semi-triangle based hypertriangles (4-5,7-8,11-20): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
      std::cout << "------------------------------------------------------" << std::endl << std::endl;

      count_contained_triangles(dirH,counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed contained hypertriangles (1-3,6): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
      std::cout << "------------------------------------------------------" << std::endl << std::endl;

      std::tuple<EdgeId, EdgeId> stars = getTotalStarCount(dirH);
      compute_final_counts(stars, counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed total weighted count of stars (1-16) and extended stars (1,3-8,10-16): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
      std::cout << "------------------------------------------------------" << std::endl << std::endl;
      
    }
    std::cout <<"Counts: \n";
    for (int i = 1; i <= 20; i++){
      std::cout << i<<"\t" << counts[i-1]<< "\n";
    }
    std::cout << "\n" << "Time:\t" << (double)(stop - start) / CLOCKS_PER_SEC << " sec" << std::endl;

    delete[] ordering;
    delete[] counts;
    return 0;
}