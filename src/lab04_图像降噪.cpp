#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;
using namespace cv;
void getZ(const Mat &roi, int &Z_min, int &Z_max, int &Z_med) {
	vector<uchar> vec;
	for (auto iter = roi.begin<uchar>(); iter != roi.end<uchar>(); ++iter) {
		vec.push_back(*iter);
	}
	sort(vec.begin(), vec.end());

	Z_min = vec.front();
	Z_max = vec.back();
	Z_med = vec[roi.rows * roi.cols / 2];
}

Mat adaptiveMedian(const Mat &srcImg, int n_max) {
	Mat retImg = srcImg.clone();
	int rows = srcImg.rows;
	int cols = srcImg.cols;

	for (int x = n_max / 2; x < rows - n_max / 2; ++x) {
		for (int y = n_max / 2; y < cols - n_max / 2; ++y) {
			//每个像素都从模板为3开始
			for (int n = 3; n <= n_max; n += 2) {
				int k = n / 2;
				Mat roi = srcImg(Range(x - k, x + k), Range(y - k, y + k));
				int Z_min, Z_max, Z_med;
				int Z_xy = srcImg.at<uchar>(x, y);
				getZ(roi, Z_min, Z_max, Z_med);

				//达到最大了, 直接赋值Z_med
				if (n == n_max) {
					retImg.at<uchar>(x, y) = Z_med;
					break;
				}

				//
				if (Z_med - Z_min == 0 || Z_med - Z_max == 0) { //中值是脉冲(为最大或最小值), 增加窗口
					continue; // 增加n
				}
				else {    //中值不是脉冲时考虑中点的值
					if (Z_xy - Z_min > 0 && Z_xy - Z_max < 0) //中心点不是脉冲, 不变
						retImg.at<uchar>(x, y) = Z_xy;
					else
						retImg.at<uchar>(x, y) = Z_med;
					break;
				}
			}
		}
	}
	return retImg;
}


//算数平均
Mat meanFilter(Mat src, int moudelInt);
//几何平均
Mat geometricFilter(Mat src, int moudelInt);
//谐波均值
Mat harmonicFilter(Mat src, int moudelInt);
//逆谐波均值 Q是滤波器阶数 |Q=0 谐波均值 |Q=-1 算数均值 |Q为正数消除胡椒噪声 为负数消除盐粒噪声 
Mat In_harmonicFilter(Mat src, int moudelInt, double Q);
//中值滤波
Mat medianFilter(Mat src, int moudelInt);
//自适应均值滤波 overall噪声的方差
Mat adaptiveMeanFilter(Mat src, int moudelInt, double overall);
//自适应中值滤波 maxMoudel 最大窗口大小 7
Mat adaptiveMedianFilter(Mat src, int moudelInt);
//求中值 n 模板大小
int getMedian(int n, int* array);


int main(int argc, char** argv)
{

	Mat src_g = imread("D:/workspace/images/saltnoise.jpg", 0);//灰度图像
	Mat src_color = imread("D:/workspace/images/saltnoise.jpg", 1);//彩色图像
	//处理灰度图像
	namedWindow("src_grey", 1);
	imshow("src_grey", src_g);

	//算数均值
	Mat src_mean = meanFilter(src_g, 7);
	namedWindow("mean", 1);
	imshow("mean", src_mean);
	//几何均值
	//Mat src_geome = geometricFilter(src_g, 5);
	//namedWindow("geometric", 1);
	//imshow("geometric", src_geome);
	//谐波
	Mat src_har = harmonicFilter(src_g, 5);
	namedWindow("harmonic", 1);
	imshow("harmonic", src_har);
	//逆谐波 In_harmonicFilter(Mat src,int moudelInt,double Q); 
	//Mat src_Inhar = In_harmonicFilter(src_g, 7, 0);
	//namedWindow("inhar", 1);
	//imshow("inhar", src_Inhar);
	//中值滤波  medianFilter
	Mat src_median = medianFilter(src_g, 7);
	namedWindow("median", 1);
	imshow("median", src_median);
	imwrite("../median.jpg", src_median);

	//自适应均值滤波 overall噪声的方差 Mat adaptiveMeanFilter(Mat src,int moudelInt,double overall);
	Mat src_adMean = adaptiveMedian(src_g,  7);
	//Mat src_adMean = adaptiveMeanFilter(src_g, 7, 0.1);
	namedWindow("adMean", 1);
	imshow("adMean", src_adMean);
	//自适应中值滤波 Mat adaptiveMedianFilter(Mat src,int moudelInt);
	//Mat src_adMedian = adaptiveMedianFilter(src_g, 7);
	//namedWindow("adMedian", 1);
	//imshow("adMedian", src_adMedian);

	waitKey(0);
	return 0;
}


