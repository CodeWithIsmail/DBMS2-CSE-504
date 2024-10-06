#include <bits/stdc++.h>
#include <sstream>

using namespace std;
set<string> values;
set<string> done;
set<string> redo;
vector<vector<string>> v;

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
        // for (auto y : x)
        //     cout << y << " ";
        // cout << "\n";
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
            values.insert(x[0]);
        }
    }
    cout << values.size();
}
int main()
{
    preprocessing();
    undo_redo_find();
    cout << "undo: ";
    for (auto x : values)
        cout << x << " ";
    cout << "\nredo: ";
    for (auto x : redo)
        cout << x << " ";
}