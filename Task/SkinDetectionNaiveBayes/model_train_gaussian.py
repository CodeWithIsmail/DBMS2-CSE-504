import numpy as np
import cv2
import math

# Define total images
total_images = 555
image_index = ["%04d" % x for x in range(1000)]  # Generating file names

# Initialize an empty list to store pixel data
features_list = []
labels_list = []

# Data collection from images
for index in range(total_images):
    original_image = cv2.imread(f"original/{image_index[index]}.jpg")
    mask_image = cv2.imread(f"mask/{image_index[index]}.bmp")

    if original_image is None or mask_image is None:
        print(f"Skipping image {index} due to missing file.")
        continue

    # Reshape images into a 2D array (flattened)
    height, width, _ = mask_image.shape
    pixels = original_image.reshape(-1, 3)  # Convert to shape (height*width, 3)
    mask_pixels = mask_image.reshape(-1, 3)

    # Create a mask where white pixels (255, 255, 255) are non-skin (label=0)
    is_non_skin = np.all(mask_pixels > 250, axis=1)

    # Append RGB values and labels
    features_list.append(pixels)
    labels_list.append(is_non_skin.astype(int))  # Convert boolean to 0/1

    print(f"Image {index} processed")

# Convert lists to NumPy arrays for efficient computation
features = np.vstack(features_list)  # Stack all images vertically
labels = np.hstack(labels_list)  # Flatten label array

# Separate skin and non-skin pixels
skin_pixels = features[labels == 0]  # Label 0 means skin (flipped logic)
non_skin_pixels = features[labels == 1]  # Label 1 means non-skin

# Compute mean and variance for each class
mean_skin = np.mean(skin_pixels, axis=0)
var_skin = np.var(skin_pixels, axis=0)

mean_non_skin = np.mean(non_skin_pixels, axis=0)
var_non_skin = np.var(non_skin_pixels, axis=0)


def gaussian_pdf(x, mean, var):
    return (1 / np.sqrt(2 * math.pi * var)) * np.exp(-((x - mean) ** 2) / (2 * var))


# Compute probabilities for all RGB values efficiently
r_vals, g_vals, b_vals = np.meshgrid(np.arange(256), np.arange(256), np.arange(256), indexing='ij')

skin_prob = (gaussian_pdf(r_vals, mean_skin[0], var_skin[0]) *
             gaussian_pdf(g_vals, mean_skin[1], var_skin[1]) *
             gaussian_pdf(b_vals, mean_skin[2], var_skin[2]))

non_skin_prob = (gaussian_pdf(r_vals, mean_non_skin[0], var_non_skin[0]) *
                 gaussian_pdf(g_vals, mean_non_skin[1], var_non_skin[1]) *
                 gaussian_pdf(b_vals, mean_non_skin[2], var_non_skin[2]))

prediction = np.divide(skin_prob, (skin_prob + non_skin_prob), where=(skin_prob + non_skin_prob) != 0)

# Save predictions efficiently
np.savetxt("train_result.txt", prediction.flatten(), fmt="%.6f")

print("Training complete. Probabilities saved to train_result.npy")
