#include <bits/stdc++.h>
using namespace std;
#define pb push_back
string fname = "data1.txt";
map<string, int> itemCount;
map<string, set<string>> transaction;
set<set<string>> itemPair;
int supportCount;
void inputProcess()
{
    string line, temp;
    ifstream file(fname);
    while (getline(file, line))
    {
        stringstream ss(line);
        string word, tname = "";
        while (getline(ss, word, ','))
        {
            if (tname == "")
                tname = word;
            else
            {
                transaction[tname].insert(word);
                itemPair.insert({{word}});
            }
        }
        temp = line;
    }
    supportCount = stoi(temp);
}
void printTransaction()
{
    int i;
    for (auto x : transaction)
    {
        cout << x.first << " : ";
        for (auto y : x.second)
        {
            cout << y << " ";
        }
        cout << "\n";
    }
}

void candidateGen()
{
    int itemCount = 1;
    while (1)
    {
        set<set<string>> tempItemPair;
        for (auto x : itemPair)
        {
            for (auto y : x)
                cout << y << " ";
            cout << "\n";
        }

        for (auto x : itemPair)
        {
            for (auto y : itemPair)
            {
                set<string> unionSet;

                set_union(x.begin(), x.end(),
                          y.begin(), y.end(),
                          inserter(unionSet, unionSet.begin()));
                if (unionSet.size() == itemCount)
                {
                    tempItemPair.insert({unionSet});
                    // for (auto z : unionSet)
                    // {
                    //     cout << z << " ";

                    // }
                }
                // cout << "\n";
            }
        }
        itemPair.clear();
        for (auto newset : tempItemPair)
        {
            int counter = 0;
            for (auto mainset : transaction)
            {
                if (includes(mainset.second.begin(), mainset.second.end(), newset.begin(), newset.end()))
                    counter++;
            }

            if (counter >= supportCount)
            {
                itemPair.insert(newset);

                for (auto y : newset)
                    cout << y << " ";
                cout << " : ";
                cout << counter << "\n";
            }
        }
        itemCount++;

        if (itemCount == 5)
            break;
    }
}
int main()
{
    inputProcess();
    // printTransaction();
    // c1Generate();
    candidateGen();
}