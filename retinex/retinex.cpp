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
		Mat dst = res.merge_2(src, 300);
		int length = (*it).length();
		string newPath = (*it).insert(length - 4, "_new");
		imwrite(newPath, dst);
		cout << "Finsh" << endl;
	}
}
int main()
{	
	Mat src = imread("D://Game//images//caps.bmp");
	

	Restoration res;
	Mat dst = res.merge_2(src,200);


	imshow("dst",dst);
	waitKey(0);
	// dst = res.merge_2(src, 180);
	//Mat now = RGB2HSV(src);
	//vector<Mat> v_channel;
	//split(now, v_channel);
	//Mat H = v_channel.at(0);
	//Mat S = v_channel.at(1);
	//Mat V = v_channel.at(2);
	//Mat value = v_channel.at(2);
	//value.convertTo(value, CV_8UC1, 255);

	//Mat dst =res.Illumination(value, 20);
	//Mat temp = res.Decomposition(src, dst);
	//cout << dst;



	

	
	return 0;      

}


