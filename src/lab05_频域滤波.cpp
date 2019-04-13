#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>

using namespace cv;
using namespace std;


Mat DFT(const Mat& image);
void genIPF(Mat& H);
Mat IDFT(const Mat& image);
Mat IPF_Filter(const Mat& image);
Mat BPF_Filter(const Mat& image);
//D0截至频率  t通过系数
static int D0, t;
static int unq_n;


int main(int argc, char** argv)
{
	
	Mat image = imread("D:/workspace/images/smallCat.jpg", 0);
	if (!image.data)
	{
		printf("No image data \n");
		return -1;
	}
	//D0截至频率  t通过系数
	D0 = 0;
	t = 2;
	//截至频率
	unq_n = 0;
	Mat new_img;
	new_img = IPF_Filter(image);

	Mat bfp_img;
	bfp_img = BPF_Filter(image);
	namedWindow("Display img", 0);
	namedWindow("Display 理想高通/低通滤波", 0);
	namedWindow("Display 布特沃斯滤波", 0);
	//string name = "new_img";
	imshow("Display img", image);
	//理想高通/低通滤波
	imshow("Display 理想高通/低通滤波", new_img);
	//布特沃斯滤波
	imshow("Display 布特沃斯滤波", bfp_img);

	//imwrite(name, new_img);
	waitKey(0);
	return 0;
}
//理想高通/低通滤波器模板
void genIPF(Mat& H)
{
	int P = H.rows;
	int Q = H.cols;
	for (int u = 0; u<P; u++)
		for (int v = 0; v<Q; v++)
			H.at<float>(u, v) = sqrt(pow(u - P / 2, 2) + pow(v - Q / 2, 2))>D0 ? abs(0 - t) : abs(1 - t);
}
//布特沃斯滤波器
void genBPF(Mat& I)
{
	int P = I.rows;
	int Q = I.cols;
	for (int u = 0; u<P; ++u)
		for (int v = 0; v<Q; ++v)
			if (t == 0)
				I.at<float>(u, v) = 1 / (1 + pow(sqrt(pow(u - P / 2, 2) + pow(v - Q / 2, 2)) / D0, 2 * unq_n));
			else
				I.at<float>(u, v) = 1 / (1 + pow(D0 / sqrt(pow(u - P / 2, 2) + pow(v - Q / 2, 2)), 2 * unq_n));
}

Mat DFT(const Mat& image) {
	int M = image.rows;
	int N = image.cols;
	int P = 2 * M, Q = 2 * N;

	//填充中心化图像
	Mat fill_img(P, Q, CV_32FC1);
	for (int i = 0; i<M; i++)
		for (int j = 0; j<N; j++)
			fill_img.at<float>(i, j) = (image.at<uchar>(i, j)) * pow(-1, i + j);

	for (int i = M; i<P; i++)
		for (int j = N; j<Q; j++)
			fill_img.at<float>(i, j) = 0;

	//实部（图像本身），虚部（全0),组合到一个Mat
	//Mat planes[] = {Mat_<float>(fill_img), Mat::zeros(fill_img.size(), CV_32F)};
	Mat combine(P, Q, CV_32FC2, Scalar(0, 0));
	for (int i = 0; i<P; i++)
		for (int j = 0; j<Q; j++)
			combine.at<Vec2f>(i, j)[0] = fill_img.at<float>(i, j);
	//对组合Mat进行dft
	dft(combine, combine);
	//对数变换，计算1+log|F|提高显示效果
	Mat f_chart(P, Q, CV_32FC1);
	for (int i = 0; i<P; i++)
		for (int j = 0; j<Q; j++)
			f_chart.at<float>(i, j) = 1 + log(sqrt(pow(combine.at<Vec2f>(i, j)[0], 2) + pow(combine.at<Vec2f>(i, j)[1], 2)));

	f_chart.convertTo(f_chart, CV_8UC1);
	normalize(f_chart, f_chart, 0, 255, NORM_MINMAX);
	namedWindow("Display f_chart", WINDOW_NORMAL);
	imshow("Display f_chart", f_chart);
	imwrite("f_chart.jpg", f_chart);
	return combine;
}

Mat IDFT(const Mat& image) {
	int P = image.rows;
	int Q = image.cols;
	Mat f(P, Q, CV_32FC1);
	dft(image, f, DFT_INVERSE | DFT_REAL_OUTPUT | DFT_SCALE);

	for (int i = 0; i<P; i++)
		for (int j = 0; j<Q; j++)
			f.at<float>(i, j) *= pow(-1, i + j);
	f.convertTo(f, CV_8UC1);
	return f;
}

Mat IPF_Filter(const Mat& image)
{
	Mat F = DFT(image);

	//生成滤波器
	int M = image.rows;
	int N = image.cols;
	int P = 2 * M, Q = 2 * N;
	Mat H(P, Q, CV_32FC1);
	genIPF(H);

	//滤波
	Mat G = F;
	for (int i = 0; i<P; i++)
		for (int j = 0; j<Q; j++) {
			//虚实部都要滤波
			G.at<Vec2f>(i, j)[0] = F.at<Vec2f>(i, j)[0] * H.at<float>(i, j);
			G.at<Vec2f>(i, j)[1] = F.at<Vec2f>(i, j)[1] * H.at<float>(i, j);
		}
	//逆变换并截取
	Mat new_img = IDFT(G)(Range(0, M), Range(0, N));
	return new_img;
}

Mat BPF_Filter(const Mat& image)
{
	Mat F = DFT(image);

	int M = image.rows;
	int N = image.cols;
	int P = 2 * M, Q = 2 * N;
	Mat H(P, Q, CV_32FC1);
	genBPF(H);

	Mat G = F;
	for (int i = 0; i<P; i++)
		for (int j = 0; j<Q; j++)
		{
			G.at<Vec2f>(i, j)[0] = F.at<Vec2f>(i, j)[0] * H.at<float>(i, j);
			G.at<Vec2f>(i, j)[1] = F.at<Vec2f>(i, j)[1] * H.at<float>(i, j);
		}
	Mat new_img = IDFT(G)(Range(0, M), Range(0, N));
	return new_img;
}



