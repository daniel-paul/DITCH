#include <iostream>
#include <vector>
#include <string>
#include "read_hypergraph.h"
#include "degeneracy.h"
#include "types.h"
#include "HypergraphCSR.h"
#include "count_hypertriangles.h"
#include <ctime>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <hypergraph_file>\n";
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
    std::cout << "Computed degeneracy ordering: "
		<< (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
	std::cout << "------------------------------------------------------" << std::endl << std::endl;

    count_hypertriangles(dirH, ordering);
    stop = clock();
    std::cout << "Computed hypertriangles: "
		<< (double)(stop - prev) / CLOCKS_PER_SEC << " sec" << std::endl;
	std::cout << "------------------------------------------------------" << std::endl << std::endl;

    delete[] ordering;
    return 0;
}