/************************************************A P I***********************************************************/
//Mat扩张，避免模板在矩阵[0,0]以及类似边缘位置时无法操作
Mat expandsrc(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "读取图像失败！" << endl;
		//return 失败信息并阻止继续执行
	}
	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	int expand = 2 * controller;
	//扩张这个Mat 避免对边缘进行操作
	//在src_clone外部套上几行几列
	int r = nRows + expand;
	int c = nCols + expand;
	Mat src_expand = Mat::zeros(r, c, CV_8UC1);//初始化扩张Mat初始值0
	int x, y = 0;
	for (x = 0; x<nRows; ++x)
	{
		for (y = 0; y<nCols; ++y)
		{
			src_expand.at<uchar>(x + controller, y + controller) = src_clone.at<uchar>(x, y);
		}
	}
	//把外层嵌套的0全部改为和边框相同的值
	//todo......

	return src_expand;

}



//src 源图像 moudelInt模板大小 为奇数
//算数均值滤波 模板大小5×5 为了获得较好的去噪效果建议不要调大小
Mat meanFilter(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "读取图像失败！" << endl;
		//return 失败信息并阻止继续执行
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//对src_expand进行计算，把计算结果写入src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//计算内部小矩阵
			int sum = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					sum += src_expand.at<uchar>(inneri, innerj);
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = sum / (moudelInt*moudelInt);
			sum = 0;
		}
	}
	return src_clone;
}


//几何均值滤波 模板大小固定5×5
Mat geometricFilter(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "读取图像失败！" << endl;
		//return 失败信息并阻止继续执行
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//对src_expand进行计算，把计算结果写入src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//计算内部小矩阵
			double sum = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					sum *= pow(src_expand.at<uchar>(inneri, innerj), 1.0 / 25.0);//这里的计算有丢失 结果全部是0
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = sum;
			sum = 0;
		}
	}
	return src_clone;
}

//谐波均值滤波 模板大小5×5 为了获得较好的去噪效果建议不要调大小
//适合处理盐噪声 不适合胡椒噪声
Mat harmonicFilter(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "读取图像失败！" << endl;
		//return 失败信息并阻止继续执行
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//对src_expand进行计算，把计算结果写入src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//计算内部小矩阵
			double sum = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					sum += 1.0 / src_expand.at<uchar>(inneri, innerj);
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = 25 / sum;
			sum = 0;
		}
	}
	return src_clone;
}
//逆谐波均值 Q是滤波器阶数
Mat In_harmonicFilter(Mat src, int moudelInt, double Q)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "读取图像失败！" << endl;
		//return 失败信息并阻止继续执行
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//对src_expand进行计算，把计算结果写入src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//计算内部小矩阵
			double sum1 = 0;
			double sum2 = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					sum1 += pow(src_expand.at<uchar>(inneri, innerj), Q);
					sum2 += pow(src_expand.at<uchar>(inneri, innerj), Q + 1);
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = sum2 / sum1;
			sum1 = 0;
			sum2 = 0;
		}
	}
	return src_clone;
}
//中值滤波 模板大小5×5 7×7
Mat medianFilter(Mat src, int moudelInt)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "读取图像失败！" << endl;
		//return 失败信息并阻止继续执行
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	//int expand = 2 * controller;

	Mat src_expand = expandsrc(src_clone, moudelInt);

	//对src_expand进行计算，把计算结果写入src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	int size = moudelInt*moudelInt;
	int array[49] = { 0 };
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			//寻找内部小矩阵的中值

			int row = 0;
			int col = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{

					row = inneri - i + controller;
					col = innerj - j + controller;

					int temp = src_expand.at<uchar>(inneri, innerj);


					array[row*moudelInt + col] = src_expand.at<uchar>(inneri, innerj);	//将矩阵放入数组
				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = getMedian(moudelInt, array);//求中值
		}
	}
	return src_clone;
}


