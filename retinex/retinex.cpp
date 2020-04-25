#include <iostream>
#include "restoration.h"
void show()
{
	Mat src = imread("D://Game//images//77.jpg");
	Mat now = RGB2HSV(src);
	int row = src.rows;
	int col = src.cols;
	Mat H(row, col, CV_32FC1);
	Mat S(row, col, CV_32FC1);
	Mat V(row, col, CV_32FC1);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			H.at<float>(i, j) = now.at<Vec3f>(i, j)[0];
			S.at<float>(i, j) = now.at<Vec3f>(i, j)[1];
			V.at<float>(i, j) = now.at<Vec3f>(i, j)[2];
		}
	}
	
	V.convertTo(V, CV_8UC3,255);
	Restoration res;
	res.Illumination(V, 10);
	imshow("V", V);
	V.convertTo(V, CV_32FC1, 1 / 255.0);
	vector <Mat> v;
	v.push_back(H);
	v.push_back(S);
	v.push_back(V);
	Mat merge_;
	merge(v, merge_);
	Mat dst = HSV2RGB(merge_);
	imshow("dst", dst);
	waitKey(0);

}
int main()
{
	Mat src = imread("D://Game//images//77.jpg");
	Mat now = getMax(src);
	imshow("now", now);
	waitKey(0);
	return 0;

}


