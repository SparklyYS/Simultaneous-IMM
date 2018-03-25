#define HEAD_INFO
//#define HEAD_TRACE
#include "sfmt/SFMT.h"
#include "head.h"

using namespace std;
typedef double (*pf)(int, int);

inline off_t fsize(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1;
}

class Graph
{
  public:
    int n, m, k;
    vector<int> inDeg;
    vector<vector<int>> gT;

    index_t *beg_pos;
    vertex_t *csr;
    index_tt *edge_stat;
    index_tt *indeg;

    vector<vector<double>> probT;

    enum InfluModel
    {
        IC,
        LT,
        CONT
    };
    InfluModel influModel;
    void setInfuModel(InfluModel p)
    {
        influModel = p;
        TRACE(influModel == IC);
        TRACE(influModel == LT);
        TRACE(influModel == CONT);
    }

    string folder;
    string graph_file;
    void readNM()
    {
        ifstream cin((folder + "attribute.txt").c_str());
        ASSERT(!cin == false);
        string s;
        while (cin >> s)
        {
            if (s.substr(0, 2) == "n=")
            {
                n = atoi(s.substr(2).c_str());
                continue;
            }
            if (s.substr(0, 2) == "m=")
            {
                m = atoi(s.substr(2).c_str());
                continue;
            }
            ASSERT(false);
        }
        TRACE(n, m);
        cin.close();
    }

    void add_edge(int a, int b, double p)
    {
        probT[b].push_back(p);
        gT[b].push_back(a);
        inDeg[b]++;
    }

    // test the csr format
    void GraphRead()
    {
        // init
        for (int i = 0; i < n; i++)
        {
            gT.push_back(vector<int>());
            probT.push_back(vector<double>());
        }

        //
        for (int i = 0; i < n; i++)
        {
            for (int j = beg_pos[i]; j < beg_pos[i + 1]; j++)
            {
                int adj_vert = csr[j];
                double prob = 1.0 / indeg[adj_vert];

                probT[adj_vert].push_back(prob);
                gT[adj_vert].push_back(i);
            }
        }
    }

    vector<bool> hasnode;
    void readGraph()
    {
        FILE *fin = fopen((graph_file).c_str(), "r");
        ASSERT(fin != false);
        int readCnt = 0;
        for (int i = 0; i < m; i++)
        {
            readCnt++;
            int a, b;

            double p;
            int c = fscanf(fin, "%d%d%lf", &a, &b, &p);
            ASSERTT(c == 3, a, b, p, c);

            //TRACE_LINE(a, b);
            ASSERT(a < n);
            ASSERT(b < n);
            hasnode[a] = true;
            hasnode[b] = true;

            add_edge(a, b, p);
        }
        TRACE_LINE_END();
        int s = 0;
        for (int i = 0; i < n; i++)
            if (hasnode[i])
                s++;
        INFO(s);
        ASSERT(readCnt == m);
        fclose(fin);
    }

    void readGraphBin()
    {
        string graph_file_bin = graph_file.substr(0, graph_file.size() - 3) + "bin";
        INFO(graph_file_bin);
        FILE *fin = fopen(graph_file_bin.c_str(), "rb");
        //fread(fin);
        struct stat filestatus;
        stat(graph_file_bin.c_str(), &filestatus);
        int64 sz = filestatus.st_size;
        char *buf = new char[sz];
        int64 sz2 = fread(buf, 1, sz, fin);
        INFO("fread finish", sz, sz2);
        ASSERT(sz == sz2);
        for (int64 i = 0; i < sz / 12; i++)
        {
            int a = ((int *)buf)[i * 3 + 0];
            int b = ((int *)buf)[i * 3 + 1];
            float p = ((float *)buf)[i * 3 + 2];
            //INFO(a,b,p);
            add_edge(a, b, p);
        }
        delete[] buf;
        fclose(fin);
    }

    // the graph is stored as a CSR format
    Graph(string dataset)
    {
        string beg_file = dataset + "_beg.bin";
        string csr_file = dataset + "_csr.bin";
        string indeg_file = dataset + "_indeg.bin";
        string stat_file = dataset + "_weight.bin";

        n = fsize(beg_file.c_str()) / sizeof(index_tt) - 1;
        m = fsize(csr_file.c_str()) / sizeof(vertex_tt);

        // read from the beg file
        FILE *file = fopen(beg_file.c_str(), "rb");
        if (file == NULL)
        {
            cout << beg_file << "can't open" << endl;
            exit(-1);
        }

        index_tt *tmp_beg_pos = new index_tt[n + 1];
        index_t ret = fread(tmp_beg_pos, sizeof(index_tt), n + 1, file);
        assert(ret == n + 1);
        fclose(file);

        // read from csr file
        file = fopen(csr_file.c_str(), "rb");
        if (file == NULL)
        {
            cout << csr_file << "can't open" << endl;
            exit(-1);
        }

        vertex_tt *tmp_csr = new vertex_tt[m];
        ret = fread(tmp_csr, sizeof(vertex_tt), m, file);
        assert(ret == m);
        fclose(file);

        // converting to uint32_t
        beg_pos = new index_t[n + 1];
        csr = new vertex_t[m];

        for (index_t i = 0; i < n + 1; i++)
            beg_pos[i] = (index_t)tmp_beg_pos[i];

        for (index_t i = 0; i < m; i++)
            csr[i] = (vertex_t)tmp_csr[i];

        delete[] tmp_beg_pos;
        delete[] tmp_csr;

        // read from the stat file
        file = fopen(stat_file.c_str(), "rb");
        if (file == NULL)
        {
            cout << stat_file << "can't open" << endl;
            exit(-1);
        }

        index_t stat_fsize = fsize(stat_file.c_str()) / sizeof(index_tt);
        cout << "stat_fsize: " << stat_fsize << endl;

        edge_stat = new index_tt[m * 2];
        ret = fread(edge_stat, sizeof(index_tt), m * 2, file);
        assert(ret == (m * 2));
        fclose(file);

        // Just add fllowing codes temporarily
        // read from the indeg file
        file = fopen(indeg_file.c_str(), "rb");
        if (file == NULL)
        {
            cout << indeg_file << "can't open" << endl;
            exit(-1);
        }
        indeg = new index_tt[n];
        ret = fread(indeg, sizeof(index_tt), n, file);
        assert(ret == n);
        fclose(file);

        cout << "Graph load(Success): " << n << "verts, " << m << "edges" << endl;

        GraphRead();
    }

    Graph(string folder, string graph_file) : folder(folder), graph_file(graph_file)
    {
        readNM();

        //init vector
        FOR(i, n)
        {
            gT.push_back(vector<int>());
            hasnode.push_back(false);

            probT.push_back(vector<double>());

            //hyperGT.push_back(vector<int>());
            inDeg.push_back(0);
        }
        readGraph();
        //system("sleep 10000");
    }
};
double sqr(double t)
{
    return t * t;
}