//自适应均值滤波 模板大小7×7 overall噪声的全局方差
Mat adaptiveMeanFilter(Mat src, int moudelInt, double overall)
{
	Mat src_clone = src.clone();
	int controller = moudelInt / 2;
	if (!src_clone.data)
	{
		cout << "读取图像失败！" << endl;
		//return 失败信息并阻止继续执行
	}

	int nRows = src_clone.rows;
	int nCols = src_clone.cols;
	Mat src_expand = expandsrc(src_clone, moudelInt);
	//对src_expand进行计算，把计算结果写入src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	for (i = controller; i<nRows + controller; ++i)
	{
		for (j = controller; j<nCols + controller; ++j)
		{
			////计算Ml 局部均值 噪声的局部方差 像素的局部方差
			double average = 0;
			double variance = 0;
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{

					//局部均值Ml
					average += src_expand.at<uchar>(inneri, innerj) / (moudelInt*moudelInt);

				}
			}
			for (inneri = i - controller; inneri <= i + controller; ++inneri)
			{
				for (innerj = j - controller; innerj <= j + controller; ++innerj)
				{
					//局部方差
					int temp = (src_expand.at<uchar>(inneri, innerj) - average);
					variance += pow(temp, 2) / (moudelInt*moudelInt);

				}
			}
			src_clone.at<uchar>(i - controller, j - controller) = src_clone.at<uchar>(i, j) - (overall / variance)*(src_clone.at<uchar>(i, j) - average);
		}
	}
	return src_clone;
}
//逻辑很混乱 但是我已经改不动了
//自适应中值滤波  模板最大大小 固定为7
Mat adaptiveMedianFilter(Mat src, int moudelInt)
{
	//因为模板的扩张未知，所以一开始原始图像的扩张要以模板的最大值来扩张
	Mat src_c = src.clone();
	if (!src_c.data)
	{
		cout << "读取图像失败！" << endl;
		//return 失败信息并阻止继续执行
	}

	int nRows = src_c.rows;
	int nCols = src_c.cols;
	int windowSize = 7;
	Mat src_expand = expandsrc(src_c, 7);//按照最大模板大小扩张
	Mat src_clone = src_expand;//clone画布和expand大小一致 方便写入clone

							   //对src_expand进行计算，把计算结果写入src_clone
	int i, j = 0;
	int inneri, innerj = 0;
	//int count = 0;//计数 测试
	int row = 0;
	int col = 0;
	int min = 0;
	int max = 0;
	int median = 0;

	for (i = 3; i<nRows + 3; ++i)
	{
		for (j = 3; j<nCols + 3; ++j)
		{
		dy_expand:
			int controller = windowSize / 2;
			if (windowSize <= 7) {
				//动态变换内部小矩阵的大小
				int length = windowSize*windowSize;
				int array[49] = { 0 };
				int current = src_expand.at<uchar>(i, j);
				for (inneri = i - controller; inneri <= i + controller; ++inneri)
				{
					for (innerj = j - controller; innerj <= j + controller; ++innerj)
					{

						//cout<<"inneri:innerj"<<inneri<<":"<<innerj<<endl;
						row = inneri - i + controller;
						col = innerj - j + controller;
						int temp = src_expand.at<uchar>(inneri, innerj);
						//cout<<"row:"<<row<<"col:"<<col<<endl;
						//cout<<"value"<<temp<<endl;
						array[row*windowSize + col] = src_expand.at<uchar>(inneri, innerj);	//将矩阵放入数组	
					}
				}
				//寻找arry的最大值 最小值 中值	
				int i1 = 0;
				int j1 = 0;
				for (i1 = 0; i1<length; ++i1)
				{
					for (j1 = i1 + 1; j1<length; ++j1)
					{
						if (array[j1]<array[i1])
						{
							int temp = 0;
							temp = array[i1];
							array[i1] = array[j1];
							array[j1] = temp;
						}
					}
				}
				min = array[0];
				max = array[length - 1];
				median = array[length / 2];
				current = src_expand.at<uchar>(i, j);

				//模板最大为7×7
				/*
				A: A1=med-min
				A2=med-max
				if(A1>0A2<0) 转到B 否则增大窗口尺寸
				if窗口尺寸小于等于最大值 重复A 否则输出med
				B: B1=current-min
				B2=current-max
				if(B1>0B2<0)输出current
				else输出med
				*/
				if (median - min>0 && median - max<0)
				{
					//cout<<"中值在min和max之间——>B"<<endl;
					//STAR B----------------------------------------
					if (current - min>0 && current - max<0)
					{
						//cout<<"min<current<max current"<<endl;
						src_clone.at<uchar>(i, j) = current;//写入的位置要想清楚
						windowSize = 3;

					}
					else {
						//cout<<"current不在[min,max] median"<<endl;
						src_clone.at<uchar>(i, j) = median;
						windowSize = 3;

					}
					//END B-----------------------------------------
				}
				else if (windowSize <= 5) {
					//cout<<"中值不在min和max之间"<<endl;

					//窗口增大
					windowSize = windowSize + 2;
					goto dy_expand;

				}
				else {
					//cout<<"窗口已最大 输出median"<<endl;
					src_clone.at<uchar>(i, j) = median;
					windowSize = 3;
				}
			}
		}
	}
	return src_clone;
}

//对规模为n的矩阵排序 并返回中值
int getMedian(int n, int* array)
{
	int length = n * n;
	int i = 0;
	int j = 0;
	for (i = 0; i<length; ++i)
	{
		for (j = i + 1; j<length; ++j)
		{
			if (array[j]<array[i])
			{
				int temp = 0;
				temp = array[i];
				array[i] = array[j];
				array[j] = temp;
			}
		}
	}
	int median = array[length / 2];
	return median;
}





