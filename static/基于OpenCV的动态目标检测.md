# 基于OpenCV的动态目标检测

[TOC]

## 一、实验原理

### 1，基于Mog2框架的背景建模

> Mog2用的是自适应的高斯混合模型（Adaptive GMM，Gaussian Mixture Model），在OpenCV中开源了的几种背景提取算法MOG，MOG2，GMG的测试程序结果中，MOG2确实在前景连续性及运算时间上都脱颖而出

### 2， Mog2建模过程及各可调参数的意义

GMM是用多个高斯模型的加权和来表示，假定是M个高斯分量，讨论M的取值是MOG2的作者研究的一个重点：在他之前Stauffer & Grimson取固定个数的高斯分量(M=4)， Zoran则根据不同输入场景自动选择分量的个数。这样做的好处是在较简单的场景下，将只选出一个较重要的高斯分量，节省了后期更新背景时选属于哪一分量的时间，提高了速度。有两个测试结果为证：一是用OpenCV中测试程序对同一简单场景测试视频跑不同算法得到的运行时间如下表，明显mog2快很多。

![img](https://img-blog.csdn.net/20141204110225156)

#### 2.1**BackgroundSubtractorMOG2创建及初始化**

```c++
Ptr<BackgroundSubtractorMOG2> mog = createBackgroundSubtractorMOG2(100,25,false);
```

- **history**：用于训练背景的帧数，默认为500帧，如果不手动设置learningRate，history就被用于计算当前的learningRate，此时history越大，learningRate越小，背景更新越慢；
- **varThreshold**：方差阈值，用于判断当前像素是前景还是背景。一般默认16，如果光照变化明显，如阳光下的水面，建议设为25,36，具体去试一下也不是很麻烦，值越大，灵敏度越低；

- **detectShadows**：是否检测影子，设为true为检测，false为不检测，检测影子会增加程序时间复杂度，如无特殊要求，建议设为false；

#### 2.2**BackgroundSubtractorMOG2运动检测**

```c++
mog->apply(src_YCrCb, foreGround, 0.005);
```

- **image**: 源图
- **fmask**: 前景（二值图像）
- l**earningRate**: 学习速率，值为0-1,为0时背景不更新，为1时逐帧更新，默认为-1，即算法自动更新；

## 二、代码框架

```c++
#include <iostream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

void MOG2Test();
void MOG2Test() {
	//create Background Subtractor objects
	//实现BackgroundSubtractorMOG2的构造方法,实现前后背景分离
	//通常我们假定没有入侵物体的静态场景具有一些常规特性，
	//可以用一个统计模型描述。GMM就是用高斯模型，而且是多个高斯模型的加权和混合在一起来模拟背景的特性。
	//这样一旦已知这个背景模型，入侵物体就能通过标出场景图像中不符合这一背景模型的部分来检测到。
	Ptr<BackgroundSubtractor> pBackSub;
	pBackSub = createBackgroundSubtractorMOG2();
	//pBackSub = createBackgroundSubtractorKNN();
	VideoCapture capture("D:workspace/images/test.mp4");
	if (!capture.isOpened()) {
		//error in opening the video input
		cerr << "Unable to open mp4: " << endl;
		return;
	}
	Mat frame, fgMask;
	while (true) {
		capture >> frame;
		if (frame.empty())
			break;
		//update the background model
		pBackSub->apply(frame, fgMask);
		//get the frame number and write it on the current frame
		rectangle(frame, Point(10, 2), Point(100, 20), Scalar(255, 255, 255), -1);
		stringstream ss;
		ss << capture.get(CAP_PROP_POS_FRAMES);
		string frameNumberString = ss.str();
		putText(frame, frameNumberString.c_str(), Point(15, 15),
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0));
		//show the current frame and the fg masks
		imshow("Frame", frame);
		imshow("FG Mask", fgMask);
		//get the input from the keyboard
		int keyboard = waitKey(30);
		if (keyboard == 'q' || keyboard == 27)
			break;
	}
}

int main() {
	MOG2Test();
	return 0;
}
```



## 三、实验结果

![1555152681577](C:\Users\Jason\AppData\Roaming\Typora\typora-user-images\1555152681577.png)

![1555152708097](C:\Users\Jason\AppData\Roaming\Typora\typora-user-images\1555152708097.png)