#include <opencv2/opencv.hpp>
#include <highgui.h>
#include <iostream>

using namespace cv;
using namespace std;
int main(int argc, char** argv) {
	cout << "hello" << endl;
	Mat src, dst;
	src = imread("D:/workspace/images/cat01.jpg", 1); //��ͼ���Բ�ɫͼ����
	if (!src.data) {
		printf("could not load image...\n");
		return -1;

	}
	
	Mat dst_smallize;
	resize(src, dst_smallize, Size(src.cols / 4, src.rows / 4), 0, 0, INTER_LINEAR);
	namedWindow("smallizeImg", 0);
	imshow("smallizeImg", dst_smallize);
	imwrite("D:/workspace/images/smallCat.jpg", dst_smallize);
	namedWindow("input window", 0);
	imshow("input window", src);
	
	waitKey(0);
	return 0;
}