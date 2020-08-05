#include <cstdio>
#include <iostream>
#include <immintrin.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//针对灰度图的中值滤波+CVPR 2019的SideWindowFilter
//其他种类的滤波直接换核即可

//        ┏━━━┯━━━┓--->y
//        ┃ A │ B ┃
//        ┠───┼───┨
//        ┃ D │ C ┃
//        ┗━━━┷━━━┛
//        |
//        v x
// Eight side windows: up(U)(AB), down(D)(DC), right(R)(BC), left(L)(AD)
//       northwest (NW)(A), northeast (NE)(B), southwest (SW)(D), southeast (SE)(C),
// {A, B, D, C, AB, DC, BC, AD}
// Record the number of non-zero elements of the kernel in each side window
int cnt[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
// Record the filter in each side window
vector <int> filter[8];

// Initialize the filter with a radius of radius
void InitFilter(int radius) {
    int n = radius * 2 + 1; // the number of pixels on the diameter
    // initialize cnt and filter
    for (int i = 0; i < 8; i++) {
        cnt[i] = 0;
        filter[i].clear();
    }
    // set initial value of filter
    for (int i = 0; i < 8; i++) {
        for (int x = 0; x < n; x++) {
            for (int y = 0; y < n; y++) {
                if (i == 0 && x <= radius && y <= radius) {
                    filter[i].push_back(1); // A
                }
                else if (i == 1 && x <= radius && y >= radius) {
                    filter[i].push_back(1); // B
                }
                else if (i == 2 && x >= radius && y <= radius) {
                    filter[i].push_back(1); // D
                }
                else if (i == 3 && x >= radius && y >= radius) {
                    filter[i].push_back(1); // C
                }
                else if (i == 4 && x <= radius) {
                    filter[i].push_back(1); // AB
                }
                else if (i == 5 && x >= radius) {
                    filter[i].push_back(1); // DC
                }
                else if (i == 6 && y >= radius) {
                    filter[i].push_back(1); // BC
                }
                else if (i == 7 && y <= radius) {
                    filter[i].push_back(1); // AD
                }
                else { // protect
                    filter[i].push_back(0);
                }
            }
        }
    }
    // set initial value of cnt
    for (int i = 0; i < 8; i++) {
        int sum = 0;
        for (int j : filter[i]) sum += j == 1;
        cnt[i] = sum;
    }
}

// Realize the median filter of Side Window Filter, force edge preservation
Mat MedianSideWindowFilter(Mat src, int radius = 1) {
    int row = src.rows;
    int col = src.cols;
    int channels = src.channels();
    InitFilter(radius);
    // for grayscale image
    vector <int> now; // store the filter image
    if (channels == 1) {
        Mat dst(row, col, CV_8UC1);
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                // at the horizons of the image
                if (i < radius || i + radius >= row || j < radius || j + radius >= col) {
                    dst.at<uchar>(i, j) = src.at<uchar>(i, j);
                    continue;
                }
                int minn = 256;
                int pos = 0;
                for (int k = 0; k < 8; k++) {
                    int val = 0;
                    int id = 0;
                    now.clear();
                    for (int x = -radius; x <= radius; x++) {
                        for (int y = -radius; y <= radius; y++) {
                            //if (x == 0 && y == 0) continue;
                            if (filter[k][id])
                                now.push_back(src.at<uchar>(i + x, j + y) * filter[k][id]);
                            id++;
                            //val += src.at<uchar>(i + x, j + y) * filter[k][id++];
                        }
                    }
                    sort(now.begin(), now.end());
                    int mid = (int)(now.size());
                    val = now[mid / 2];
                    if (abs(val - src.at<uchar>(i, j)) < minn) {
                        minn = abs(val - src.at<uchar>(i, j));
                        pos = k;
                    }
                }
                int val = 0;
                int id = 0;
                now.clear();
                for (int x = -radius; x <= radius; x++) {
                    for (int y = -radius; y <= radius; y++) {
                        //if (x == 0 && y == 0) continue;
                        if (filter[pos][id]) now.push_back(src.at<uchar>(i + x, j + y) * filter[pos][id]);
                        id++;
                        //val += src.at<uchar>(i + x, j + y) * filter[k][id++];
                    }
                }
                sort(now.begin(), now.end());
                int mid = (int)(now.size());
                val = now[mid / 2];
                dst.at<uchar>(i, j) = val;
            }
        }
        return dst;
    }
    //for colorful(RGB) image
    Mat dst(row, col, CV_8UC3);
    for (int c = 0; c < 3; c++) {
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                if (i < radius || i + radius >= row || j < radius || j + radius >= col) {
                    dst.at<Vec3b>(i, j)[c] = src.at<Vec3b>(i, j)[c];
                    continue;
                }
                int minn = 256;
                int pos = 0;
                for (int k = 0; k < 8; k++) {
                    int val = 0;
                    int id = 0;
                    now.clear();
                    for (int x = -radius; x <= radius; x++) {
                        for (int y = -radius; y <= radius; y++) {
                            //if (x == 0 && y == 0) continue;
                            //val += src.at<Vec3b>(i + x, j + y)[c] * filter[k][id++];
                            if (filter[k][id]) now.push_back(src.at<Vec3b>(i + x, j + y)[c] * filter[k][id]);
                            id++;
                        }
                    }
                    sort(now.begin(), now.end());
                    int mid = (int)(now.size());
                    val = now[mid / 2];
                    if (abs(val - src.at<Vec3b>(i, j)[c]) < minn) {
                        minn = abs(val - src.at<Vec3b>(i, j)[c]);
                        pos = k;
                    }
                }
                int val = 0;
                int id = 0;
                now.clear();
                for (int x = -radius; x <= radius; x++) {
                    for (int y = -radius; y <= radius; y++) {
                        //if (x == 0 && y == 0) continue;
                        //val += src.at<Vec3b>(i + x, j + y)[c] * filter[k][id++];
                        if (filter[pos][id]) now.push_back(src.at<Vec3b>(i + x, j + y)[c] * filter[pos][id]);
                        id++;
                    }
                }
                sort(now.begin(), now.end());
                int mid = (int)(now.size());
                val = now[mid / 2];
                dst.at<Vec3b>(i, j)[c] = val;
            }
        }
    }
    return dst;
}

