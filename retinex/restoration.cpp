#include"restoration.h"
#include<vector>

Mat RGB2HSV(Mat src) {
    int row = src.rows;
    int col = src.cols;
    Mat dst(row, col, CV_32FC3);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            float b = src.at<Vec3b>(i, j)[0] / 255.0;
            float g = src.at<Vec3b>(i, j)[1] / 255.0;
            float r = src.at<Vec3b>(i, j)[2] / 255.0;
            float minn = min(r, min(g, b));
            float maxx = max(r, max(g, b));
            dst.at<Vec3f>(i, j)[2] = maxx; //V
            float delta = maxx - minn;
            float h, s;
            if (maxx != 0) {
                s = delta / maxx;
            }
            else {
                s = 0;
            }
            if (r == maxx) {
                h = (g - b) / delta;
            }
            else if (g == maxx) {
                h = 2 + (b - r) / delta;
            }
            else {
                h = 4 + (r - g) / delta;
            }
            h *= 60;
            if (h < 0)
                h += 360;
            dst.at<Vec3f>(i, j)[0] = h;
            dst.at<Vec3f>(i, j)[1] = s;
        }
    }
    return dst;
}

Mat HSV2RGB(Mat src) {
    int row = src.rows;
    int col = src.cols;
    Mat dst(row, col, CV_8UC3);
    float r, g, b, h, s, v;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            h = src.at<Vec3f>(i, j)[0];
            s = src.at<Vec3f>(i, j)[1];
            v = src.at<Vec3f>(i, j)[2];
            if (s == 0) {
                r = g = b = v;
            }
            else {
                h /= 60;
                int offset = floor(h);
                float f = h - offset;
                float p = v * (1 - s);
                float q = v * (1 - s * f);
                float t = v * (1 - s * (1 - f));
                switch (offset)
                {
                case 0: r = v; g = t; b = p; break;
                case 1: r = q; g = v; b = p; break;
                case 2: r = p; g = v; b = t; break;
                case 3: r = p; g = q; b = v; break;
                case 4: r = t; g = p; b = v; break;
                case 5: r = v; g = p; b = q; break;
                default:
                    break;
                }
            }
            dst.at<Vec3b>(i, j)[0] = int(b * 255);
            dst.at<Vec3b>(i, j)[1] = int(g * 255);
            dst.at<Vec3b>(i, j)[2] = int(r * 255);
        }
    }
    return dst;
} 

float Restoration::GetPixel(IplImage *img,int steps ,int channels,int num)//steps 行 channels 列
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
      
            
            ChangePixel(temp, j, i, 0, (char)int2smallint(v1));
            if (img->nChannels == 1) continue;
            ChangePixel(temp, j, i, 1, (char)int2smallint(v2));
            ChangePixel(temp, j, i, 2, (char)int2smallint(v3));
      

        }
    }   
    imshow("mid", cvarrToMat(temp));
    waitKey(0);

   

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
    FastFilter(&tmp_ipl, sigma);
    return cvarrToMat(&tmp_ipl);
}
void Restoration::Illumination(Mat* img, double sigma) 
{
    Mat fA, fB,fC;
    Mat temp = img->clone();
    //temp = FastFilter(temp, sigma);
    //14:13改
    FastFilter(temp, sigma);
    temp.convertTo(fA, CV_32FC3);
  
    img->convertTo(fB, CV_32FC3);
    absdiff(fA, fB, fC); //fC是
    fC.convertTo(fC, CV_32FC3, 1 / 255.0);  //wxy
    fC = fC.mul(fB) + fA - fC.mul(fA);

    fC.convertTo(*img, CV_8UC3);



}
Mat Restoration::Illumination(Mat src, double sigma)
{
    Mat temp = src.clone();
    Illumination(&temp,sigma);
    return temp;
}

void Restoration::Decomposition(IplImage* img, double sigma, int gain, int offset)
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

    Illumination(&cvarrToMat(A), sigma);
    //FastFilter(A, sigma);

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


}
void  Restoration::Decomposition(Mat src,double sigma, int gain, int offset)
{
    IplImage tmp_ipl;
    tmp_ipl = IplImage(src);
    Decomposition(&tmp_ipl, sigma, gain, offset);

}
