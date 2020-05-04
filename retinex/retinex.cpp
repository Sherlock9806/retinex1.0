#include <iostream>
#include "restoration.h"
#include<io.h>
void getFiles(string path, vector<string>& files)
{
	//文件句柄
	intptr_t hFile   = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
void enhance(string filePath)
{
	vector<string> files;

	////获取该路径下的所有文件
	getFiles(filePath, files);
	Restoration res;
	for (vector<string>::iterator it = files.begin(); it != files.end(); it++)
	{
		Mat src = imread(*it);
		Mat dst = res.merge_2(src, 15);
		int length = (*it).length();
		string newPath = (*it).insert(length - 4, "_new");
		imwrite(newPath, dst);
		cout << "Finsh" << endl;
	}
}
int main()
{	
	string filePath = "D://Game//images";
	enhance(filePath);
	
	//Mat dst = res.GammaCorrection(V);
	
	//dst.convertTo(dst, CV_8UC1,255);


	waitKey(0);

	return 0;

	/*
	Mat now = RGB2HSV(src);
	vector<Mat> v_channel;
	split(now, v_channel);
	Mat H = v_channel.at(0);
	Mat S = v_channel.at(1);
	Mat V = v_channel.at(2);

	Restoration res;
	
	V.convertTo(V, CV_8UC1, 255);
	Mat ill = res.Illumination(V,10);//ILL FMXY
	Mat rgb = res.Decomposition(src, ill);
	Mat Gamma(ill.rows, ill.cols, CV_32FC1);
	ill.convertTo(Gamma, CV_32FC1, 1.0 / 255);
	Mat dst = res.GammaCorrection(Gamma);
	dst.convertTo(dst, CV_8UC1, 255);
	Mat hist = res.e_hist(dst);
	Mat output(src.rows, src.cols, CV_32FC3);
	for (int i = 0; i < output.rows; i++) {
		for (int j = 0; j < output.cols; j++) {

			output.at<Vec3f>(i, j)[0] = (rgb.at<Vec3f>(i, j)[0] * hist.at<uchar>(i, j));
			output.at<Vec3f>(i, j)[1] = (rgb.at<Vec3f>(i, j)[1] * hist.at<uchar>(i, j));
			output.at<Vec3f>(i, j)[2] = (rgb.at<Vec3f>(i, j)[2] * hist.at<uchar>(i, j));
			//output.at<Vec3b>(i, j)[0] = Decom.at<Vec3f>(i,j)[0]*hist.at<float>(i,j);
		}
	}
	output.convertTo(output, CV_8UC3);
	imshow(" ", output);
	waitKey(0);
	*/

}


