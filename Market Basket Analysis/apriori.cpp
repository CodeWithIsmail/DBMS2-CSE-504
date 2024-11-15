#include <bits/stdc++.h>
using namespace std;
string fname = "data1.txt";
map<string, set<string>> transaction;
set<set<string>> itemPair;
int supportCount;
int countSearch = 0;

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
set<set<string>> generateSubsets(set<string> &inputSet)
{
    set<set<string>> subsets;
    vector<string> elements(inputSet.begin(), inputSet.end());
    for (int i = 0; i < elements.size(); i++)
    {
        set<string> subset;
        for (int j = 0; j < elements.size(); ++j)
        {
            if (j != i)
                subset.insert(elements[j]);
        }
        subsets.insert(subset);
    }
    return subsets;
}
void printSet(const set<string> &st)
{
    cout << "\n";
    for (auto y : st)
        cout << y << " ";
}
void printSetSet(const set<set<string>> &st)
{
    for (auto x : st)
        printSet(x);
}
void apriori()
{
    int itemCount = 1;

    while (itemPair.size())
    {
        // cout << "\n\nL" << itemCount;
        set<set<string>> tempItemPair;
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
                    if (itemCount == 1)
                        tempItemPair.insert(unionSet);
                    else
                    {
                        set<set<string>> allPairs = generateSubsets(unionSet);
                        bool nibo = true;
                        for (auto all : allPairs)
                        {
                            if (itemPair.find(all) == itemPair.end())
                            {
                                nibo = false;
                                break;
                            }
                        }
                        if (nibo)
                            tempItemPair.insert(unionSet);
                    }
                }
            }
        }
        itemPair.clear();
        for (auto newset : tempItemPair)
        {
            int counter = 0;
            for (auto mainset : transaction)
            {
                countSearch++;
                if (includes(mainset.second.begin(), mainset.second.end(), newset.begin(), newset.end()))
                    counter++;
            }
            if (counter >= supportCount)
            {
                itemPair.insert(newset);
                printSet(newset);
                cout << " : " << counter;
            }
        }
        // printSetSet(itemPair);
        itemCount++;
    }
}
int main()
{
    inputProcess();
    apriori();
}