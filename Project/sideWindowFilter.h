#ifndef INC_9804PROJECT_SIDEWINDOWFILTER_H
#define INC_9804PROJECT_SIDEWINDOWFILTER_H

#include <opencv2\opencv.hpp>

/**
 * side window box filter
 * @param src original image
 * @param dst output image after filter
 * @param radius the radius of the side window
 * @param iteration
 */
void sideWindowBoxFilter(const cv::Mat& src, cv::Mat& dst, int radius, int iteration);

// Initialize the filter with a radius of radius
void InitFilter(int radius);
cv::Mat MedianSideWindowFilter(cv::Mat src, int radius);

#endif //INC_9804PROJECT_SIDEWINDOWFILTER_H