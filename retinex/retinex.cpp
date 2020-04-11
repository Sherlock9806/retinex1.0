#include <iostream>
#include "restoration.h"
#include "Msrcr.h"
int main()
{
	Mat img = imread("D://Game//images//77.jpg");
	cout << "raw picture shows" << endl;

	Restoration res;
	Msrcr msr;
	//res.Retinex(img, 200);

	Mat result = msr.FastFilter(img, 200);
	imshow("img", img);
	waitKey(0);

	return 0;

}


//int main()
//{
//    Mat img = imread("D://Game//images//2.jpg");
//    //namedWindow("before", WINDOW_AUTOSIZE);
//    imshow("raw", img);
//    cout << "showing pic1" << endl;
//    Msrcr ms;
//    Mat Gaussin = ms.FilterGaussian(img, 3);
//
//    cout << "showing pic2" << endl;
//    imshow("Gaussin", Gaussin);
//    Mat afterProcessed = ms.Wxy(&img, 3);
////namedWindow("after", WINDOW_AUTOSIZE);
//    imshow("Wxy", afterProcessed);
//    waitKey(0);
//
//    //Mat img = imread("F://Graduation//code//retinex//img//1.jpg");
//    //namedWindow("before", WINDOW_AUTOSIZE);
//    //imshow("before", img);
//    //cout<<"原图输入完毕\n";
//    //Msrcr test;
//    //Mat after = test.Retinex(img, 90);
//    //namedWindow("after", WINDOW_AUTOSIZE);
//    //imshow("after", after);
//    //waitKey(0);
//  
//
//
//    return 0;
//
//}
