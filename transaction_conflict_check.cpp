#include <bits/stdc++.h>
using namespace std;
#define pb push_back

int step;
string fname = "transaction.txt";
map<string, vector<string>> processList;
map<string, set<pair<string, int>>> processTrack;
map<string, set<string>> adjacencyList;
map<string, int> indegree;
vector<string> topoOrder;

void preProcessing()
{
    ifstream file(fname);
    string line;
    int pos = 0;
    while (getline(file, line))
    {
        stringstream ss(line);
        string word, pname = "";
        int pos = -1;
        while (getline(ss, word, ','))
        {
            if (word[0] == 'T')
                pname = word;
            else
            {
                processList[pname].pb(word);
                processTrack[word].insert({pname, pos});
            }
            pos++;
        }
        step = processList[pname].size() - 1;
    }
}
set<string> conflictPair(string op)
{
    set<string> st;
    string temp = op;
    if (op[0] == 'R')
    {
        temp[0] = 'W';
        st.insert(temp);
    }
    else if (op[0] == 'W')
    {
        st.insert(temp);
        temp[0] = 'R';
        st.insert(temp);
    }
    return st;
}
void graphMake()
{
    int i;
    for (i = 0; i < step; i++)
    {
        for (auto x : processList)
        {
            string current = x.second[i];
            set<string> conflict = conflictPair(current);
            for (auto conflicted : conflict)
            {
                for (auto getname : processTrack[conflicted])
                {
                    if (getname.first != x.first && getname.second > i)
                    {
                        adjacencyList[x.first].insert(getname.first);
                    }
                }
            }
        }
    }
}
void topologicalSort()
{
    for (auto x : adjacencyList)
    {
        for (auto y : x.second)
            indegree[y]++;
    }
    queue<string> q;
    for (auto x : processList)
    {
        cout << x.first << " " << indegree[x.first] << "\n";
        if (indegree[x.first] == 0)
            q.push(x.first);
    }
    while (!q.empty())
    {
        string current = q.front();
        topoOrder.pb(current);
        q.pop();
        for (auto x : adjacencyList[current])
        {
            indegree[x]--;
            if (indegree[x] == 0)
                q.push(x);
        }
    }
}
void solve()
{
    if (topoOrder.size() == adjacencyList.size())
    {
        cout << "conflict serializable\nserial schedule:\n";
        for (auto x : topoOrder)
            cout << x << " ";
    }
    else
        cout << "conflict not serializable\n";
}
int main()
{
    preProcessing();
    graphMake();
    topologicalSort();
    solve();

    // for (auto x : processList)
    // {
    //     cout << x.first << " : ";
    //     for (auto y : x.second)
    //         cout << y << " ";
    //     cout << "\n";
    // }
    // for (auto x : processTrack)
    // {
    //     cout << x.first << " : ";
    //     for (auto y : x.second)
    //         cout << y.first << " " << y.second << "\n";
    //     cout << "\n";
    // }
    // cout << "\n";
    // for (auto x : adjacencyList)
    // {
    //     cout << x.first << " : ";
    //     for (auto y : x.second)
    //         cout << y << " ";
    //     cout << "\n";
    // }
    // for (auto x : processList)
    //     cout << x.first << " " << indegree[x.first] << "\n";
    // for (auto x : topoOrder)
    //     cout << x << " ";
}