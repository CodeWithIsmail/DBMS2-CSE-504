#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define pb push_back
typedef vector<double> vd;
#define NO_OF_CLUSTER 3  // Adjust based on skin tone variations
vector<vector<vd>> CLUSTER(NO_OF_CLUSTER, vector<vd>{});
vector<vd> CENTROID, POINTS;
#define MAX_ITER 100

void readImage(string filename) {
    Mat img = imread(filename);  // Load image
    if (img.empty()) {
        cout << "Could not open or find the image!\n";
        return;
    }
    
    // Resize image for faster processing (optional)
    resize(img, img, Size(300, 300));

    // Convert to RGB
    cvtColor(img, img, COLOR_BGR2RGB);
    
    // Store pixels as data points
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            Vec3b pixel = img.at<Vec3b>(i, j);
            vd entry = {double(pixel[0]), double(pixel[1]), double(pixel[2])}; // R, G, B
            POINTS.pb(entry);
        }
    }
}

void init() {
    srand(time(0));
    for (int i = 0; i < NO_OF_CLUSTER; i++)
        CENTROID.pb(POINTS[rand() % POINTS.size()]);
}

double distance(const vd &point1, const vd &point2) {
    double dis = 0;
    for (int i = 0; i < point1.size(); i++)
        dis += pow(point2[i] - point1[i], 2);
    return sqrt(dis);
}

vd calculateCentroid(const vector<vd> &cluster) {
    if (cluster.empty()) return {};
    int dimension = cluster[0].size();
    vd centroid(dimension, 0.0);
    for (const auto &point : cluster) {
        for (int i = 0; i < dimension; i++) {
            centroid[i] += point[i];
        }
    }
    for (int i = 0; i < dimension; i++)
        centroid[i] /= cluster.size();
    return centroid;
}

bool compareCentroid(const vd &a, const vd &b) {
    for (int i = 0; i < a.size(); i++) {
        if (fabs(a[i] - b[i]) > 1e-6)
            return false;
    }
    return true;
}

void KMeansClustering() {
    for (int iter = 0; iter < MAX_ITER; iter++) {
        CLUSTER.assign(NO_OF_CLUSTER, vector<vd>{});

        for (const auto &x : POINTS) {
            double min_distance = numeric_limits<double>::max();
            int closest_centroid = -1;
            for (int i = 0; i < NO_OF_CLUSTER; i++) {
                double temp = distance(CENTROID[i], x);
                if (temp < min_distance) {
                    min_distance = temp;
                    closest_centroid = i;
                }
            }
            CLUSTER[closest_centroid].pb(x);
        }

        bool converged = true;
        for (int i = 0; i < NO_OF_CLUSTER; i++) {
            vd new_centroid = calculateCentroid(CLUSTER[i]);
            if (!compareCentroid(new_centroid, CENTROID[i])) {
                converged = false;
                CENTROID[i] = new_centroid;
            }
        }

        if (converged) {
            cout << "Converged after " << iter + 1 << " iterations.\n";
            break;
        }
    }
}

void applySkinMask(string filename) {
    Mat img = imread(filename);
    resize(img, img, Size(300, 300));
    cvtColor(img, img, COLOR_BGR2RGB);

    Mat skinMask = Mat::zeros(img.size(), CV_8UC1);

    // Find the skin cluster (highest red component)
    int skinCluster = 0;
    double maxRed = 0;
    for (int i = 0; i < NO_OF_CLUSTER; i++) {
        if (CENTROID[i][0] > maxRed) { // Comparing Red channel
            maxRed = CENTROID[i][0];
            skinCluster = i;
        }
    }

    // Apply mask
    int index = 0;
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++, index++) {
            if (CLUSTER[skinCluster][index][0] == POINTS[index][0] &&
                CLUSTER[skinCluster][index][1] == POINTS[index][1] &&
                CLUSTER[skinCluster][index][2] == POINTS[index][2]) {
                skinMask.at<uchar>(i, j) = 255;  // Keep skin (white mask)
            }
        }
    }

    // Create output image
    Mat result;
    img.copyTo(result, skinMask);
    for (int i = 0; i < result.rows; i++) {
        for (int j = 0; j < result.cols; j++) {
            if (skinMask.at<uchar>(i, j) == 0) {
                result.at<Vec3b>(i, j) = Vec3b(255, 255, 255);  // Non-skin areas white
            }
        }
    }

    // Display results
    imshow("Original Image", img);
    imshow("Skin Detection", result);
    waitKey(0);
    destroyAllWindows();
}

int main() {
    string filename = "face.jpg";
    readImage(filename);

    cout << "Processing Image...\n";
    init();
    KMeansClustering();
    applySkinMask(filename);

    return 0;
}
