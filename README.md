 ## Degeneracy Inspired Triangle Counter for Hypergraphs (DITCH) 

This package computes the number of hypertriangles and other motifs with three hyperedges in any hypergraph.

 ### How to build?

 Just run make in the main folder. It requires g++.

 ### How to run?

 Run ditch.exe / ditch  <hypergraph_file> <c(losed)/a(ll)/s(tatistics)> <output_file (only for s, do not include .csv)>

 example call:
 ditch.exe ./Datasets/email-Eu.csv c

 or 

  ditch ./Datasets/email-Eu.csv c

 ### Hypergraph file

The file must be in comma-delimited csv format. Each Row will correspond to one hyperedge with each column corresponding to each vertex of the hyperedge. Do not include a header.
We obtained all the datasets from https://www.cs.cornell.edu/~arb/data/ and cleaned repeated hyperedges and temporal information.

### Mode

Closed (c): Only patterns 1-20

All (a): All patterns 1-26

Statistics (s): Only outputs statistics, requires an extra parameter <output_file>. The program will create files prefixed by <output_file> with the degrees and outdegrees distributions.
