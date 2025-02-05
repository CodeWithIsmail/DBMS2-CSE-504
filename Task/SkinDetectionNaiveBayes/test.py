import numpy as np
import imageio
import matplotlib.pyplot as plt

input_image = "testing/images.jpg"
output_image = "testing/masked.jpg"
fp = open('train_result.txt', "r")

trained_prob= np.zeros(shape=(256, 256, 256))
test_image = np.empty(shape=(256, 256, 256), dtype=int).fill(0)
test_image = imageio.v2.imread(input_image)

for r in range(256):
    for g in range(256):
        for b in range(256):
            val = fp.readline()
            trained_prob[r][g][b] = float(val)


height, width, _ = test_image.shape
T =0.15

for x in range(height):
    for y in range(width):
        red = test_image[x, y, 0]
        green = test_image[x, y, 1]
        blue = test_image[x, y, 2]

        if (trained_prob[red, green, blue] <= T):
            test_image[x, y, 0] = 255
            test_image[x, y, 1] = 255
            test_image[x, y, 2] = 255

plt.imsave(output_image, test_image)
print("Done...")
