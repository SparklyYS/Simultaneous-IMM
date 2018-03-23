#ifndef GRAPH_READER
#define GRAPH_READER

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <iostream>

typedef int index_t;
typedef int vertex_t;
typedef uint64_t index_tt;
typedef uint64_t vertex_tt;


using namespace std;

inline off_t fsize(const char *filename) 
  {
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1;
}

class graph_reader
{
  public:
    index_t *beg_pos;
    vertex_t *csr;
    index_t vert_count;
    index_t edge_count;
    index_tt *edge_stat;
    

  public:
    graph_reader() {}
    ~graph_reader() {}
    graph_reader(const char *beg_file, const char *csr_file, const char *stat_file);

    void print_graph();
};

#endif