#include <bits/stdc++.h>
#include <sstream>
using namespace std;
set<string> values, done, redo, dataEntries;
vector<vector<string>> v;
map<string, vector<string>> mp;
map<string, string> finalValue;

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
    for (auto x : v)
    {
        if (x.size() == 2)
        {
            if (x[0] == "COMMIT")
            {
                if (!checkpoint)
                {
                    done.insert(x[1]);
                    values.erase(x[1]);
                }
                else
                {
                    redo.insert(x[1]);
                    values.erase(x[1]);
                }
            }
            if (x[0] == "CKPT")
                checkpoint = true;
        }
        else if (x.size() == 4)
        {
            mp[x[0]] = {x[1], x[2], x[3]};
            dataEntries.insert(x[1]);
            values.insert(x[0]);
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
    undo_redo_find();
    valueUpdate();

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