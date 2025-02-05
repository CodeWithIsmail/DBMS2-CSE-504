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
    int median = (points.size() - 1) / 2;

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
    ifstream file("input.txt");
    vector<vd> points;
    string line;

    while (getline(file, line))
    {
        stringstream ss(line);
        string coord;
        vd point;
        while (getline(ss, coord, ','))
        {
            point.push_back(stod(coord));
        }
        points.push_back(point);
    }

    int k = points[0].size();
    KDNode *root = buildKDTree(points, 0, k);
    cout << "Initial KD Tree (Inorder Traversal):\n";
    printInOrder(root);

    cout << "\nDeleting node (3,4,5)\n";
    root = deleteNode(root, {3, 4, 5}, 0, k);
    printInOrder(root);

    vd target = {10, 4, 7};
    KDNode *best = nullptr;
    double bestDist = DBL_MAX;
    nearestNeighbor(root, target, 0, k, best, bestDist);

    cout << "\nNearest Neighbor to (10, 4, 7): ";
    for (double coord : best->coords)
        cout << coord << " ";
    cout << "\n";
}
