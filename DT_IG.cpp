#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
#include <random>
#include <string>

using namespace std;

struct DecisionTreeNode
{
    int attribute_index;
    double threshold;
    string label;
    DecisionTreeNode *left;
    DecisionTreeNode *right;

    DecisionTreeNode(int attribute_index = -1, double threshold = 0.0, const string &label = "", DecisionTreeNode *left = nullptr, DecisionTreeNode *right = nullptr)
        : attribute_index(attribute_index), threshold(threshold), label(label), left(left), right(right) {}
};

typedef pair<vector<double>, string> DataPoint;
typedef vector<DataPoint> Dataset;

Dataset load_iris_data(const string &filename)
{
    Dataset data;
    ifstream file(filename);
    string line;

    while (getline(file, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);
        vector<double> features;
        string value, label;

        for (int i = 0; i < 4; ++i)
        {
            getline(iss, value, ',');
            features.push_back(stod(value));
        }
        getline(iss, label, ',');
        data.emplace_back(features, label);
    }

    return data;
}

double calculate_entropy(const Dataset &data)
{
    map<string, int> label_counts;
    for (const auto &row : data)
    {
        label_counts[row.second]++;
    }

    double entropy = 0.0;
    int total_samples = data.size();

    for (const auto &[label, count] : label_counts)
    {
        double probability = static_cast<double>(count) / total_samples;
        entropy -= probability * log2(probability);
    }

    return entropy;
}

pair<Dataset, Dataset> split_data(const Dataset &data, int attribute_index, double threshold)
{
    Dataset left, right;

    for (const auto &row : data)
    {
        if (row.first[attribute_index] <= threshold)
        {
            left.push_back(row);
        }
        else
        {
            right.push_back(row);
        }
    }

    return {left, right};
}

double calculate_information_gain(const Dataset &data, int attribute_index, double threshold)
{
    double parent_entropy = calculate_entropy(data);
    auto [left, right] = split_data(data, attribute_index, threshold);

    double left_weight = static_cast<double>(left.size()) / data.size();
    double right_weight = static_cast<double>(right.size()) / data.size();

    double weighted_entropy = left_weight * calculate_entropy(left) + right_weight * calculate_entropy(right);

    return parent_entropy - weighted_entropy;
}

tuple<int, double, double> find_best_split(const Dataset &data)
{
    double best_gain = 0.0;
    int best_attribute = -1;
    double best_threshold = 0.0;

    int n_features = data[0].first.size();

    for (int attribute_index = 0; attribute_index < n_features; ++attribute_index)
    {
        set<double> values;
        for (const auto &row : data)
        {
            values.insert(row.first[attribute_index]);
        }

        for (double threshold : values)
        {
            double gain = calculate_information_gain(data, attribute_index, threshold);
            if (gain > best_gain)
            {
                best_gain = gain;
                best_attribute = attribute_index;
                best_threshold = threshold;
            }
        }
    }

    return {best_attribute, best_threshold, best_gain};
}

DecisionTreeNode *build_decision_tree(const Dataset &data, int depth = 0)
{
    map<string, int> label_counts;
    for (const auto &row : data)
    {
        label_counts[row.second]++;
    }

    if (label_counts.size() == 1)
    {
        return new DecisionTreeNode(-1, 0.0, data[0].second);
    }

    if (data.empty())
    {
        return nullptr;
    }

    auto [best_attribute, best_threshold, best_gain] = find_best_split(data);

    if (best_gain == 0.0)
    {
        string majority_label;
        int max_count = 0;

        for (const auto &[label, count] : label_counts)
        {
            if (count > max_count)
            {
                max_count = count;
                majority_label = label;
            }
        }

        return new DecisionTreeNode(-1, 0.0, majority_label);
    }

    auto [left_data, right_data] = split_data(data, best_attribute, best_threshold);
    DecisionTreeNode *left_subtree = build_decision_tree(left_data, depth + 1);
    DecisionTreeNode *right_subtree = build_decision_tree(right_data, depth + 1);

    return new DecisionTreeNode(best_attribute, best_threshold, "", left_subtree, right_subtree);
}

string predict(DecisionTreeNode *tree, const vector<double> &sample)
{
    if (tree->label != "")
    {
        return tree->label;
    }

    if (sample[tree->attribute_index] <= tree->threshold)
    {
        return predict(tree->left, sample);
    }
    else
    {
        return predict(tree->right, sample);
    }
}

double evaluate(DecisionTreeNode *tree, const Dataset &test_data)
{
    int correct = 0;

    for (const auto &[sample, label] : test_data)
    {
        if (predict(tree, sample) == label)
        {
            correct++;
        }
    }

    return static_cast<double>(correct) / test_data.size();
}

void print_tree(DecisionTreeNode *node, int depth = 0)
{
    if (!node)
        return;

    for (int i = 0; i < depth; ++i)
        cout << "  ";

    if (node->label != "")
    {
        cout << "Leaf: " << node->label << "\n";
    }
    else
    {
        cout << "Attribute " << node->attribute_index << " <= " << node->threshold << "?\n";
        for (int i = 0; i < depth; ++i)
            cout << "  ";
        cout << "Left:\n";
        print_tree(node->left, depth + 1);
        for (int i = 0; i < depth; ++i)
            cout << "  ";
        cout << "Right:\n";
        print_tree(node->right, depth + 1);
    }
}

int main()
{
    Dataset data = load_iris_data("iris.data");

    shuffle(data.begin(), data.end(), mt19937{random_device{}()});

    Dataset train_data(data.begin(), data.begin() + 120);
    Dataset test_data(data.begin() + 120, data.end());

    DecisionTreeNode *decision_tree = build_decision_tree(train_data);

    cout << "Decision Tree Structure:\n";
    print_tree(decision_tree);

    double accuracy = evaluate(decision_tree, test_data);
    cout << "Accuracy: " << accuracy * 100 << "%\n";

    return 0;
}
