#pragma once
#include <string>
#include "HypergraphCSR.h"


// Reads a hypergraph from a file and returns its CSR representation
// The file format assumed: each line represents a hyperedge with space-separated vertex IDs
void read_hypergraph(HypergraphCSR& H,const std::string& filename);
