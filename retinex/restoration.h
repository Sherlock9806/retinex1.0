#pragma once

#ifndef _RESTORATION_H_
#define _RESTORATION_H
#include<vector>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
Mat RGB2HSV(Mat src);
Mat HSV2RGB(Mat src);
class Restoration {
private:
#define INT_PREC 1024.0
#define INT_PREC_BITS 10
	inline double int2double(int x) { return (double)x / INT_PREC; }
	inline int double2int(double x) { return (int)(x * INT_PREC + 0.5); }
	inline int int2smallint(int x) { return (int)(x >> INT_PREC_BITS); }
	inline int int2bigint(int x) { return (x << INT_PREC_BITS); }
public:

	void ChangePixel(IplImage* img, int, int, int channels,char ch);
	float GetPixel(IplImage *img, int steps, int channels, int num);
	vector<double> CreateKernel(double sigma);
	vector<int> CreateFastKernel(double sigma);
	void GuassianFilter(IplImage* img, double sigma);
	Mat GuassianFilter(Mat src, double sigma);
	void FastFilter(IplImage* img, double sigma);
	Mat FastFilter(Mat src, double sigma);
	void Illumination(Mat *src, double sigma);
	Mat Illumination(Mat src, double sigma);
	void Modified(Mat src, double sigma);
	void Decomposition(IplImage* img, double sigma, int gain, int offset);
	void  Decomposition(Mat src, double sigma,int gain = 128, int offset = 128);
	
};
#endif // !_RESTORATION_H_
