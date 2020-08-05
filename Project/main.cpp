#include <iostream>
#include "detect.h"

int main() {
    std::cout << "Hello, World!" << std::endl;
    Mat src = cv::imread("floor.jpg");
    vector<vector<Point> > squares;
    findSquares(src, squares, 5, 50);
    drawSquares(src, squares);
    imshow("result", src);
    imwrite("floor_ms.jpg", src); // m: median filter; ms: median side window filter
    waitKey(0);
    return 0;
}
//#include <iostream>
//#include <opencv2/highgui/highgui.hpp>
//
//using namespace std;
//using namespace cv;
//
//int main() {
//    Mat img = imread("opencv_ios.png");
//    if (img.empty()) {
//        cout << "Error" << endl;
//        return -1;
//    }
//    namedWindow("pic", WINDOW_FULLSCREEN);
//    imshow("pic", img);
//    waitKey();
//    return 0;
//}