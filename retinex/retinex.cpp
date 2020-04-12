#include <iostream>
#include "restoration.h"
#include "Msrcr.h"
int main()
{
	Mat img = imread("D://Game//images//1.jpg");
	cout << "raw picture shows" << endl;

	Restoration res;
	Msrcr msr;
	//Mat result = msr.FastFilter(img, 200);
	
  res.Decomposition(img,180);
  imshow("img", img);
  waitKey(0);



	return 0;

}


