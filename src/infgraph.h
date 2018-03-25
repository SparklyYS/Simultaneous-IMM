
typedef pair<double, int> dipair;

#include "iheap.h"
#include "ibfs/ibfs.h"
#include <queue>
#include <utility>

struct CompareBySecond
{
    bool operator()(pair<int, int> a, pair<int, int> b)
    {
        return a.second < b.second;
    }
};

class InfGraph : public Graph
{
  private:
    vector<bool> visit;
    vector<int> visit_mark;

  public:
    vector< vector<int> > hyperG;
    vector< vector<int> > hyperGT;
    //return the number of edges visited
    deque<int> q;
    sfmt_t sfmtSeed;
    vector<int> seedSet;

    InfGraph(string dataset) : Graph(dataset)
    {
        sfmt_init_gen_rand(&sfmtSeed, 95082);
        init_hyper_graph();
        visit = vector<bool>(n);
        visit_mark = vector<int>(n);
    }

    void init_hyper_graph()
    {
        hyperG.clear();
        for (int i = 0; i < n; i++)
            hyperG.push_back(vector<int>());
        hyperGT.clear();
    }

    /*
     * BFS starting from one node
    */
    int BuildHypergraphNode(int uStart, int hyperiiid)
    {
        int n_visit_edge = 1;
        ASSERT((int)hyperGT.size() > hyperiiid);
        hyperGT[hyperiiid].push_back(uStart);

        int n_visit_mark = 0;

        q.clear();
        q.push_back(uStart);
        ASSERT(n_visit_mark < n);
        visit_mark[n_visit_mark++] = uStart;
        visit[uStart] = true;
        while (!q.empty())
        {

            int expand = q.front();
            q.pop_front();
            if (influModel == IC)
            {
                int i = expand;
                for (int j = 0; j < (int)gT[i].size(); j++)
                {
                    //int u=expand;
                    int v = gT[i][j];
                    n_visit_edge++;
                    double randDouble = sfmt_genrand_real1(&sfmtSeed);
                    if (randDouble > probT[i][j])
                        continue;
                    if (visit[v])
                        continue;
                    if (!visit[v])
                    {
                        ASSERT(n_visit_mark < n);
                        visit_mark[n_visit_mark++] = v;
                        visit[v] = true;
                    }
                    q.push_back(v);
                    ASSERT((int)hyperGT.size() > hyperiiid);
                    hyperGT[hyperiiid].push_back(v);
                }
            }
            else if (influModel == LT)
            {
                if (gT[expand].size() == 0)
                    continue;
                ASSERT(gT[expand].size() > 0);
                n_visit_edge += gT[expand].size();
                double randDouble = sfmt_genrand_real1(&sfmtSeed);
                for (int i = 0; i < (int)gT[expand].size(); i++)
                {
                    ASSERT(i < (int)probT[expand].size());
                    randDouble -= probT[expand][i];
                    if (randDouble > 0)
                        continue;
                    //int u=expand;
                    int v = gT[expand][i];

                    if (visit[v])
                        break;
                    if (!visit[v])
                    {
                        visit_mark[n_visit_mark++] = v;
                        visit[v] = true;
                    }
                    q.push_back(v);
                    ASSERT((int)hyperGT.size() > hyperiiid);
                    hyperGT[hyperiiid].push_back(v);
                    break;
                }
            }
            else
                ASSERT(false);
        }
        for (int i = 0; i < n_visit_mark; i++)
            visit[visit_mark[i]] = false;
        return n_visit_edge;
    }

    void build_hyper_graph_r(int64 R, const Argument &arg)
    {
        if (R > INT_MAX)
        {
            cout << "Error:R too large" << endl;
            exit(1);
        }
        //INFO("build_hyper_graph_r", R);

        int prevSize = hyperGT.size();
        while ((int)hyperGT.size() <= R)
            hyperGT.push_back(vector<int>());

        vector<int> random_number;
        for (int i = 0; i < R; i++)
        {
            random_number.push_back(sfmt_genrand_uint32(&sfmtSeed) % n);
        }

        // trying simultaneous BFS from same node
        //ibfs();
        //trying BFS start from same node
        for (int i = prevSize; i < R; i++)
        {
            BuildHypergraphNode(random_number[i], i);
        }
        cout << "R: " << R << endl;

        int totAddedElement = 0;
        for (int i = prevSize; i < R; i++)
        {
            for (int t : hyperGT[i])
            {
                hyperG[t].push_back(i);
                //hyperG.addElement(t, i);
                totAddedElement++;
            }
        }
    }

    //This is build on Mapped Priority Queue
    double build_seedset(int k)
    {

        priority_queue<pair<int, int>, vector<pair<int, int>>, CompareBySecond> heap;
        vector<int> coverage(n, 0);

        for (int i = 0; i < n; i++)
        {
            pair<int, int> tep(make_pair(i, (int)hyperG[i].size()));
            heap.push(tep);
            coverage[i] = (int)hyperG[i].size();
        }

        int maxInd;

        long long influence = 0;
        long long numEdge = hyperGT.size();

        // check if an edge is removed
        vector<bool> edgeMark(numEdge, false);
        // check if an node is remained in the heap
        vector<bool> nodeMark(n + 1, true);

        seedSet.clear();
        while ((int)seedSet.size() < k)
        {
            pair<int, int> ele = heap.top();
            heap.pop();
            if (ele.second > coverage[ele.first])
            {
                ele.second = coverage[ele.first];
                heap.push(ele);
                continue;
            }

            maxInd = ele.first;
            vector<int> e = hyperG[maxInd];
            influence += coverage[maxInd];
            seedSet.push_back(maxInd);
            nodeMark[maxInd] = false;

            for (unsigned int j = 0; j < e.size(); ++j)
            {
                if (edgeMark[e[j]])
                    continue;

                vector<int> nList = hyperGT[e[j]];
                for (unsigned int l = 0; l < nList.size(); ++l)
                {
                    if (nodeMark[nList[l]])
                        coverage[nList[l]]--;
                }
                edgeMark[e[j]] = true;
            }
        }
        return 1.0 * influence / hyperGT.size();
    }
};
