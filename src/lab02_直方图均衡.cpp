#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#define L 256

using namespace std;
using namespace cv;

void hist(IplImage* src, IplImage* dst);

int main(int argc, char** argv) {
	IplImage* src = cvLoadImage("D:/workspace/images/cat01.jpg", 3);
	if (src == NULL) {
		cout << "load image failed";
		return -1;
	}
	IplImage* dst = cvCreateImage(cvSize(src->width, src->height), 8, 3);
	hist(src, dst);
	namedWindow("原图", 0);
	namedWindow("直方图均衡", 0);
	cvShowImage("原图", src);
	cvShowImage("直方图均衡", dst);
	waitKey(0);
	return 0;
}


void hist(IplImage* src, IplImage* dst) {
	int n[] = { L, L, L };
	int r[256] = { 0 }, g[256] = { 0 }, b[256] = { 0 };
	
	int width = src->width;
	int height = src->height;
	int sum = width * height;
	int i, j;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			b[((uchar*)(src->imageData + i*src->width))[j*src->nChannels + 0]]++;
			g[((uchar*)(src->imageData + i*src->width))[j*src->nChannels + 0]]++;
			r[((uchar*)(src->imageData + i*src->width))[j*src->nChannels + 0]]++;
		}
	}

	//构建直方图累计分布方程，对直方图进行均衡化
	double val[3] = { 0 };
	for (int i = 0; i < L; i++) {
		val[0] += b[i];
		val[1] += g[i];
		val[2] += r[i];

		b[i] = val[0] * (L - 1) / sum;
		g[i] = val[1] * (L - 1) / sum;
		r[i] = val[2] * (L - 1) / sum;
	}

	//归一化直方图
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			((uchar*)(dst->imageData +  i*dst->widthStep))[j*dst->nChannels + 0] = 
				b[((uchar*)(src->imageData + i*src->widthStep))[j*src->nChannels + 0]];
			((uchar*)(dst->imageData + i*dst->widthStep))[j*dst->nChannels + 1] =
				g[((uchar*)(src->imageData + i*src->widthStep))[j*src->nChannels + 1]];
			((uchar*)(dst->imageData + i*dst->widthStep))[j*dst->nChannels + 2] = 
				r[((uchar*)(src->imageData + i*src->widthStep))[j*src->nChannels + 2]];

		}
	}

}



