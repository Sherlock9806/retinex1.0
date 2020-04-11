#include"restoration.h"
#include<vector>

uchar Restoration::GetPixel(IplImage *img,int steps ,int channels,int num)//steps 行 channels 列
{
    return img->imageData[(img->widthStep * steps) + (img->nChannels * channels) + num];
}
void Restoration::ChangePixel(IplImage* img, int steps,  int channels, int num ,char ch) {
    img->imageData[(img->widthStep * steps) + (img->nChannels * channels) + num] = ch;
}
vector<double> Restoration::CreateKernel(double sigma)
{
    int i, x, filter_size;
    vector<double> filter;
    double sum;
    // set sigma's upline
    // 为sigma设定上限
    if (sigma > 300) sigma = 300;

    // get needed filter size (enforce oddness)
    // 获取需要的滤波尺寸，且强制为奇数；
    filter_size = (int)floor(sigma * 6) / 2;
    filter_size = filter_size * 2 + 1;

    // Calculate exponential
    // 计算指数
    sum = 0;
    for (i = 0; i < filter_size; i++)
    {
        double tmpValue;
        x = i - (filter_size / 2);
        tmpValue = exp(-(x * x) / (2 * sigma * sigma));
        filter.push_back(tmpValue);

        sum += tmpValue;
    }

    // Normalize
    // 归一化计算
    for (i = 0; i < filter_size; i++)
        filter[i] /= sum;

    return filter;
}
//转换为整型
vector<int> Restoration::CreateFastKernel(double sigma)
{
    vector<double> fp_kernel;
    vector<int> kernel;
    int i, filter_size;

    // Reject unreasonable demands
    // 设置上限
    if (sigma > 300) sigma = 300;

    // get needed filter size (enforce oddness)
    // 获取需要的滤波尺寸，且强制为奇数；
    filter_size = (int)floor(sigma * 6) / 2;
    filter_size = filter_size * 2 + 1;

    // Create Kernel
    // 创建内核
    fp_kernel = CreateKernel(sigma);

    // Change kernel's data type from double to int
    // double内核转为int型
    for (i = 0; i < filter_size; i++)
    {
        int tmpValue;
        tmpValue = double2int(fp_kernel[i]);
        kernel.push_back(tmpValue);
    }

    return kernel;
}

void Restoration::GuassianFilter(IplImage* img, double sigma)
{   
    /*
     高斯滤波
     先横向再纵向

     */
    

    int i, j, k, source, filter_size;

    vector<int> kernel;
    IplImage* temp;
    int v1, v2, v3;
    // Reject unreasonable demands
    // 设置上限
    if (sigma > 300) sigma = 300;

    // get needed filter size (enforce oddness)
    // 获取需要的滤波尺寸，且强制为奇数；
    filter_size = (int)floor(sigma * 6) / 2;
    filter_size = filter_size * 2 + 1;

    // Create Kernel
    // 创建内核
    kernel = CreateFastKernel(sigma);

    temp = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);

    // filter x axis
    // X轴滤波 
    for (j = 0; j < temp->height; j++)
    {
        for (i = 0; i < temp->width; i++)
        {
            // inner loop has been unrolled
            // 内层循环已经展开
            v1 = v2 = v3 = 0;
            for (k = 0; k < filter_size; k++)
            {
                source = i + filter_size / 2 - k;

                if (source < 0) source *= -1;
                if (source > img->width - 1) source = 2 * (img->width - 1) - source; //对称处理


                v1 += kernel[k] * GetPixel(img, j, source, 0);
                if (img->nChannels == 1) continue;
                v2 += kernel[k] * GetPixel(img, j, source, 1);
                v3 += kernel[k] * GetPixel(img, j, source, 2);
       
              
            }
      

             //set value and move on
            
            ChangePixel(temp, j, i, 0, (char)int2smallint(v1));
            if (img->nChannels == 1) continue;
            ChangePixel(temp, j, i, 1, (char)int2smallint(v2));
            ChangePixel(temp, j, i, 2, (char)int2smallint(v3));
      

        }
    }   
    //imshow("mid", cvarrToMat(img));
    //waitKey(0);

   

     //filter y axis
     //Y轴滤波
    for (j = 0; j < img->height; j++)
    {
        for (i = 0; i < img->width; i++)
        {
            v1 = v2 = v3 = 0;
            for (k = 0; k < filter_size; k++)
            {
                source = j + filter_size / 2 - k;

                if (source < 0) source *= -1;
                if (source > temp->height - 1) source = 2 * (temp->height - 1) - source;
                //pc(image, x, y, c) image->imageData[(image->widthStep * y) + (image->nChannels * x) + c]
                v1 += kernel[k] * GetPixel(temp, source, i, 0);
                if (img->nChannels == 1) continue;
                v2 += kernel[k] * GetPixel(temp, source, i, 1);
                v3 += kernel[k] * GetPixel(temp, source, i, 2);
            }
        


        // set value and move on
        ChangePixel(img, j, i, 0, (char)int2smallint(v1));
        /* pc(img, i, j, 0) = (char)int2smallint(v1);*/
        if (img->nChannels == 1) continue;
        ChangePixel(img, j, i, 1, (char)int2smallint(v2));
        ChangePixel(img, j, i, 2, (char)int2smallint(v3));


        }
    }
 /*   imshow("end", cvarrToMat(img));
    waitKey(0);*/

    cvReleaseImage(&temp);


}
Mat Restoration::GuassianFilter(Mat src, double sigma) 
{   
 
    IplImage tmp_ipl;
    tmp_ipl = IplImage(src);
    GuassianFilter(&tmp_ipl, sigma);
    return cvarrToMat(&tmp_ipl);
}

