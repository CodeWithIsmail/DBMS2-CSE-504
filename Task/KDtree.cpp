#include <bits/stdc++.h>
using namespace std;
typedef vector<double> vd;

struct KDNode
{
    vd coords;
    KDNode *left;
    KDNode *right;
    KDNode(vd coords) : coords(coords), left(nullptr), right(nullptr) {}
};

KDNode *buildKDTree(vector<vd> points, int depth, int k)
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

// Find minimum node on a given axis
KDNode *findMin(KDNode *node, int depth, int k, int axis)
{
    if (!node)
        return nullptr;

    if (depth % k == axis)
    {
        if (!node->left)
            return node;
        return findMin(node->left, depth + 1, k, axis);
    }

    KDNode *leftMin = findMin(node->left, depth + 1, k, axis);
    KDNode *rightMin = findMin(node->right, depth + 1, k, axis);

    KDNode *minNode = node;
    if (leftMin && leftMin->coords[axis] < minNode->coords[axis])
        minNode = leftMin;
    if (rightMin && rightMin->coords[axis] < minNode->coords[axis])
        minNode = rightMin;
    return minNode;
}

// Delete node from KD-Tree
KDNode *deleteNode(KDNode *node, vd point, int depth, int k)
{
    if (!node)
        return nullptr;

    int axis = depth % k;
    if (node->coords == point)
    {
        if (node->right)
        {
            KDNode *minNode = findMin(node->right, depth + 1, k, axis);
            node->coords = minNode->coords;
            node->right = deleteNode(node->right, minNode->coords, depth + 1, k);
        }
        else if (node->left)
        {
            KDNode *minNode = findMin(node->left, depth + 1, k, axis);
            node->coords = minNode->coords;
            node->right = deleteNode(node->left, minNode->coords, depth + 1, k);
            node->left = nullptr;
        }
        else
        {
            delete node;
            return nullptr;
        }
    }
    else if (point[axis] < node->coords[axis])
        node->left = deleteNode(node->left, point, depth + 1, k);
    else
        node->right = deleteNode(node->right, point, depth + 1, k);
    return node;
}

// Nearest neighbor search
void nearestNeighbor(KDNode *node, vd target, int depth, int k, KDNode *&best, double &bestDist)
{
    if (!node)
        return;

    double dist = 0;
    for (int i = 0; i < k; i++)
        dist += (node->coords[i] - target[i]) * (node->coords[i] - target[i]);

    if (dist < bestDist)
    {
        bestDist = dist;
        best = node;
    }

    int axis = depth % k;
    KDNode *nextBranch = target[axis] < node->coords[axis] ? node->left : node->right;
    KDNode *otherBranch = target[axis] < node->coords[axis] ? node->right : node->left;

    nearestNeighbor(nextBranch, target, depth + 1, k, best, bestDist);

    if ((target[axis] - node->coords[axis]) * (target[axis] - node->coords[axis]) < bestDist)
        nearestNeighbor(otherBranch, target, depth + 1, k, best, bestDist);
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

    // Delete a node
    cout << "\nDeleting node (6,12)\n";
    root = deleteNode(root, {6, 12}, 0, k);
    printInOrder(root);

    // Nearest neighbor search
    vd target = {10, 10};
    KDNode *best = nullptr;
    double bestDist = DBL_MAX;
    nearestNeighbor(root, target, 0, k, best, bestDist);

    cout << "\nNearest Neighbor to (10,10): ";
    for (double coord : best->coords)
        cout << coord << " ";
    cout << "\n";
}