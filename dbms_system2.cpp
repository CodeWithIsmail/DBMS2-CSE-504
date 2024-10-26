#include <bits/stdc++.h>
#include <sstream>
using namespace std;
set<string> values, done, redo, dataEntries;
vector<vector<string>> v;
map<string, vector<string>> mp;
map<string, string> finalValue;
regex pattern("T\\d+");
set<string> transactions;
int start = 0;
void lastckpt()
{
    int i;
    for (i = v.size() - 1; i >= 0; i--)
    {
        if (v[i][0] == "CKPT")
        {
            auto words_begin = sregex_iterator(v[i][1].begin(), v[i][1].end(), pattern);
            auto words_end = sregex_iterator();
            for (sregex_iterator i = words_begin; i != words_end; i++)
                transactions.insert((*i).str());
            cout << v[i][1] << "\n";
            break;
        }
    }
}
void startindex()
{
    int i;
    for (i = 0; i < v.size(); i++)
    {
        if (v[i][0] == "START")
        {
            if (transactions.find(v[i][1]) != transactions.end())
            {
                start = i;
                break;
            }
        }
    }
}
void preprocessing()
{
    ifstream file("log.txt");
    string line;
    while (getline(file, line))
    {
        line.pop_back();
        line = line.substr(1);
        vector<string> current;
        istringstream word(line);
        while (word)
        {
            string temp;
            word >> temp;
            current.push_back(temp);
        }
        current.pop_back();
        v.push_back(current);
    }
}
void undo_redo_find()
{
    bool checkpoint = false;
    int i;
    for (i = start; i < v.size(); i++)
    {
        if (v[i].size() == 2)
        {
            if (v[i][0] == "COMMIT")
            {
                if (!checkpoint)
                {
                    done.insert(v[i][1]);
                    values.erase(v[i][1]);
                }
                else
                {
                    redo.insert(v[i][1]);
                    values.erase(v[i][1]);
                }
            }
            if (v[i][0] == "CKPT")
                checkpoint = true;
            // if (v[i][0] == "END")
            //     checkpoint = false;
        }
        else if (v[i].size() == 4)
        {
            mp[v[i][0]] = {v[i][1], v[i][2], v[i][3]};
            dataEntries.insert(v[i][1]);
            values.insert(v[i][0]);
        }
    }
}
void valueUpdate()
{
    for (auto x : done)
        finalValue[mp[x][0]] = mp[x][2];
    for (auto x : redo)
        finalValue[mp[x][0]] = mp[x][2];
    for (auto x : values)
        finalValue[mp[x][0]] = mp[x][1];
}
int main()
{
    preprocessing();
    for (int i = v.size() - 1; i >= 0; i--)
    {
        for (auto x : v[i])
            cout << x << " ";
        cout << "\n";
    }
    lastckpt();
    for (auto x : transactions)
        cout << x << " ";
    cout << "\n";

    startindex();
    undo_redo_find();
    valueUpdate();

    cout << start << "\n";

    // for (auto x : transactions)
    //     cout << x << " ";
    // cout << "\n";

    cout << "undo: ";
    for (auto x : values)
        cout << x << " ";
    cout << "\nredo: ";
    for (auto x : redo)
        cout << x << " ";
    cout << "\nFinal value:\n";
    for (auto x : dataEntries)
        cout << x << " : " << finalValue[x] << "\n";
}