#include <bits/stdc++.h>
using namespace std;
typedef vector<double> vd;

struct KDNode
{
    vd coords;
    KDNode *left;
    KDNode *right;
    KDNode(vd coords) : coords(coords), left(nullptr), right(nullptr) {}
}

KDNode *
buildKDTree(vector<vd> points, int depth, int k)
{
    if (points.empty())
        return nullptr;

    int axis = depth % k;
    int median = points.size() / 2;

    sort(points.begin(), points.end(), [axis](const vd &a, const vd &b)
         { return a[axis] < b[axis]; });

    KDNode *node = new KDNode(points[median]);
    node->left = buildKDTree(vector<vd>(points.begin(), points.begin() + median), depth + 1, k);
    node->right = buildKDTree(vector<vd>(points.begin() + median + 1, points.end()), depth + 1, k);

    return node;
}

void printInOrder(KDNode *node)
{
    if (node == nullptr)
        return;

    printInOrder(node->left);
    for (auto coord : node->coords)
        cout << coord << " , ";
    cout << "\n";
    printInOrder(node->right);
}
int main()
{
    vector<vd> points = {
        {3, 6},
        {17, 15},
        {13, 15},
        {6, 12},
        {9, 1},
        {2, 7},
        {10, 19}};

    int k = points[0].size();
    KDNode *root = buildKDTree(points, 0, k);
    cout << "Initial KD Tree (Inorder Traversal):\n";
    printInOrder(root);
}