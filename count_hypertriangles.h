#pragma once
#include "HypergraphCSR.h"

void count_hypertriangles(DirHypergraphCSR& dirH, EdgeId* counts);
void count_hypertriangles_dense(HypergraphCSR& H, DirHypergraphCSR& dirH, EdgeId* counts);
void count_contained_triangles(DirHypergraphCSR& dirH, EdgeId* counts);
void count_hypertriangles_flexible(DirHypergraphCSR& dirH, EdgeId* counts);
std::tuple<EdgeId, EdgeId> getTotalStarCount(DirHypergraphCSR& dirH);
void compute_final_counts(std::tuple<EdgeId, EdgeId> stars,  EdgeId* counts);
void compute_non_hypertriangles(DirHypergraphCSR& dirH, EdgeId* counts);
