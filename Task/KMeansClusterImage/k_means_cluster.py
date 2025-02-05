import random
import math
import numpy as np
import cv2

NO_OF_CLUSTER = 3
MAX_ITER = 10
CLUSTER = []
for i in range(NO_OF_CLUSTER):
    CLUSTER.append([])

CENTROID = []
PIXELS = []
LABELS = []


def read_image(image_path):
    global PIXELS, img_shape
    img = cv2.imread(image_path)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img_shape = img.shape
    PIXELS = []
    for row in img:
        for pixel in row:
            PIXELS.append(pixel.tolist())


def init():
    global CENTROID
    random.seed()
    CENTROID = []
    selected_pixels = random.sample(PIXELS, NO_OF_CLUSTER)
    for pixel in selected_pixels:
        CENTROID.append(pixel)


def euclidean_distance(p1, p2):
    total = 0
    for i in range(len(p1)):
        total += (p1[i] - p2[i]) ** 2
    return math.sqrt(total)


def calculate_centroid(cluster):
    if len(cluster) == 0:
        return []

    sum_r = 0
    sum_g = 0
    sum_b = 0
    count = len(cluster)

    for pixel in cluster:
        sum_r += pixel[0]
        sum_g += pixel[1]
        sum_b += pixel[2]

    centroid = [sum_r / count, sum_g / count, sum_b / count]
    return centroid


def compare_centroid(a, b):
    for i in range(len(a)):
        if abs(a[i] - b[i]) > 1e-6:
            return False
    return True


def k_means_clustering():
    global CLUSTER, CENTROID, LABELS

    for iteration in range(MAX_ITER):
        CLUSTER = []
        LABELS = []
        for i in range(NO_OF_CLUSTER):
            CLUSTER.append([])

        for pixel in PIXELS:
            min_distance = float("inf")
            closest_centroid = -1

            for i in range(NO_OF_CLUSTER):
                distance = euclidean_distance(CENTROID[i], pixel)
                if distance < min_distance:
                    min_distance = distance
                    closest_centroid = i

            CLUSTER[closest_centroid].append(pixel)
            LABELS.append(closest_centroid)

        converged = True
        for i in range(NO_OF_CLUSTER):
            new_centroid = calculate_centroid(CLUSTER[i])
            if not compare_centroid(new_centroid, CENTROID[i]):
                converged = False
                CENTROID[i] = new_centroid

        if converged:
            print("Converged after", iteration + 1, "iterations.")
            break

    for i in range(len(CLUSTER)):
        print("\nCluster", i + 1, "(Size:", len(CLUSTER[i]), "pixels):")
        print(CLUSTER[i][:10])


def create_clustered_image(output_image_path):
    clustered_image = np.zeros((img_shape[0], img_shape[1], 3), dtype=np.uint8)
    index = 0
    for i in range(img_shape[0]):
        for j in range(img_shape[1]):
            clustered_image[i, j] = CENTROID[LABELS[index]]
            index += 1
    cv2.imwrite(output_image_path, cv2.cvtColor(
        clustered_image, cv2.COLOR_RGB2BGR))


def main():
    image_path = "myimg.jpg"
    output_image_path = "clustered_output.jpg"
    read_image(image_path)
    init()
    k_means_clustering()
    create_clustered_image(output_image_path)


if __name__ == "__main__":
    main()
