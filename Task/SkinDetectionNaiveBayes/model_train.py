import numpy as np
import cv2

skin_rgb = np.zeros(shape=(256, 256, 256), dtype=int)
non_skin_rgb = np.zeros(shape=(256, 256, 256), dtype=int)

total_skin_count = 0
total_non_skin_count = 0
total_image = 555
image_index = ["%04d" % x for x in range(1000)]

for index in range(total_image):
    original_image = cv2.imread("original/"+str(image_index[index])+".jpg")
    mask_image = cv2.imread("mask/"+str(image_index[index])+".bmp")

    height, width, channel = mask_image.shape

    for i in range(height):
        for j in range(width):

            mask_blue = mask_image[i, j, 0]
            mask_green = mask_image[i, j, 1]
            mask_red = mask_image[i, j, 2]

            blue = original_image[i, j, 0]
            green = original_image[i, j, 1]
            red = original_image[i, j, 2]

            if mask_blue > 250 and mask_green > 250 and mask_red > 250:
                non_skin_rgb[red, green, blue] += 1
                total_non_skin_count += 1
            else:
                skin_rgb[red, green, blue] += 1
                total_skin_count += 1

    print(f"image {index} done")


fp = open('train_result.txt', 'w')
p_s = total_skin_count/total_image
p_ns = total_non_skin_count/total_image
for r in range(256):
    for g in range(256):
        for b in range(256):
            prob_skin = skin_rgb[r, g, b]/total_skin_count
            prob_non_skin = non_skin_rgb[r, g, b]/total_non_skin_count

            if non_skin_rgb[r, g, b] != 0:
                threashlod = (prob_skin*p_s)/(prob_non_skin*p_ns)
            else:
                threashlod = 0.0

            fp.write(f"{threashlod}\n")

fp.close()
