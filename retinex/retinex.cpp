#include <iostream>
#include "restoration.h"
Mat GammaCorrection(Mat src)
{
	int row = src.rows;
	int col = src.cols;
	float average = mean(src)[0]/255;// m

	Mat out(row, col,CV_32FC1);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			float gamma = (src.at<float>(i, j) + average + 1) / (1 + 1 + average);
			out.at<float>(i, j) = powf(src.at<float>(i, j), gamma);
		}
	}
	
	return out;
}

int main()
{
	Mat src = imread("D://Game//images//77.jpg");
	Mat now = RGB2HSV(src);
	vector<cv::Mat> v_channel;
	split(now, v_channel);
	Mat H = v_channel.at(0);
	Mat S = v_channel.at(1);
	Mat V = v_channel.at(2);
	Restoration res;

	Mat dst = res.FastFilter(V, 5);
	//Mat dst = res.Illumination(V, 180);//dst是单通道  //src三通道
	vector <Mat> v;
	v.push_back(H);
	v.push_back(S);
	v.push_back(dst);
	Mat merge_;
	merge(v, merge_);
	Mat result = HSV2RGB(merge_);
	//
	////Mat result = Decomposition(src, dst);

	imshow("dst", result);
	waitKey(0);
	return 0;      

}


