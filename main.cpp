#include <iostream>
#include <vector>
#include <string>
#include "read_hypergraph.h"
#include "degeneracy.h"
#include "types.h"
#include "HypergraphCSR.h"
#include "count_patterns.h"
#include <ctime>
#include <tuple>


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <hypergraph_file> <c(losed)/a(ll)>\n";
        return 1;
    }

    //Mode selector c for only patterns 1-20, a computes also patterns 21-26
    bool all_patterns = false;
    std::string mode = argv[2];
    if (mode == "c") {
      all_patterns = false;
    } else if (mode == "a"){
      all_patterns = true;
    } else {
      std::cerr << "Usage: " << argv[0] << " <hypergraph_file> <c(losed)/a(ll)>\n";
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

    //Compute the degeneracy ordering
    VertexId* ordering = new VertexId[H.num_vertices]();
    compute_degeneracy_ordering(dirH, H, ordering);
    prev = stop;
    stop = clock();
    std::cout << "Computed degeneracy ordering (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

    EdgeId* counts = new EdgeId[26]();

    //Compute statistics for the hypergraph
    H.compute_sum_degrees();
    dirH.compute_sum_outdegrees();

    //Compute the hyperedge degrees
    dirH.compute_edge_degrees();
    prev = stop;
    stop = clock();
    std::cout << "Computed edge_degrees (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

    //Compute the triangle based patterns
    count_triangle_based_patterns(dirH,counts);
    prev = stop;
    stop = clock();
    std::cout << "Computed triangle based patterns (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

    //Compute closed contained patterns
    count_closed_contained_patterns(dirH,counts);
    prev = stop;
    stop = clock();
    std::cout << "Computed closed contained patterns (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

    //Compute the number of stars and extended stars
    std::tuple<EdgeId, EdgeId> stars = count_stars(dirH);
    //Compute the counts of patterns 9 and 10
    compute_final_counts(stars, counts);
    prev = stop;
    stop = clock();
    std::cout << "Computed stars and extended stars (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;

    if(all_patterns) {
      //Compute open patterns
      compute_open_patterns(dirH, counts);
      prev = stop;
      stop = clock();
      std::cout << "Computed open patterns (s): " << (double)(stop - prev) / CLOCKS_PER_SEC << std::endl;
    }

    //Print the counts
    stop = clock();
    std::cout <<"Counts: \n";
    if(!all_patterns){
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