const double eps = 1e-7;

//calculate the angle between vector pt0->pt1 and vecotr pt0->pt2
static double angle(const Point& pt1, const Point& pt2, const Point& pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + eps);
}

/**
 * Search for squares
 * @param image
 * @param(out) squares: store the squares found
 * @param N
 * @param thresh: second threshold for Canny algorithm.
 */
static void findSquares(const Mat& image, vector<vector<Point> >& squares, int N = 5, int thresh = 15)
{
    // Filtering can improve the performance of edge detection
    Mat timg(image); // store the filtered image
    // Ordinary median filter
    medianBlur(image, timg, 9);
    // Median Side Window Filter
    //timg = MedianSideWindowFilter(image, 4);

    Mat gray0(timg.size(), CV_8U), gray;
    vector<vector<Point> > contours; // Store contour

    // Find a rectangle in each color channel of the image
    for (int c = 0; c < 3; c++)
    {
        int ch[] = { c, 0 };
        // Copies specified channels from input arrays to the specified channels of output arrays.
        mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // Try a few different thresholds
        for (int l = 0; l < N; l++)
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            // When the level is 0, the threshold value of 0 is not used,
            // but the Canny edge detection algorithm is used
            if (l == 0)
            {
                // Finds edges in an image using the Canny algorithm
                Canny(gray0, gray, 5, thresh, 5);
                // Perform morphological dilation
                dilate(gray, gray, Mat(), Point(-1, -1));
            }
            else
            {
                // When l is not equal to 0, execute tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l + 1) * 255 / N;
            }

            // Find contours and store them all as a list
            findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

            //Store a polygon (rectangle)
            vector<Point> approx;

            // Test every contour
            for (auto & contour : contours)
            {
                // Approximates a polygonal curve(s) with the specified precision.
                approxPolyDP(Mat(contour), approx, 20, true);

                // After approximation, the square outline should have 4 vertices
                // a relatively large area (to filter out noisy contours), and be a convex set.
                // Note: Use the absolute value of the area, because the area can be positive
                // or negative-depending on the contour direction
                if (approx.size() == 4 &&
                    fabs(contourArea(Mat(approx))) > 1000 &&
                    isContourConvex(Mat(approx)))
                {
                    double maxCosine = 0;

                    for (int j = 2; j < 5; j++)
                    {
                        // Find the greatest cosine of the angle between adjacent sides
                        double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // If the cosines of all angles are small (all angles are 90 degrees),
                    // write the vertex set to the result vector
                    if (maxCosine < 0.3)
                        squares.push_back(approx);
                }
            }
        }
    }
}

//Draw squares on the image
void drawSquares(Mat &image, const vector<vector<Point> >& squares) {
    for (const auto & square : squares)
    {
        const Point* p = &square[0];

        int n = (int)square.size();
        // not detect boundaries
        if (p->x > 3 && p->y > 3)
            polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
    }
}
