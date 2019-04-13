#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include "highgui.h"
using namespace cv;
using namespace std;
//设定阈值，使用迭代法
int DetectThreshold(IplImage* src);
//二值化函数
Mat toBinary(Mat src, int threshold);
Mat toLog(Mat src);
Mat toGamma(Mat src, float gamma);
Mat RGBtoCMY(Mat src);


int main(int argc, char** argv) {
	//一、图像显示：方法1
	Mat src, dst;
	src = imread("D:/workspace/images/smallCat.jpg", 1); //将图像以彩色图像导入
	if (!src.data) {
		printf("could not load image...\n");
		return -1;

	}
	cout << "图像的通道数: " << src.channels() << endl;
	Mat dst_smallize;
	namedWindow("input window", 0);
	imshow("input window", src);
	

	/*二、二值化处理
	*阈值选取方法：
	*1，双峰法
	*2，P参数法
	*3，大津法
	*4，最大熵阈值法
	*5，迭代法√
	**/
	//int thresh1 = DetectThreshold(cvLoadImage("D:/workspace/images/cat01.jpg"));
	Mat gray;
	cvtColor(src, gray,COLOR_BGR2GRAY);  //转换为灰度图像然后二值化
	namedWindow("gray", 0);
	imshow("gray", gray);
	dst = toBinary(gray, 179);
	//threshold(gray, dst, 100, 200, THRESH_BINARY_INV);
	namedWindow("二值图", 0);
	imshow("二值图", dst);


	/*
	*三、灰度图像对数变换：
	*	设置并调整r值对图像进行对数变换
	*	公式：s = log（1+r）
	*/
	Mat result = toLog(src);
	namedWindow("对数变换", 0);
	imshow("对数变换", result);


	/*
	*四、伽马变换
	*	设置并调整伽马值对图像进行伽马变换
	*
	*/
	Mat result_gamma = toGamma(src, 3.0);
	namedWindow("伽马变换", 0);
	imshow("伽马变换", result_gamma);

	/*
	*五、补色变换
	*	对彩色图像进行补色变换
	*	公式 s = 255-i
	*/
	Mat result_cmy = RGBtoCMY(src);
	namedWindow("补色变换", 0);
	imshow("补色变换", result_cmy);

	while (1) {
		if (cvWaitKey(100) == 27)break;
	}
	waitKey(0);
	exit(0);
}
Mat toBinary(Mat src, int threshold) {
	Mat result;
	int channels = src.channels();
	int rows = src.rows;
	int cols = src.cols*channels;
	int j;
	uchar* p;
	uchar* r;
	p = src.ptr<uchar>(0);
	r = result.ptr<uchar>(0);
	for (j = 0; j < cols*rows; j++) {
		if (p[j] > threshold) {
			p[j] = 255;
		}
		else {
			p[j] = 0;
		}
	}
	return src;
}

Mat toLog(Mat src) {
	double pixels[256];
	for (int i = 0; i < 256; i++)
		pixels[i] = log(1 + i);

	Mat result(src.size(), CV_32FC3);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			result.at<Vec3f>(i, j)[0] = pixels[src.at<Vec3b>(i, j)[0]];
			result.at<Vec3f>(i, j)[1] = pixels[src.at<Vec3b>(i, j)[1]];
			result.at<Vec3f>(i, j)[2] = pixels[src.at<Vec3b>(i, j)[2]];

		}
	}

	normalize(result, result, 0, 255, CV_MINMAX);
	convertScaleAbs(result, result);
	return result;
}

Mat toGamma(Mat src, float gamma) {
	double pixels[256];
	for (int i = 0; i < 256; i++)
		pixels[i] = pow(i, gamma);
	Mat result(src.size(), CV_32FC3);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			result.at<Vec3f>(i, j)[0] = pixels[src.at<Vec3b>(i, j)[0]];
			result.at<Vec3f>(i, j)[1] = pixels[src.at<Vec3b>(i, j)[1]];
			result.at<Vec3f>(i, j)[2] = pixels[src.at<Vec3b>(i, j)[2]];
		}
	}

	normalize(result, result, 0, 255, CV_MINMAX);
	convertScaleAbs(result, result);
	return result;
}

Mat RGBtoCMY(Mat src) {
	double pixels[256];
	for (int i = 0; i < 256; i++)
		pixels[i] = 255 - i;
	Mat result(src.size(), CV_32FC3);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			result.at<Vec3f>(i, j)[0] = pixels[src.at<Vec3b>(i, j)[0]];
			result.at<Vec3f>(i, j)[1] = pixels[src.at<Vec3b>(i, j)[1]];
			result.at<Vec3f>(i, j)[2] = pixels[src.at<Vec3b>(i, j)[2]];
		}
	}

	normalize(result, result, 0, 255, CV_MINMAX);
	convertScaleAbs(result, result);
	return result;
}

//迭代法
int DetectThreshold(IplImage* src) {
	uchar iThrehold;//阈值
	try {
		int height = src->height;
		int width = src->width;
		int step = src->widthStep / sizeof(uchar);//相邻行的同列点之间的字节数
		uchar *data = (uchar*)src->imageData;

		int iDiffRec = 0;
		int F[256] = { 0 }; //直方图数组  
		int iTotalGray = 0;//灰度值和  
		int iTotalPixel = 0;//像素数和  
		uchar bt;//某点的像素值  

		uchar iNewThrehold;//新阀值
		uchar iMaxGrayValue = 0, iMinGrayValue = 255;//原图像中的最大灰度值和最小灰度值  
		uchar iMeanGrayValue1, iMeanGrayValue2;
		//获取(i,j)的值，存于直方图数组F  
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				bt = data[i*step + j];
				if (bt < iMinGrayValue)
					iMinGrayValue = bt;
				if (bt > iMaxGrayValue)
					iMaxGrayValue = bt;
				F[bt]++;
			}
		}
		iThrehold = 0;
		iNewThrehold = (iMinGrayValue + iMaxGrayValue) / 2;//初始阀值  
		iDiffRec = iMaxGrayValue - iMinGrayValue;
		for (int a = 0; (abs(iThrehold - iNewThrehold) > 0.5); a++)//迭代中止条件  
		{
			iThrehold = iNewThrehold;
			//小于当前阀值部分的平均灰度值  
			for (int i = iMinGrayValue; i < iThrehold; i++)
			{
				iTotalGray += F[i] * i;//F[]存储图像信息  
				iTotalPixel += F[i];
			}
			iMeanGrayValue1 = (uchar)(iTotalGray / iTotalPixel);
			//大于当前阀值部分的平均灰度值  
			iTotalPixel = 0;
			iTotalGray = 0;
			for (int j = iThrehold + 1; j < iMaxGrayValue; j++)
			{
				iTotalGray += F[j] * j;//F[]存储图像信息  
				iTotalPixel += F[j];
			}
			iMeanGrayValue2 = (uchar)(iTotalGray / iTotalPixel);

			iNewThrehold = (iMeanGrayValue2 + iMeanGrayValue1) / 2; //新阀值  
			iDiffRec = abs(iMeanGrayValue2 - iMeanGrayValue1);
		}
	}
	catch (cv::Exception e)
	{
	}

	return iThrehold;
}

/*Mat binary_convert(Mat src, uchar iThreshold) {
	Mat result = src;
	int row = result.rows;
	int col = result.cols;
	for (int i = 0; i <= row; i++) {
		for (int j = 0; j <= col; j++) {
			result[i][j] = result[i][j] > iThreshold ? 255 : 0;
		}
	}
	return result;
}*/

