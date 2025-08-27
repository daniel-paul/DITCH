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
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <hypergraph_file>\n";
        return 1;
    }
    bool enumerate = false;

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
      count_contained_triangles(dirH,counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed contained hypertriangles (1-8): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
      std::cout << "------------------------------------------------------" << std::endl << std::endl;


      count_hypertriangles_flexible(dirH,counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed trimmed-triangle and semi-triangle based hypertriangles (11-20): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
      std::cout << "------------------------------------------------------" << std::endl << std::endl;

      std::tuple<EdgeId, EdgeId> stars = getTotalStarCount(dirH);
      compute_final_counts(stars, counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed total count of stars (1-16) and extended stars (1,3-8,10-16): " << (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
      std::cout << "------------------------------------------------------" << std::endl << std::endl;
      
    }

    for (int i = 1; i <= 20; i++){
      std::cout << "Number of unique hypertriangles of type "<<i<<": " << counts[i-1]<< "\n";
    }
    std::cout << "\n" << "Total time: " << (double)(stop - start) / CLOCKS_PER_SEC << " sec" << std::endl;

    delete[] ordering;
    delete[] counts;
    return 0;
}