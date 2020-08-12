#include "sideWindowFilter.h"
#include <armadillo> // a high quality linear algebra library (matrix maths)

void sideWindowBoxFilter(const cv::Mat& src, cv::Mat& dst, int radius, int iteration)
{
    int c = src.channels();
    int r = radius;

    int src_h = src.rows;
    int src_w = src.cols;
    int src_N = src_h * src_w;

    int pad_h = src_h + 2 * r;
    int pad_w = src_w + 2 * r;
    int pad_N = pad_h * pad_w;

    cv::Mat k = cv::Mat::ones(2 * r + 1, 1, CV_64F);
    cv::Mat k_L = k.clone();
    cv::Mat k_R = k.clone();

    k /= (2.0 * r + 1.0);
    k_L(cv::Range(r + 1, 2 * r + 1), cv::Range(0, 1)) = 0;
    k_L /= (r + 1.0);
    k_R(cv::Range(0, r), cv::Range(0, 1)) = 0;
    k_R /= (r + 1.0);

    cv::Mat I;
    src.convertTo(I, CV_64F, 1.0 / 255.0);
    std::vector<cv::Mat> I_channels;
    cv::split(I, I_channels);

    std::vector<cv::Mat> d(8);

    arma::mat D(pad_N, 8);

    for (int c_ = 0; c_ < c; c_++) {
        cv::Mat I_padded;
        cv::copyMakeBorder(I_channels[c_], I_padded, r, r, r, r, cv::BORDER_REPLICATE);

        for (int i = 0; i < iteration; i++) {
            cv::sepFilter2D(I_padded, d[0], CV_64F, k_L, k_L, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
            cv::sepFilter2D(I_padded, d[1], CV_64F, k_L, k_R, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
            cv::sepFilter2D(I_padded, d[2], CV_64F, k_R, k_L, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
            cv::sepFilter2D(I_padded, d[3], CV_64F, k_R, k_R, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
            cv::sepFilter2D(I_padded, d[4], CV_64F, k_L, k, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
            cv::sepFilter2D(I_padded, d[5], CV_64F, k_R, k, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
            cv::sepFilter2D(I_padded, d[6], CV_64F, k, k_L, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
            cv::sepFilter2D(I_padded, d[7], CV_64F, k, k_R, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);


            arma::mat U(reinterpret_cast<double*>(I_padded.data), pad_w, pad_h);
            U = arma::reshape(U, pad_N, 1);

            for (int j = 0; j < 8; j++) {
                arma::mat tmp(reinterpret_cast<double*>(d[j].data), pad_w, pad_h);
                D.col(j) = arma::reshape(tmp, pad_N, 1);
            }

            arma::ucolvec jj = arma::index_min(arma::abs(D.each_col() - U), 1);

            U = D.elem(jj * pad_N + arma::linspace<arma::ucolvec>(0, pad_N - 1, pad_N));

            I_padded = cv::Mat(pad_h, pad_w, CV_64F, U.memptr()).clone();
        }

        I_padded(cv::Range(r, src_h + r), cv::Range(r, src_w + r)).copyTo(I_channels[c_]);
    }

    cv::merge(I_channels, dst);
    dst.convertTo(dst, CV_8U, 255.0);
}

using namespace cv;
using namespace std;

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
