#include <bits/stdc++.h>
using namespace std;

#define pb push_back
typedef vector<double> vd;
#define NO_OF_CLUSTER 3
vector<vector<vd>> CLUSTER(NO_OF_CLUSTER, vector<vd>{});
vector<vd> CENTROID, POINTS;

#define MAX_ITER 100

void readInput()
{
    ifstream input("k_means_input.txt");
    string line;
    while (getline(input, line))
    {
        vd entry;
        stringstream ss(line);
        string val;
        while (getline(ss, val, ','))
            entry.pb(stod(val));
        POINTS.pb(entry);
    }
}
void init()
{
    srand(time(0));
    for (int i = 0; i < NO_OF_CLUSTER; i++)
        CENTROID.pb(POINTS[rand() % POINTS.size()]);
}
void print(const vector<vd> &v)
{
    for (const auto &entry : v)
    {
        for (const auto &x : entry)
            cout << x << " ";
        cout << endl;
    }
    cout << "\n";
}
void printv(const vector<vector<vd>> &clusters)
{
    for (int i = 0; i < clusters.size(); i++)
    {
        cout << "Cluster " << i + 1 << ":\n";
        print(clusters[i]);
    }
}
double distance(const vd &point1, const vd &point2)
{
    double dis = 0;
    for (int i = 0; i < point1.size(); i++)
        dis += pow(point2[i] - point1[i], 2);
    return sqrt(dis);
}
vd calculateCentroid(const vector<vd> &cluster)
{
    if (cluster.empty())
        return {};
    int dimension = cluster[0].size();
    vd centroid(dimension, 0.0);
    for (const auto &point : cluster)
    {
        for (int i = 0; i < dimension; i++)
        {
            centroid[i] += point[i];
        }
    }
    for (int i = 0; i < dimension; i++)
        centroid[i] /= cluster.size();
    return centroid;
}
bool compareCentroid(const vd &a, const vd &b)
{
    for (int i = 0; i < a.size(); i++)
    {
        if (fabs(a[i] - b[i]) > 1e-6)
            return false;
    }
    return true;
}
void KMeansClustering()
{
    for (int iter = 0; iter < MAX_ITER; iter++)
    {
        CLUSTER.assign(NO_OF_CLUSTER, vector<vd>{});

        for (const auto &x : POINTS)
        {
            double min_distance = numeric_limits<double>::max();
            int closest_centroid = -1;
            for (int i = 0; i < NO_OF_CLUSTER; i++)
            {
                double temp = distance(CENTROID[i], x);
                if (temp < min_distance)
                {
                    min_distance = temp;
                    closest_centroid = i;
                }
            }
            CLUSTER[closest_centroid].pb(x);
        }

        bool converged = true;
        for (int i = 0; i < NO_OF_CLUSTER; i++)
        {
            vd new_centroid = calculateCentroid(CLUSTER[i]);
            if (!compareCentroid(new_centroid, CENTROID[i]))
            {
                converged = false;
                CENTROID[i] = new_centroid;
            }
        }

        if (converged)
        {
            cout << "Converged after " << iter + 1 << " iterations.\n";
            break;
        }
    }
    
}
void printResult()
{
    printv(CLUSTER);
    cout << "Final Centroids:\n";
    print(CENTROID);
}
int main()
{
    readInput();
    cout << "Given points:\n";
    print(POINTS);
    init();
    cout << "Initial centroids:\n";
    print(CENTROID);
    KMeansClustering();
    printResult();
}
