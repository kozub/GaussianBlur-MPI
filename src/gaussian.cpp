#include <stdio.h>
#include "gaussian.hpp"
#include <opencv2/highgui/highgui.hpp>
#include "mpi.h"
#include "handleError.h"

using namespace cv;

const int Mask[5][5] = {
            {0,1,2,1,0},
            {1,4,8,4,1},
            {2,8,16,8,20},
            {1,4,8,4,1},
            {0,1,2,1,0}
    };
const int sumOfElementsInMask = 96;

GaussianBlur::GaussianBlur(cv::Mat * input)
{
    source = input;
    result = new Mat(source->rows, source->cols, CV_8UC3, 0.0);
}

GaussianBlur::~GaussianBlur()
{
    delete result;
}

Vec3b GaussianBlur::processPixel(int x, int y) {
    Point p = Point(x,y);
    Vec3b pixel = source->at<Vec3b>(p);

    for (int i = 0; i < 3; ++i) {
        int sum = 0;

        for (int j = -2; j <= 2; ++j) {
            for (int k = -2; k <= 2; ++k) {
                int x_idx = x+j;
                int y_idx = y+k;

                if (x_idx < 0 || x_idx > result->cols-1) {
                    x_idx = x;
                }

                if (y_idx < 0 || y_idx > result->rows-1) {
                    y_idx = y;
                }

                sum += source->at<Vec3b>(Point(x_idx,y_idx)).val[i] * Mask[j+2][k+2];
            }
        }
        int newValue = sum / sumOfElementsInMask;
        pixel.val[i] = newValue;
    }

    return pixel;
}

void GaussianBlur::process() {
    for (int i = 0; i < result->cols; ++i) {
        for (int j = 0; j < result->rows; ++j) {
            Point p = Point(i,j);
            Vec3b pixel = processPixel(i, j);
            result->at<Vec3b>(p) = pixel;
        }
    }
}

cv::Mat* GaussianBlur::getResult() {
    return result;
}