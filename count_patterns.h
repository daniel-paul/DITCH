#pragma once
#include "HypergraphCSR.h"

//Structure that stores the intersection regions for three hyperedges.
struct TripleIntersections {
    int i12_not3 = 0;  // |e1 ∩ e2 \ e3|
    int i23_not1 = 0;  // |e2 ∩ e3 \ e1|
    int i31_not2 = 0;  // |e3 ∩ e1 \ e2|
    int i123 = 0;      // |e1 ∩ e2 ∩ e3|
    int i1_not23 = 0;  // |e1 \ e2 \ e3|
    int i2_not13 = 0;  // |e2 \ e1 \ e3|
    int i3_not12 = 0;  // |e3 \ e1 \ e2|
};

void count_closed_contained_patterns(DirHypergraphCSR& dirH, EdgeId* counts);
void count_triangle_based_patterns(DirHypergraphCSR& dirH, EdgeId* counts);
std::tuple<EdgeId, EdgeId> count_stars(DirHypergraphCSR& dirH);
void compute_final_counts(std::tuple<EdgeId, EdgeId> stars,  EdgeId* counts);
void compute_open_patterns(DirHypergraphCSR& dirH, EdgeId* counts);
