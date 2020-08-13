#ifndef INC_9804PROJECT_DETECTSQUARE_H
#define INC_9804PROJECT_DETECTSQUARE_H

#include <opencv2/opencv.hpp>

extern int g_dst_hight; //height of the output image
extern int g_dst_width; //width of the output image

void sortCorners(std::vector<cv::Point2f>& corners,
                 cv::Point2f center);

cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b);

bool IsBadLine(int a, int b);

void CalcDstSize(const std::vector<cv::Point2f>& corners);

/**
 * Search for rectangles
 * @param image
 * @param(out) squares: store the squares found
 * @param N
 * @param thresh: second threshold for Canny algorithm.
 */
static void findSquares(const cv::Mat& image, std::vector<std::vector<cv::Point> >& squares,
        int N = 5, int thresh = 15);

//Draw squares on the image
void drawSquares(cv::Mat &image, const std::vector<std::vector<cv::Point> >& squares);

#endif //INC_9804PROJECT_DETECTSQUARE_H
