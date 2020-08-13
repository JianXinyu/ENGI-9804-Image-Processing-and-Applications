#include "detectSquare.h"
#include "sideWindowFilter.h"

#include <iostream>
#include <algorithm>

using namespace std;
using namespace cv;

template<typename  T>
std::ostream& operator << ( std::ostream& outs, const std::vector<T> &vec)
{
    std::size_t n = 0;
    for(const auto& entry : vec)
    {
//        outs << (n++? " " : "") << entry;
        outs << entry << " ";
    }

    return outs;
}

//int main()
//{
//    vector<int> vec;
//    for(int i = 0; i < 6; i++)
//    {
//        vec.push_back((i+3)%6);
//    }
//    sort(vec.begin(), vec.end());
//
//    std::cout << vec;
//    return 0;
//}



int main()
{
    Mat src = imread("./original_image/mess.png");
    imshow("src img", src);
    Mat source = src.clone();
    Mat bkup = src.clone();
    Mat img = src.clone();
    /***************************Pre-process*****************************/
    //! Transform into grayscale image
    cvtColor(img, img, COLOR_RGB2GRAY);
    imshow("gray", img);
    imwrite("./output_images/gray_image.jpg", img);

//    equalizeHist(img, img);
//    imshow("equal", img);

    //! filter
//    GaussianBlur(img, img, Size(5, 5), 0, 0);  // Gaussian filtering
//    img = MedianSideWindowFilter(img, 4);
    sideWindowBoxFilter(img, img, 4, 6);
    imwrite("./output_images/filtered_image.jpg", img);
    //! dilation
    // Get a custom core
    // The first parameter MORPH_RECT represents the rectangular convolution kernel,
    // of course, you can also choose elliptical, cross-shaped
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(img, img, element); // Dilation
    imshow("dilate", img);
    imwrite("./output_images/dilated_image.jpg", img);

    //! edge detection
    Canny(img, img, 30, 120, 3);
    imshow("get contour", img);
    imwrite("./output_images/contour.jpg", img);

    /********************************** contours processing *****************************/
    vector<vector<Point> > contours;
    vector<vector<Point> > f_contours;
    std::vector<cv::Point> approx2;
    //Note that the fifth parameter is CV_RETR_EXTERNAL, only the outer frame is retrieved
    findContours(img, f_contours, RETR_EXTERNAL, CHAIN_APPROX_NONE); // Find contours

    // Find the contour with the largest area
    int max_area = 0;
    int index;
    for (int i = 0; i < f_contours.size(); i++)
    {
        double tmparea = fabs(contourArea(f_contours[i]));
        if (tmparea > max_area)
        {
            index = i;
            max_area = tmparea;
        }

    }
    contours.push_back(f_contours[index]);

    // Because here is to find the outermost contour, there is only one contour theoretically
    cout << "Number of Contours: " << contours.size() << endl;

    vector<Point> tmp = contours[0];

    /*********************************Vertices Search***************************************/
    for (int line_type = 1; line_type <= 3; line_type++)
    {
        cout << "line_type: " << line_type << endl;
        Mat black = img.clone();
        black.setTo(0);
        // Pay attention to the thickness of the line, don’t choose it too thin
        drawContours(black, contours, 0, Scalar(255), line_type);
        imshow("show contour", black);
        imwrite("./output_images/processed_contour.jpg", black);

        std::vector<Vec4i> lines;
        std::vector<cv::Point2f> corners;
        std::vector<cv::Point2f> approx;

        int para = 10;
        int flag = 0;
        int round = 0;
        cv::Point2f center(0, 0);
        for (; para < 300; para++)
        {
            cout << "round: " << ++round << endl;
            lines.clear();
            corners.clear();
            approx.clear();
            center = Point2f(0, 0);

            cv::HoughLinesP(black, lines, 1, CV_PI / 180, para, 30, 10);

            // Filter straight lines that are too close
            std::set<int> ErasePt;
            for (int i = 0; i < lines.size(); i++)
            {
                for (int j = i + 1; j < lines.size(); j++)
                {
                    if (IsBadLine(abs(lines[i][0] - lines[j][0]), abs(lines[i][1] - lines[j][1])) && (IsBadLine(abs(lines[i][2] - lines[j][2]), abs(lines[i][3] - lines[j][3]))))
                    {
                        ErasePt.insert(j);// Add the bad line to the collection
                    }
                }
            }

            int Num = lines.size();
            while (Num != 0)
            {
                std::set<int>::iterator j = ErasePt.find(Num);
                if (j != ErasePt.end())
                {
                    lines.erase(lines.begin() + Num - 1);
                }
                Num--;
            }
            // Filter the situation that the number of detected lines is not 4
            if (lines.size() != 4)
            {
                continue;
            }

            // Calculate the intersection points of straight lines
            for (int i = 0; i < lines.size(); i++)
            {
                for (int j = i + 1; j < lines.size(); j++)
                {
                    cv::Point2f pt = computeIntersect(lines[i], lines[j]);
                    // Ensure that the intersection point is within the range of the image
                    if (pt.x >= 0 && pt.y >= 0 && pt.x <= src.cols && pt.y <= src.rows)
                        corners.push_back(pt);
                }
            }
            if (corners.size() != 4)
            {
                continue;
            }

            bool IsGoodPoints = true;

            // Ensure that the distance between the point and the point is large enough to eliminate the wrong point
            for (int i = 0; i < corners.size(); i++)
            {
                for (int j = i + 1; j < corners.size(); j++)
                {
                    int distance = sqrt((corners[i].x - corners[j].x)*(corners[i].x - corners[j].x) + (corners[i].y - corners[j].y)*(corners[i].y - corners[j].y));
                    if (distance < 5)
                    {
                        IsGoodPoints = false;
                    }
                }
            }

            if (!IsGoodPoints) continue;

            cv::approxPolyDP(cv::Mat(corners), approx, cv::arcLength(cv::Mat(corners), true) * 0.02, true);

            if (lines.size() == 4 && corners.size() == 4 && approx.size() == 4)
            {
                flag = 1;
                break;
            }
        }

        // Get mass center
        for (auto & corner : corners)
            center += corner;
        center *= (1. / corners.size());

        if (flag)
        {
            cout << "we found it!" << endl;
            cv::circle(bkup, corners[0], 3, CV_RGB(255, 0, 0), -1);
            cv::circle(bkup, corners[1], 3, CV_RGB(0, 255, 0), -1);
            cv::circle(bkup, corners[2], 3, CV_RGB(0, 0, 255), -1);
            cv::circle(bkup, corners[3], 3, CV_RGB(255, 255, 255), -1);
            cv::circle(bkup, center, 3, CV_RGB(255, 0, 255), -1);
            imshow("backup", bkup);
            cout << "corners size " << corners.size() << endl;
            // cv::waitKey();

            // bool sort_flag = sort_corners(corners);
            // if (!sort_flag) cout << "fail to sort" << endl;

            sortCorners(corners, center);
            cout << "corners size " << corners.size() << endl;
            cout << "tl:" << corners[0] << endl;
            cout << "tr:" << corners[1] << endl;
            cout << "br:" << corners[2] << endl;
            cout << "bl:" << corners[3] << endl;

            /****************************** Perspective Transformation**********************/
            CalcDstSize(corners);

            cv::Mat quad = cv::Mat::zeros(g_dst_hight, g_dst_width, CV_8UC3);
            std::vector<cv::Point2f> quad_pts;
            quad_pts.emplace_back(0, 0);
            quad_pts.emplace_back(quad.cols, 0);
            quad_pts.emplace_back(quad.cols, quad.rows);

            quad_pts.emplace_back(0, quad.rows);

            // perspective transform
            cv::Mat transmtx = cv::getPerspectiveTransform(corners, quad_pts);
            cv::warpPerspective(source, quad, transmtx, quad.size());

            imshow("find", bkup);
            imwrite("./output_images/vertices.jpg", bkup);
            imshow("quadrilateral", quad);
            imwrite("./output_images/output.jpg", quad);
            /*uncomment this if Binarization is needed*/
            /*
            Mat local,gray;
            cvtColor(quad, gray, CV_RGB2GRAY);
            int blockSize = 25;
            int constValue = 10;
            adaptiveThreshold(gray, local, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, blockSize, constValue);
            imshow("Binarization", local);
            */

            cv::waitKey();
            return 0;
        }
    }

    cout << "can not transform!" << endl;
    waitKey();
}