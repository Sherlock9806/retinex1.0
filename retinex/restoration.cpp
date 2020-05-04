#include"restoration.h"
#include<vector>

Mat getMax(Mat src)
{
    int row = src.rows;
    int col = src.cols;
    Mat dst(row, col, CV_8UC1);
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            int b = src.at<Vec3b>(i, j)[0];
            int g = src.at<Vec3b>(i, j)[1];
            int r = src.at<Vec3b>(i, j)[2];
            int maxx = max(r, max(g, b));
            dst.at<uchar>(i, j) = maxx;
        }
    }
    return dst;
}
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

uchar Restoration::GetPixel(IplImage *img,int steps ,int channels,int num)//steps 行 channels 列
{
    return img->imageData[(img->widthStep * steps) + (img->nChannels * channels) + num];
}
void Restoration::ChangePixel(IplImage* img, int steps,  int channels, int num ,float ch) {
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
    float v1, v2, v3;
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
 
   
    FastFilter(temp, sigma);    //temp 是FXY
    
    temp.convertTo(fA, CV_32FC1);  
  
    img->convertTo(fB, CV_32FC1);//fB 是原图
    absdiff(fA, fB, fC); //fC是 Dxy
    
    fC.convertTo(fC, CV_8UC1);
    FastFilter(fC, sigma); //wxy = dxy *gxy
    

    Scalar ss = sum(fC);
    double normalize = 1.0 / ss[0];
   
    fC.convertTo(fC, CV_32FC1,normalize );  //wxy
    
    fC = fC.mul(fB) + (1 - fC).mul(fA);
    
   

    fC.convertTo(*img, CV_32FC1);
    
    
}

Mat Restoration::Illumination(Mat src, double sigma)
{
    Mat temp = src.clone();
    Illumination(&temp,sigma);
   
    return temp;
}//返回的是CV_32FC1

Mat Restoration::Decomposition(Mat src, Mat dst)
{
    int row = src.rows;
    int col = src.cols;
    Mat temp = Mat(row, col, CV_32FC3);
    //cout << "right" << endl;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            float b = (float)src.at<Vec3b>(i, j)[0] / (float)dst.at<float>(i, j);
            float g = (float)src.at<Vec3b>(i, j)[1] / (float)dst.at<float>(i, j);
            float r = (float)src.at<Vec3b>(i, j)[2] / (float)dst.at<float>(i, j);
            temp.at<Vec3f>(i, j)[0] = b;
            temp.at<Vec3f>(i, j)[1] = g;
            temp.at<Vec3f>(i, j)[2] = r;

        }
    }
    //temp.convertTo(temp, CV_8UC3, 255);
    return temp;


}
Mat Restoration::GammaCorrection(Mat src)
{
    int row = src.rows;
    int col = src.cols;
    float average = mean(src)[0];// m

    Mat out(row, col, CV_32FC1);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            float gamma = (src.at<float>(i, j) + average + 1) / (1 + 1 + average);
            out.at<float>(i, j) = powf(src.at<float>(i, j), gamma);        }
    }

    return out;
}
Mat Restoration::e_hist(Mat src)
{
    int gray[256] = { 0 };  //记录每个灰度级别下的像素个数
    double gray_prob[256] = { 0 };  //记录灰度分布密度
    double pdf_MA[256] = { 0 };
    //double pdf_U[256] = { 1/256};
    double pdf_final[256] = { 0 };
    double pdf_mod[256] = { 0 };
    double gray_distribution[256] = { 0 };  //记录累计密度
    int gray_equal[256] = { 0 };  //均衡化后的灰度值
    double l = 1.0 / 256;
    double w = 0;
    int col = src.cols;
    int row = src.rows;
    int gray_sum = col * row;
    Mat output(row, col, CV_8UC1);
    //灰度个数统计
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            int value = (int)src.at<uchar>(i, j);
            gray[value]++;
        }
    }
    //计算概率密度
    for (int i = 0; i < 256; i++)
    {

        gray_prob[i] = ((double)gray[i] / gray_sum);

    }

    int h = 4;  //filter size of the moving average 
    for (int i = 0; i < 256; i++)  //pdf ma
    {
        double temp = 0;
        if (i + h >= 256)
        {
            h = 256 - i;
        }
        for (int j = 0; j < h; j++)
        {

            temp += gray_prob[j + i];
        }
        pdf_MA[i] = temp / h;


    }

    for (int i = 0; i < 256; i++)//pdf mod
    {
        if (pdf_MA[i] < l)
            pdf_mod[i] = pdf_MA[i];
        else pdf_mod[i] = l;
        //cout << pdf_mod[i] << endl;

    }



    for (int i = 0; i < 256; i++)//w
    {
        w += l - pdf_mod[i];

    }


    for (int i = 0; i < 256; i++)//pdf_final
    {
        pdf_final[i] = w * gray_prob[i] + (1 - w) * l;

    }


    gray_distribution[0] = gray_prob[0];
    for (int i = 1; i < 256; i++)
    {
        gray_distribution[i] = gray_distribution[i - 1] + pdf_final[i];

    }

    //重新计算均衡化后的灰度值，四舍五入。参考公式：(N-1)*T+0.5

    for (int i = 0; i < 256; i++)
    {
        gray_equal[i] = (255 * gray_distribution[i] + 0.5);


    }
    //直方图均衡化,更新原图每个点的像素值

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            output.at<uchar>(i, j) = gray_equal[(int)src.at<uchar>(i, j)];
        }
    }


    return output;
}

Mat Restoration::merge_(Mat src,double sigma)
{
    Mat now = RGB2HSV(src);
    vector<Mat> v_channel;
    split(now, v_channel);
    Mat H = v_channel.at(0);
    Mat S = v_channel.at(1);
    Mat V = v_channel.at(2);
    Restoration res;
    V.convertTo(V, CV_8UC1, 255);
    Mat dst(V.rows, V.cols, CV_32FC1);
    dst = res.Illumination(V, sigma);
    for (int i = 0; i < dst.rows; i++) {
        for (int j = 0; j < dst.cols; j++) {
            dst.at<float>(i, j) = dst.at<float>(i, j) / 255;
        }
    }
    dst = res.GammaCorrection(dst);
    dst.convertTo(dst, CV_8UC1, 255);

    Mat hist = res.e_hist(dst);
    hist.convertTo(hist, CV_32FC1, 1.0 / 255);

    vector <Mat> v;
    v.push_back(H);
    v.push_back(S);
    v.push_back(hist);
    Mat merge_;
    merge(v, merge_);
    Mat result = HSV2RGB(merge_);

    return result;
}

Mat Restoration::merge_2(Mat src, double sigma)
{
    Restoration res;
    Mat now = RGB2HSV(src);
    vector<Mat> v_channel;
    split(now, v_channel);
    Mat H = v_channel.at(0);
    Mat S = v_channel.at(1);
    Mat V = v_channel.at(2);

 

    V.convertTo(V, CV_8UC1, 255);
    Mat ill = res.Illumination(V, sigma);//ILL FMXY
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
 




    return output;
}