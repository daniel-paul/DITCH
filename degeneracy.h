#pragma once
#include "HypergraphCSR.h"

// Computes a degeneracy ordering of the hypergraph nodes
void compute_degeneracy_ordering(DirHypergraphCSR& dirH, HypergraphCSR& H, VertexId* ordering);
