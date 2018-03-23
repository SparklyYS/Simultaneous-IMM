#include "graph_reader.h"

graph_reader::graph_reader(const char *beg_file, const char *csr_file, const char *stat_file)
{
    vert_count = fsize(beg_file) / sizeof(index_tt) - 1;
    edge_count = fsize(csr_file) / sizeof(vertex_tt);

    // read from beg file
    FILE *file = fopen(beg_file, "rb");
    if (file == NULL)
    {
        cout << beg_file << "can't open " << endl;
        exit(-1);
    }

    index_tt *tmp_beg_pos = new index_tt[vert_count + 1];
    index_tt ret = fread(tmp_beg_pos, sizeof(index_tt), vert_count + 1, file);
    assert(ret == vert_count + 1);
    fclose(file);

    // read from csr file
    file = fopen(csr_file, "rb");
    if (file == NULL)
    {
        cout << csr_file << "can't open " << endl;
        exit(-1);
    }

    vertex_tt *tmp_csr = new vertex_tt[edge_count];
    ret = fread(tmp_csr, sizeof(vertex_tt), edge_count, file);
    assert(ret == edge_count);
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

    // read from the stat file
    file = fopen(stat_file, "rb");
    if (file == NULL)
    {
        cout << stat_file << " can't open" << endl;
        exit(-1);
    }

    index_t stat_fsize = fsize(stat_file) / sizeof(index_tt);
    cout << "stat_fsize: " << stat_fsize << endl;

    edge_stat = new index_tt[edge_count*2];
    ret = fread(edge_stat, sizeof(index_tt), edge_count*2, file);
    assert(ret == (edge_count *2));
    fclose(file);

    cout << "Graph load(success): " << vert_count << "verts, " << edge_count << "edges " << endl;
}

void graph_reader::print_graph()
{
    for (int i = 0; i < vert_count; i++)
    {
        for (int j = beg_pos[i]; j < beg_pos[i + 1]; j++)
        {
            cout << i << " " << csr[j] << " " << edge_stat[j*2] << " " << edge_stat[j*2+1] << endl;
        }
    }
}