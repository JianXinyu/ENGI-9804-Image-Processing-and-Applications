#include "detectSquare.h"

using namespace cv;
using namespace std;

bool sort_corners(std::vector<cv::Point2f>& corners)
{
    std::vector<cv::Point2f> top, bot;
    cv::Point2f tmp_pt;
    std::vector<cv::Point2f> olddata = corners;

    if (corners.size() != 4)
    {
        return false;
    }

    for (size_t i = 0; i < corners.size(); i++)
    {
        for (size_t j = i + 1; j<corners.size(); j++)
        {
            if (corners[i].y < corners[j].y)
            {
                tmp_pt = corners[i];
                corners[i] = corners[j];
                corners[j] = tmp_pt;
            }
        }
    }
    top.push_back(corners[0]);
    top.push_back(corners[1]);
    bot.push_back(corners[2]);
    bot.push_back(corners[3]);
    if (top.size() == 2 && bot.size() == 2) {
        corners.clear();
        cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
        cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
        cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
        cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];
        corners.push_back(tl);
        corners.push_back(tr);
        corners.push_back(br);
        corners.push_back(bl);
        return true;
    }
    else
    {
        corners = olddata;
        return false;
    }
}

cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b)
{
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

    if (float d = ((float)(x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4)))
    {
        cv::Point2f pt;
        pt.x = ((x1*y2 - y1*x2) * (x3 - x4) - (x1 - x2) * (x3*y4 - y3*x4)) / d;
        pt.y = ((x1*y2 - y1*x2) * (y3 - y4) - (y1 - y2) * (x3*y4 - y3*x4)) / d;
        return pt;
    }
    else
        return cv::Point2f(-1, -1);
}

bool IsBadLine(int a, int b)
{
    if (a * a + b * b < 100)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool x_sort(const Point2f & m1, const Point2f & m2)
{
    return m1.x < m2.x;
}

// Determine the centerline of the four points
void sortCorners(std::vector<cv::Point2f>& corners,
                 cv::Point2f center)
{
    std::vector<cv::Point2f> top, bot;
    vector<Point2f> backup = corners;

    sort(corners.begin(), corners.end(), x_sort);  // Note that the 4 points are sorted by the size of x first

    for (int i = 0; i < corners.size(); i++)
    {
        if (corners[i].y < center.y && top.size() < 2)    // The value less than 2 here is to avoid
                                                          // the situation where all three vertices are on top
            top.push_back(corners[i]);
        else
            bot.push_back(corners[i]);
    }
    corners.clear();

    if (top.size() == 2 && bot.size() == 2)
    {
        cout << "log" << endl;
        cv::Point2f tl = top[0].x > top[1].x ? top[1] : top[0];
        cv::Point2f tr = top[0].x > top[1].x ? top[0] : top[1];
        cv::Point2f bl = bot[0].x > bot[1].x ? bot[1] : bot[0];
        cv::Point2f br = bot[0].x > bot[1].x ? bot[0] : bot[1];


        corners.push_back(tl);
        corners.push_back(tr);
        corners.push_back(br);
        corners.push_back(bl);
    }
    else
    {
        corners = backup;
    }
}

int g_dst_hight; // height of the final image
int g_dst_width; // width of the final image

void CalcDstSize(const vector<cv::Point2f>& corners)
{
    int h1 = sqrt((corners[0].x - corners[3].x)*(corners[0].x - corners[3].x) + (corners[0].y - corners[3].y)*(corners[0].y - corners[3].y));
    int h2 = sqrt((corners[1].x - corners[2].x)*(corners[1].x - corners[2].x) + (corners[1].y - corners[2].y)*(corners[1].y - corners[2].y));
    g_dst_hight = MAX(h1, h2);

    int w1 = sqrt((corners[0].x - corners[1].x)*(corners[0].x - corners[1].x) + (corners[0].y - corners[1].y)*(corners[0].y - corners[1].y));
    int w2 = sqrt((corners[2].x - corners[3].x)*(corners[2].x - corners[3].x) + (corners[2].y - corners[3].y)*(corners[2].y - corners[3].y));
    g_dst_width = MAX(w1, w2);
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


static void findSquares(const Mat& image, vector<vector<Point> >& squares, int N, int thresh)
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