void Restoration::FastFilter(IplImage* img, double sigma)
{
    int filter_size;
    if (sigma > 300) sigma = 300;

    filter_size = (int)floor(sigma * 6) / 2;
    filter_size = filter_size * 2 + 1;

    if (filter_size < 3) return;
    if (filter_size < 10) {
        GuassianFilter(img, sigma);
    }
    else
    {
        if (img->width < 2 || img->height < 2) return;
        IplImage* sub_img = cvCreateImage(cvSize(img->width / 2, img->height / 2), img->depth, img->nChannels);
        cvPyrDown(img, sub_img);
        FastFilter(sub_img, sigma / 2.0);
        cvResize(sub_img, img, CV_INTER_LINEAR);
        cvReleaseImage(&sub_img);
    }
}
Mat Restoration::FastFilter(Mat src, double sigma)
{
    IplImage tmp_ipl;
    tmp_ipl = IplImage(src);
    GuassianFilter(&tmp_ipl, sigma);
    return cvarrToMat(&tmp_ipl);
   
}
void Restoration::Illumination(Mat* img, double sigma) 
{
    Mat fA, fB,fC;
    //vector<Mat> channels;

    //IplImage temp = IplImage(img->clone());
    //FastFilter(&temp, sigma);//temp = Fxy
    Mat temp = img->clone();

    temp = FastFilter(temp, sigma);
   /* temp = GuassianFilter(temp, sigma);*/
  
    

    temp.convertTo(fA, CV_32FC3);
 
    img->convertTo(fB, CV_32FC3);
    
    absdiff(fA, fB, fC); //fC是
    ////分离通道
    //split ( fC, channels);
    //归一化 
 
    fC.convertTo(fC, CV_32FC3, 1 / 255.0);  //wxy
  /*  fC = fC.mul(*img) + (1 - fC).mul(temp);*/
 
    fC = fC.mul(fB) + fA - fC.mul(fA);
    //fC.convertTo(*img, CV_8UC3);
  /*  cout << fC << endl;*/

    fC.convertTo(*img, CV_8UC3);

 
 /*   cvConvertScale(&fC, img);*/
}
Mat Restoration::Illumination(Mat src, double sigma)
{
    Mat temp = src.clone();
    Illumination(&temp,sigma);
    return temp;
}

//void Restoration::Modified(Mat img,double sigma)
//{
//
//    
//    Mat fA = img.clone();//fA = img
//    Mat fB;
//    Mat fC = img.clone();
//    
//    fA.convertTo(fA, CV_32FC3);
//    log(fA, fB);//fB = log fA
//    //Illumination(&fC, sigma); //fC 为光照
//    //GaussianFilter(&fC, sigma);
//    fC.convertTo(fC, CV_32FC3);
//    log(fC, fC);//fC = log fC
//    fC = fB - fC;
//    IplImage temp = IplImage(fC);
//    cvConvertScale(&temp,&temp, 128, 128);
//    cout << "********************" << endl;
//    Mat dst = cvarrToMat(&temp);
//    dst.convertTo(dst, CV_8UC3);
//    imshow("dst", dst);
//    waitKey(0);
//    
// 
//}
void Restoration::Retinex(IplImage* img, double sigma, int gain, int offset)
{
    IplImage* A, * fA, * fB, * fC;

    // Initialize temp images
    // 初始化缓存图像
    fA = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
    fB = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
    fC = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);

    // Compute log image
    // 计算对数图像
    cvConvert(img, fA); //fA = img
    cvLog(fA, fB);      //fB = log(fA)

    // Compute log of blured image
    // 计算滤波后模糊图像的对数图像
    A = cvCloneImage(img);
    GuassianFilter(cvarrToMat(A), sigma);
   
    cvConvert(A, fA); // fA = A 
    cvLog(fA, fC);        //fC = log(A)

    // Compute difference
    // 计算两图像之差
    cvSub(fB, fC, fA);  //fA = fB - fC

    // Restore
    // 恢复图像
    cvConvertScale(fA, img, gain, offset);

    // Release temp images
    // 释放缓存图像
    cvReleaseImage(&A);
    cvReleaseImage(&fA);
    cvReleaseImage(&fB);
    cvReleaseImage(&fC);
    imshow("img", cvarrToMat(img));
    waitKey(0);


}
void  Restoration::Retinex(Mat src,double sigma, int gain, int offset)
{
    IplImage tmp_ipl;
    tmp_ipl = IplImage(src);
    Retinex(&tmp_ipl, sigma, gain, offset);

}
