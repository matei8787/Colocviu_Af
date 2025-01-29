#include<bits/stdc++.h>
using namespace std;
struct Nod
{
    int to,cost;
};
struct Muchie
{
    int x,y,cost;
};
class Graf
{
    vector<Nod> *g, *gt;
    vector<Muchie> muchii;
    bool orientat;
    bool ponderat;
    vector<int> viz, st, tata_dsu;
    map<pair<int, int>, int> cost_usor;
    static vector<int> dist_dijkstra;
public:
    int n, m;
    Graf(int n, int m, bool orientat = false, bool ponderat = false)
    {
        this->n = n;
        this->m = m;
        this->orientat = orientat;
        this->ponderat = ponderat;
        g = new vector<Nod>[n+1];
        gt = new vector<Nod>[n+1];
        viz = vector<int>(n+1, 0);
    }
    void addEdge(int x, int y, int cost = 1)
    {
        Nod nod;
        nod.to = y;
        nod.cost = cost;
        g[x].push_back(nod);
        nod.to = x;
        gt[y].push_back(nod);
        if ( !orientat )
        {
            nod.to = x;
            g[y].push_back(nod);
        }
        Muchie muchie = {x, y, cost};
        muchii.push_back(muchie);
        cost_usor[make_pair(x, y)] = cost;
        if ( !orientat )
            cost_usor[make_pair(y, x)] = cost;
    }
    void readGraph(istream &in)
    {
        int x, y, cost;
        for ( int i = 1; i <= m; i++ )
        {
            in >> x >> y;
            if (ponderat)
            {
                in >> cost;
                addEdge(x, y, cost);
            }
            else
            {
                addEdge(x, y);
            }
        }
    }
    void afisare()
    {
        for ( int i = 1; i <= n; i++ )
        {
            cout << i << ": ";
            for ( auto it : g[i] )
                cout << it.to << " ";
            cout << "\n";
        }
    }
    void afisareMuchii()
    {
        for ( auto it : muchii )
            cout << it.x << " " << it.y << " " << it.cost << "\n";
    }
    void dfs_cc(int nod, int nr)
    {
        viz[nod] = nr;
        for ( auto it : g[nod] )
            if ( !viz[it.to] )
                dfs_cc(it.to, nr);   
    }
    vector< vector<int> > cc()
    {
        viz.clear();
        viz.resize(n+1, 0);
        int nr = 0;
        for ( int i = 1 ; i <= n ; i++ )
        {
            if ( !viz[i] )
                dfs_cc(i, ++nr);
        }
        vector< vector<int> > ans;
        for ( int i = 1 ; i <= nr ; i++ )
        {
            vector<int> aux;
            for ( int j = 1 ; j <= n ; j++ )
                if ( viz[j] == i )
                    aux.push_back(j);
            ans.push_back(aux);
        }
        return ans;
    }
    void dfs_orientat_ctc(int nod, int nr)
    {
        viz[nod] = nr;
        for ( auto it : g[nod] )
            if ( !viz[it.to] )
                dfs_orientat_ctc(it.to, nr);
        st.push_back(nod);
    }
    void dfs_orientat_gt_ctc(int nod, int nr)
    {
        viz[nod] = nr;
        for ( auto it : gt[nod] )
            if ( !viz[it.to] )
                dfs_orientat_gt_ctc(it.to, nr);
    }
    vector< vector<int> > ctc()
    {
        viz.clear();
        viz.resize(n+1, 0);
        for ( int i = 1 ; i <= n ; i++ )
            if ( !viz[i] )
                dfs_orientat_ctc(i, 1);
        reverse(st.begin(), st.end());
        int nr = 0;
        viz.clear();
        viz.resize(n+1, 0);
        for ( int i : st )
            if ( !viz[i] )
                dfs_orientat_gt_ctc(i, ++nr);
        vector< vector<int> > ans;
        for ( int i = 1 ; i <= nr ; i++ )
        {
            vector<int> aux;
            for ( int j = 1 ; j <= n ; j++ )
                if ( viz[j] == i )
                    aux.push_back(j);
            ans.push_back(aux);
        }
        return ans;
    }
    vector< vector<int> > make_cc()
    {
        if ( !orientat )
            return cc();
        return ctc();
    }
    vector<Muchie> gaseste_ciclu(Muchie mu)
    {
        g[mu.x].push_back({mu.y, mu.cost});
        if (!orientat)
            g[mu.y].push_back({mu.x, mu.cost});

        vector<int> parent(n + 1, -1);
        vector<Muchie> cycle;
        bool found = false;

        function<void(int, int)> dfs = [&](int node, int tata) {
            if (found) return;
            viz[node] = 1;
            for (auto &vec : g[node]) {
                if ( found )
                    return;
                if ( vec.to == tata ) 
                    continue;
                if ( viz[vec.to] == 0 ) 
                {
                    parent[vec.to] = node;
                    dfs(vec.to, node);
                } 
                else if (viz[vec.to] == 1) 
                {
                    found = true;
                    int nod = node;
                    while (nod != vec.to) {
                        cycle.push_back({nod, parent[nod], cost_usor[make_pair(nod, parent[nod])]});
                        nod = parent[nod];
                    }
                    cycle.push_back({nod, vec.to, cost_usor[make_pair(nod, vec.to)]});
                    return;
                }
            }
        };

        viz.assign(n + 1, 0);
        dfs(mu.x, -1);
        g[mu.x].pop_back();
        if (!orientat)
            g[mu.y].pop_back();

        return cycle;
    }
    int gaseste_tata(int x)
    {
        if ( tata_dsu[x] == x )
            return x;
        return tata_dsu[x] = gaseste_tata(tata_dsu[x]);
    }
    void uneste_dsu(int x, int y)
    {
        x = gaseste_tata(x);
        y = gaseste_tata(y);
        if ( x == y )
            return;
        tata_dsu[y] = x;
    }
    vector<Muchie> kruskal()
    {
        tata_dsu.clear();
        tata_dsu.resize(n+1, 0);
        for ( int i = 1 ; i <= n ; i++ )
            tata_dsu[i] = i;
        sort(muchii.begin(), muchii.end(), [](Muchie a, Muchie b) { return a.cost < b.cost; });
        vector<Muchie> ans;
        for ( auto it : muchii )
        {
            if ( gaseste_tata(it.x) != gaseste_tata(it.y) )
            {
                ans.push_back(it);
                uneste_dsu(it.x, it.y);
            }
        }
        return ans;
    }
    vector<int> dijkstra(int start)
    {
        dist_dijkstra.resize(n+1, 1e9);
        struct crt
        {
            bool operator()(int a, int b)
            {
                return dist_dijkstra[a] > dist_dijkstra[b];
            }
        };
        priority_queue<int, vector<int>, crt> pq;
        dist_dijkstra[start] = 0;
        pq.push(start);
        while ( !pq.empty() )
        {
            int nod = pq.top();
            pq.pop();
            for ( auto it : g[nod] )
            {
                if ( dist_dijkstra[it.to] > dist_dijkstra[nod] + it.cost )
                {
                    dist_dijkstra[it.to] = dist_dijkstra[nod] + it.cost;
                    pq.push(it.to);
                }
            }
        }
        vector<int> ans(dist_dijkstra.begin(), dist_dijkstra.end());
        return ans;
    }
    int bfs(int start, int stop, vector<int>& t)
    {
        t.clear();
        t.resize(n+1, 0);
        queue<int> q;
        q.push(start);
        t[start] = -1;
        while ( !q.empty() )
        {
            int nod = q.front();
            q.pop();
            for ( auto it : g[nod] )
            {
                if ( !t[it.to] )
                {
                    t[it.to] = nod;
                    q.push(it.to);
                }
            }
        }
        return t[stop];
    }
    pair<int**, int> flux(int s=1, int t=-1)
    {
        if ( t == -1 )
            t = n;
        int a[n+1][n+1];
        for ( int i = 1 ; i <= n ; i++ )
            for ( int j = 1 ; j <= n ; j++ )
                a[i][j] = 0;
        for ( auto it : muchii )
            a[it.x][it.y] = it.cost;
        int f = 0;
        vector<int> tata(n+1, 0);
        while ( bfs(s, t, tata) )
        {
            int flow = 1e9;
            for ( int i = t ; i != s ; i = tata[i] )
                flow = min(flow, a[tata[i]][i]);
            for ( int i = t ; i != s ; i = tata[i] )
            {
                a[tata[i]][i] -= flow;
                a[i][tata[i]] += flow;
            }
            f += flow;
        }
        int** ans = new int*[n+1];
        for ( int i = 1 ; i <= n ; i++ )
        {
            ans[i] = new int[n+1];
            for ( int j = 1 ; j <= n ; j++ )
                ans[i][j] = a[i][j];
        }
        return make_pair(ans, f);
    }
};
int cost_apm_daca_adaug_muchie(Graf& g, Muchie mu)
{
    vector<Muchie> apm = g.kruskal();
    int cost = 0;
    Graf gr(g.n, g.n-1, false, true);
    for ( auto it : apm )
    {
        cost += it.cost;
        gr.addEdge(it.x, it.y, it.cost);
    }
    vector<Muchie> cycle = gr.gaseste_ciclu(mu);
    if ( cycle.size() == 0 )
        return cost;
    int max_cost = 0;
    for ( auto it : cycle )
        if ( ( it.x != mu.x && it.y != mu.y ) || ( it.x != mu.y && it.x != mu.y ) )
            max_cost = max(max_cost, it.cost);
    return cost - max_cost + mu.cost;
}
int cost_apm_daca_adaug_2_muchii(Graf& g, Muchie mu1, Muchie mu2)
{
    vector<Muchie> apm = g.kruskal();
    int cost = 0;
    Graf gr(g.n, g.n-1, false, true);
    for ( auto it : apm )
    {
        cost += it.cost;
        gr.addEdge(it.x, it.y, it.cost);
    }
    vector<Muchie> cycle1 = gr.gaseste_ciclu(mu1);
    vector<Muchie> cycle2 = gr.gaseste_ciclu(mu2);
    if ( cycle1.size() == 0 && cycle2.size() == 0 )
        return cost;
    if ( cycle1.size() == 0 )
    {
        int max_cost = 0;
        for ( auto it : cycle2 )
            if ( ( it.x != mu1.x && it.y != mu1.y ) || ( it.x != mu1.y && it.x != mu1.y ) )
                max_cost = max(max_cost, it.cost);
        return cost - max_cost + mu2.cost;
    }
    if ( cycle2.size() == 0 )
    {
        int max_cost = 0;
        for ( auto it : cycle1 )
            if ( ( it.x != mu2.x && it.y != mu2.y ) || ( it.x != mu2.y && it.x != mu2.y ) )
                max_cost = max(max_cost, it.cost);
        return cost - max_cost + mu1.cost;
    }
    int max_cost1 = 0;
    for (auto it : cycle1)
        if ( ( it.x != mu1.x && it.y != mu1.y ) || ( it.x != mu1.y && it.x != mu1.y ) )
            max_cost1 = max(max_cost1, it.cost);
    int max_cost2 = 0;
    for (auto it : cycle2)
        if ( ( it.x != mu2.x && it.y != mu2.y ) || ( it.x != mu2.y && it.x != mu2.y ) )
            max_cost2 = max(max_cost2, it.cost);

    return cost - max_cost1 - max_cost2 + mu1.cost + mu2.cost;
}
int cost_apm(Graf& g)
{
    vector<Muchie> apm = g.kruskal();
    int cost = 0;
    for ( auto it : apm )
        cost += it.cost;
    return cost;
}
int main()
{
    int n, m;
    cin>>n>>m;
    Graf g(n, m, true, true);
    int s, t;
    cin>>s>>t;
    g.readGraph(cin);
    cout<<g.flux(s, t).second<<'\n';
    return 0;
}
/*
6 12
4 3
1 3 5
1 5 4
2 1 3
2 6 5
4 2 6
4 3 3
4 5 4
4 6 6
5 3 4
6 1 5
6 2 4
6 3 100
*/