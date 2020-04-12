#pragma once
#ifndef _MSRCR_H_
#define _MSRCR_H
#include<vector>
#include <math.h>
#include "cv.h"
#include "highgui.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class Msrcr {
private:
#define pc(image, x, y, c) image->imageData[(image->widthStep * y) + (image->nChannels * x) + c] //y行 x列 c通道  
	/*
	详见https://blog.csdn.net/xiaowei_cqu/article/details/7557063
	*/
#define INT_PREC 1024.0
#define INT_PREC_BITS 10
	inline double int2double(int x) { return (double)x / INT_PREC; }
	inline int double2int(double x) { return (int)(x * INT_PREC + 0.5); }
	inline int int2smallint(int x) { return (x >> INT_PREC_BITS); }
	inline int int2bigint(int x) { return (x << INT_PREC_BITS); }
public:
	vector<double> CreateKernel(double sigma);
	vector<int> CreateFastKernel(double sigma);
	void FilterGaussian(IplImage* img, double sigma);
	void FilterGaussian(Mat src, Mat& dst, double sigma);
	Mat FilterGaussian(Mat src, double sigma);
	void FastFilter(IplImage* img, double sigma);
	//Mat FastFilter(Mat src,  double sigma);
	//void Retinex(IplImage* img, double sigma, int gain = 128, int offset = 128);
	//Mat Retinex(Mat src,double sigma, int gain = 128, int offset = 128);
	Mat FastFilter(Mat src, double sigma);
	void Retinex(IplImage* img, double sigma, int gain = 128, int offset = 128);
	void Retinex(Mat src, double sigma, int gain = 128, int offset = 128);
	void MultiScaleRetinex(IplImage* img, vector<double> weights, vector<double> sigmas, int gain = 128, int offset = 128);
	void MultiScaleRetinex(Mat src, Mat& dst, vector<double> weights, vector<double> sigmas, int gain = 128, int offset = 128);
	void MultiScaleRetinexCR(IplImage* img, vector<double> weights, vector<double> sigmas, int gain = 128, int offset = 128,
		double restoration_factor = 6, double color_gain = 2);
	void MultiScaleRetinexCR(Mat src, Mat& dst, vector<double> weights, vector<double> sigmas, int gain = 128, int offset = 128,
		double restoration_factor = 6, double color_gain = 2);
	void Dxy(IplImage* img, double sigma);
	Mat Dxy(Mat src, double sigma);
	Mat Wxy(Mat* img, double sigma);
	//Mat Wxy(Mat src, double sigma);
};
#endif // !_MSRCR_H_
