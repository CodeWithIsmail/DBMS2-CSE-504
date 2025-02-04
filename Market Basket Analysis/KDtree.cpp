#include <bits/stdc++.h>

using namespace std;

// Node of the k-d tree
struct KDNode
{
    vector<double> coords; // Coordinates of the point
    KDNode *left;
    KDNode *right;

    KDNode(vector<double> coords) : coords(coords), left(nullptr), right(nullptr) {}
};

// Build the k-d tree recursively
KDNode *buildKDTree(vector<vector<double>> points, int depth, int k)
{
    if (points.empty())
        return nullptr;

    int axis = depth % k;
    int median = points.size() / 2;

    sort(points.begin(), points.end(), [axis](const vector<double> &a, const vector<double> &b)
         { return a[axis] < b[axis]; });

    // // Sort points by the current axis
    // nth_element(points.begin(), points.begin() + median, points.end(),
    //             [axis](const vector<double> &a, const vector<double> &b)
    //             {
    //                 return a[axis] < b[axis];
    //             });

    // Create node and recursively build subtrees
    KDNode *node = new KDNode(points[median]);
    node->left = buildKDTree(vector<vector<double>>(points.begin(), points.begin() + median), depth + 1, k);
    node->right = buildKDTree(vector<vector<double>>(points.begin() + median + 1, points.end()), depth + 1, k);

    return node;
}

// Insert a new point into the k-d tree
KDNode *insertKDNode(KDNode *node, vector<double> point, int depth, int k)
{
    if (node == nullptr)
    {
        return new KDNode(point);
    }

    int axis = depth % k;
    if (point[axis] < node->coords[axis])
    {
        node->left = insertKDNode(node->left, point, depth + 1, k);
    }
    else
    {
        node->right = insertKDNode(node->right, point, depth + 1, k);
    }

    return node;
}

// Print the k-d tree (inorder traversal)
void printInOrder(KDNode *node)
{
    if (node == nullptr)
        return;
    printInOrder(node->left);
    for (double coord : node->coords)
    {
        cout << coord << " ";
    }
    cout << endl;
    printInOrder(node->right);
}

// Delete the k-d tree (to free memory)
void deleteKDTree(KDNode *node)
{
    if (node == nullptr)
        return;
    deleteKDTree(node->left);
    deleteKDTree(node->right);
    delete node;
}

int main()
{
    // Example points in 2D space
    vector<vector<double>> points = {
        {3, 6},
        {17, 15},
        {13, 15},
        {6, 12},
        {9, 1},
        {2, 7},
        {10, 19}};

    // Determine dimensionality of points
    int k = points[0].size();

    // Build the k-d tree
    KDNode *root = buildKDTree(points, 0, k);

    // Print the initial tree
    cout << "Initial KD Tree (Inorder Traversal):" << endl;
    printInOrder(root);

    // Insert a new point
    vector<double> newPoint = {8, 14};
    root = insertKDNode(root, newPoint, 0, k);

    // Print the tree after insertion
    cout << "\nKD Tree after inserting point (8, 14):" << endl;
    printInOrder(root);

    // Clean up memory
    deleteKDTree(root);

    return 0;
}
