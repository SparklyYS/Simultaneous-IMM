#include <cstdio>
#include <iostream>
#include "graph_reader.h"

using namespace std;

int main(int argc, char **argv)
{
    const char *beg_file = argv[1];  // beg_file
    const char *csr_file = argv[2];  // csr_file
    const char *stat_file = argv[3]; // stat_file

    graph_reader g(beg_file, csr_file, stat_file);
    g.print_graph();

    return 0;
}