#include <iostream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
using namespace cv;
using namespace std;


void rotateAndZoom(Mat img, const Point &center, double angle, double scale);
void myRotate(Mat img, double angle);
void myZoom(Mat img, double scale);
//2. 利用canny进行边缘检测
void cannyTest(Mat grayImg, int edgeThresh);
//3. 角点检测
void harrisTest(Mat grayImg, Mat img, int thresh);
void showImg(const string& name, const Mat& img);
void showImg(const string& name, const Mat& img) {
	namedWindow(name);
	imshow(name, img);
}

void rotateAndZoom(Mat img, const Point &center, double angle, double scale) {
	Mat rotateMat = getRotationMatrix2D(center, angle, scale);
	Mat rotatedImg = Mat::zeros(img.rows, img.cols, img.type());
	warpAffine(img, rotatedImg, rotateMat, Size(img.cols, img.rows));

	showImg("旋转后", rotatedImg);
}

void myRotate(Mat img,double angle) {
	int x, y;
	const double pi = 4 * atan(1);
	angle = angle / 360.0 * 2 * pi;
	Mat rotatedImg = Mat::zeros(img.rows, img.cols, img.type());
	for (int i = 0; i < img.cols; ++i) {
		for (int j = 0; j < img.rows; ++j) {
			x = i * cos(angle) - j * sin(angle);
			y = j * cos(angle) + i * sin(angle);
			if (x >= 0 && x < img.cols && y >= 0 && y < img.rows) {
				rotatedImg.at<Vec3b>(x, y) = img.at<Vec3b>(i, j);
			}
		}
	}
	showImg("旋转后", rotatedImg);
}

void myZoom(Mat img, double scale) {
	Mat zoomImg = Mat::zeros(int(img.rows * scale), int(img.cols * scale), img.type());
	if (scale - 1.0 > 0.0000001) {

	}
	else {

	}
	showImg("缩放后的", zoomImg);
}

void cannyTest(Mat grayImg, int edgeThresh = 1) {
	Mat blurImage, cedge;
	const char* window_name = "Canny边缘图";
	namedWindow(window_name, 1);
	blur(grayImg, blurImage, Size(3, 3));
	Canny(blurImage, cedge, edgeThresh, edgeThresh * 3, 3);
	imshow(window_name, cedge);
}

void harrisTest(Mat grayImg, Mat img, int thresh = 100) {
	Mat dst = Mat::zeros(grayImg.size(), CV_32FC1);
	cornerHarris(grayImg, dst, 2, 3, 0.04);                     //角点检测
	normalize(dst, dst, 0, 255, NORM_MINMAX, CV_32FC1, Mat());  //规范值的范围
	convertScaleAbs(dst, dst);                                  //缩放并计算绝对值

	Mat resultImg = img.clone();
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			if ((int)dst.at<float>(i, j) > thresh) {
				circle(resultImg, Point(j, i), 5, Scalar(0, 255, 0), 2, 8, 0);//绘点
			}
		}
	}
	showImg("Harris角点", resultImg);
	waitKey();
}
int main() {
	Mat src = imread("D:/workspace/images/smallCat.jpg", 1);
	Mat gray = imread("D:/workspace/images/smallCat.jpg", 0);
	if (!src.data) {
		cout << "can not load img" << endl;
	}
	Mat src1 = src.clone();
	Mat src2 = src.clone();
	Mat src3 = src.clone();
	//harrisTest(gray, src, 100);
	//myRotate(src1, 45);
	//myZoom(src, 0.5);
	rotateAndZoom( src3, Point(10,10),45, 2);
	cannyTest(gray, 1);
	//harrisTest(gray, src, 100);
	waitKey(0);
	return 0;
}