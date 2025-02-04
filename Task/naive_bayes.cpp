#include <bits/stdc++.h>
using namespace std;
typedef vector<int> vi;
#define pb push_back

vi labels;
vector<vi> features;
vector<string> keywords = {"free", "buy", "link", "urgent", "limited"};
map<pair<int, int>, double> conditional_p;
map<int, double> prior_p;

bool containWord(string text, string word)
{
    return text.find(word) != string::npos;
}
int labelToNumeric(string label)
{
    return (label == "spam") ? 1 : 0;
}
string toLowerMessage(string text)
{
    transform(text.begin(), text.end(), text.begin(), ::tolower);
    return text;
}
vi extractFeatures(string text)
{
    vi feature;
    for (auto keyword : keywords)
    {
        feature.pb(containWord(text, keyword) ? 1 : 0);
    }
    return feature;
}
void readInput()
{
    ifstream input("naive_bayes_input.txt");
    string line;
    while (getline(input, line))
    {
        int label_pos = line.find("label:");
        string label = line.substr(label_pos + 7);
        string message = line.substr(0, label_pos);
        message = toLowerMessage(message);
        features.pb(extractFeatures(message));
        labels.pb(labelToNumeric(label));
    }
}
void NaiveBayesTrain()
{
    int yes = count(labels.begin(), labels.end(), 1);
    prior_p[1] = (double)yes / labels.size();
    prior_p[0] = 1.0 - prior_p[1];

    for (int label = 0; label < 2; label++)
    {
        for (int key = 0; key < keywords.size(); key++)
        {
            for (int feature = 0; feature < features.size(); feature++)
            {
                if (features[feature][key] == 1)
                    conditional_p[{label, key}]++;
            }
        }
    }
    for (int label = 0; label < 2; label++)
    {
        for (int key = 0; key < keywords.size(); key++)
        {
            conditional_p[{label, key}] /= prior_p[label];
        }
    }
}
int main()
{
    readInput();
    NaiveBayesTrain();

    string test = "Are you available for a quick chat later today?";
}