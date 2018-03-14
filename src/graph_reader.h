#ifndef __GRAPH_READER_H__
#define __GRAPH_READER_H__

#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>

typedef int vertex_t;
typedef int index_t;
typedef unsigned long long index_tt;
typedef unsigned long long vertex_tt;

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
    vertex_tt *edge_state;
    index_t vert_count;
    index_t edge_count;


  public:
    graph_reader() {}

    ~graph_reader() {}

    graph_reader(const char *beg_file, const char *csr_file, const char *weight_file)
    {
        vert_count = fsize(beg_file) / sizeof(index_tt) - 1;
        edge_count = fsize(csr_file) / sizeof(vertex_tt);

        // read beg_pos from file to tmp buffer
        FILE *file = fopen(beg_file, "rb");
        if (file == NULL)
        {
            std::cout << beg_file << " can't open" << std::endl;
            exit(-1);
        }

        index_tt *tmp_beg_pos = new index_tt[vert_count + 1];
        index_tt ret = fread(tmp_beg_pos, sizeof(index_tt), vert_count + 1, file);
        assert(ret == vert_count + 1);
        fclose(file);

        // read csr from file to tmp buffer
        file = fopen(csr_file, "rb");
        if (file == NULL)
        {
            std::cout << csr_file << " can't open" << std::endl;
            exit(-1);
        }

        vertex_tt *tmp_csr = new vertex_tt[edge_count];
        ret = fread(tmp_csr, sizeof(vertex_tt), edge_count, file);
        assert(ret == edge_count);
        fclose(file);

        // read weight from file to tmp buffer
        file = fopen(weight_file, "rb");
        if (file == NULL)
        {
            std::cout << weight_file << " can't open" << std::endl;
            exit(-1);
        }

        vertex_tt *edge_state = new vertex_tt[2 * edge_count];
        ret = fread(edge_state, sizeof(vertex_tt), edge_count << 1, file);
        assert(ret == (edge_count << 1));
        fclose(file);

        // converting to uint32_t
        beg_pos = new index_t[vert_count + 1];
        csr = new vertex_t[edge_count];

        for (index_t i = 0; i < vert_count + 1; i++)
            beg_pos[i] = (index_t)tmp_beg_pos[i];

        for (index_t i = 0; i < edge_count; i++)
            csr[i] = (vertex_t)tmp_csr[i];

        delete[] tmp_beg_pos;
        delete[] tmp_csr;

        std::cout << "Graph Load(Success)" << std::endl;
        std::cout << "vertex count: " << vert_count << " edges: " << edge_count << std::endl;
    }
};

#endif