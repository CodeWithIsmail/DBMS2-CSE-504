# import random
# import math
# import numpy as np

# NO_OF_CLUSTER = 3
# MAX_ITER = 100
# CLUSTER = [[] for _ in range(NO_OF_CLUSTER)]
# CENTROID = []
# POINTS = []


# def read_input():
#     global POINTS
#     with open("k_means_input.txt", "r") as file:
#         for line in file:
#             entry = list(map(float, line.strip().split(',')))
#             POINTS.append(entry)


# def init():
#     global CENTROID
#     random.seed()
#     CENTROID = random.sample(POINTS, NO_OF_CLUSTER)


# def print_points(points):
#     for point in points:
#         print(" ".join(map(str, point)))
#     print()


# def print_clusters():
#     for i, cluster in enumerate(CLUSTER):
#         print(f"Cluster {i + 1}:")
#         print_points(cluster)


# def euclidean_distance(point1, point2):
#     return math.sqrt(sum((x - y) ** 2 for x, y in zip(point1, point2)))


# def calculate_centroid(cluster):
#     if not cluster:
#         return []
#     return np.mean(cluster, axis=0).tolist()


# def compare_centroid(a, b):
#     return all(abs(x - y) <= 1e-6 for x, y in zip(a, b))


# def k_means_clustering():
#     global CLUSTER, CENTROID
#     for iteration in range(MAX_ITER):
#         CLUSTER = [[] for _ in range(NO_OF_CLUSTER)]

#         for point in POINTS:
#             min_distance = float("inf")
#             closest_centroid = -1
#             for i in range(NO_OF_CLUSTER):
#                 dist = euclidean_distance(CENTROID[i], point)
#                 if dist < min_distance:
#                     min_distance = dist
#                     closest_centroid = i
#             CLUSTER[closest_centroid].append(point)

#         converged = True
#         for i in range(NO_OF_CLUSTER):
#             new_centroid = calculate_centroid(CLUSTER[i])
#             if not compare_centroid(new_centroid, CENTROID[i]):
#                 converged = False
#                 CENTROID[i] = new_centroid

#         if converged:
#             print(f"Converged after {iteration + 1} iterations.")
#             break


# def print_result():
#     print_clusters()
#     print("Final Centroids:")
#     print_points(CENTROID)


# def main():
#     read_input()
#     print("Given points:")
#     print_points(POINTS)
#     init()
#     print("Initial centroids:")
#     print_points(CENTROID)
#     k_means_clustering()
#     print_result()


# if __name__ == "__main__":
#     main()

import random
import math
import numpy as np
import cv2
import matplotlib.pyplot as plt

NO_OF_CLUSTER = 3  # Number of colors/clusters
MAX_ITER = 100
CLUSTER = [[] for _ in range(NO_OF_CLUSTER)]
CENTROID = []
PIXELS = []

def read_image(image_path):
    """Reads an image and converts it into a list of pixel values."""
    global PIXELS, img_shape
    img = cv2.imread(image_path)  # Read image
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # Convert BGR to RGB
    img_shape = img.shape  # Store original shape for reconstruction
    PIXELS = img.reshape(-1, 3).tolist()  # Flatten image to list of RGB pixels

def init():
    """Initialize centroids randomly from the pixels."""
    global CENTROID
    random.seed()
    CENTROID = random.sample(PIXELS, NO_OF_CLUSTER)

def euclidean_distance(p1, p2):
    return math.sqrt(sum((x - y) ** 2 for x, y in zip(p1, p2)))

def calculate_centroid(cluster):
    if not cluster:
        return []
    return np.mean(cluster, axis=0).tolist()

def compare_centroid(a, b):
    return all(abs(x - y) <= 1e-6 for x, y in zip(a, b))

def k_means_clustering():
    """Performs K-Means clustering on pixel values."""
    global CLUSTER, CENTROID
    for iteration in range(MAX_ITER):
        CLUSTER = [[] for _ in range(NO_OF_CLUSTER)]
        
        # Assign each pixel to the closest centroid
        for pixel in PIXELS:
            min_distance = float("inf")
            closest_centroid = -1
            for i in range(NO_OF_CLUSTER):
                dist = euclidean_distance(CENTROID[i], pixel)
                if dist < min_distance:
                    min_distance = dist
                    closest_centroid = i
            CLUSTER[closest_centroid].append(pixel)
        
        # Recalculate centroids
        converged = True
        for i in range(NO_OF_CLUSTER):
            new_centroid = calculate_centroid(CLUSTER[i])
            if not compare_centroid(new_centroid, CENTROID[i]):
                converged = False
                CENTROID[i] = new_centroid
        
        if converged:
            print(f"Converged after {iteration + 1} iterations.")
            break

def reconstruct_image():
    """Reconstructs the image based on clustered pixel values."""
    clustered_pixels = []
    
    # Assign each pixel to its cluster's centroid
    for pixel in PIXELS:
        distances = [euclidean_distance(pixel, centroid) for centroid in CENTROID]
        closest_cluster = distances.index(min(distances))
        clustered_pixels.append(CENTROID[closest_cluster])
    
    # Convert list back to NumPy array and reshape it to original image shape
    clustered_img = np.array(clustered_pixels, dtype=np.uint8).reshape(img_shape)
    
    # Display the original and clustered images
    plt.figure(figsize=(10,5))
    plt.subplot(1,2,1)
    plt.imshow(np.array(PIXELS, dtype=np.uint8).reshape(img_shape))
    plt.title("Original Image")
    plt.axis("off")
    
    plt.subplot(1,2,2)
    plt.imshow(clustered_img)
    plt.title(f"K-Means Clustered (K={NO_OF_CLUSTER})")
    plt.axis("off")
    
    plt.show()

def main():
    image_path = "your_image.jpg"  # Change to your image path
    read_image(image_path)
    init()
    k_means_clustering()
    reconstruct_image()

if __name__ == "__main__":
    main()
