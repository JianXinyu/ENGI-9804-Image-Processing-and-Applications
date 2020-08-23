# Rectangular Detection and Restoration Using Side Window Filter

## Dependencies:

- OpenCV version 4.4.0

- armadillo: a scientific calculation library used for the fast computation of side window filter

## Usage

- build the project.

- put the images that you want to process in the folder ./original_image in the directory where .exe file is.

- change the input image name in the main().

- run the code, check the step by step process by clicking off the image panel.

- check the results in the folder ./output_image


## Overview

Given an input image with a skew rectangular document and noisy background, the program can generate an output image with a regular rectangular document automatically. The workflow is shown in the following figure.

![image-20200822222428170](.\image-20200822222428170.png)

## Some thoughts

Side window filter is a really useful and powerful filter, yet it's simple and flexible. It can be combined with different filters, such as box filter, median filter. It could preserve the edges and corners, which is an wonderful feature in the edge detection. 

At first, I thought doing perspective transformation is easy. It then turns out I was awfully wrong. At last, I used some tricky engineering technique to implement the function. However, it's not robust. It doesn't work for some figures. AI might be the final solution.