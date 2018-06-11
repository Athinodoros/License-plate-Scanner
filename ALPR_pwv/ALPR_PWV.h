#pragma once


#ifndef MYCVAPI;

#define MYCVAPI __declspec(dllexport)

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\objdetect.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <windows.h>
#include <sstream>
#include <vector>
#include <iterator>
#include <fstream>



using namespace cv;

using namespace std;
cv::Mat _currentFrame;





extern "C" void MYCVAPI GetRawImage(unsigned char* data, int width, int height)
{
	cv::Mat resizedMat(height, width, _currentFrame.type());
	cv::resize(_currentFrame, resizedMat, resizedMat.size(), cv::INTER_CUBIC);
	cv::Mat argbImg;
	cv::cvtColor(resizedMat, argbImg, CV_BGR2RGBA);
	std::memcpy(data, argbImg.data, argbImg.total() * argbImg.elemSize());
}


/*

uchar*  MYCVAPI test(void)
{
	Mat OriginalImg = imread("c:\\20100812133.jpg", 0);
	threshold(OriginalImg, OriginalImg, 0, 255, THRESH_OTSU);
	cvtColor(OriginalImg, OriginalImg, CV_GRAY2BGR);
	return OriginalImg.data;
}
*/

typedef struct 
{
	//char *plate;
	unsigned int y1;
	unsigned int x1;
	unsigned int y2;
	unsigned int x2;
	bool hasPlateOn;
	unsigned int size;
} Sframe;

extern "C" void MYCVAPI findPossiblePlatesClassifier(Mat& in, Mat& out); // Specify the API NA
MYCVAPI void read_directory(const std::string& name, vector<std::string>& v);
MYCVAPI bool verifySizes(RotatedRect candidate);
MYCVAPI void GammaCorrection(Mat& src, Mat& dst, float fGamma);
//MYCVAPI void findPossiblePlates(Mat& in, Mat& out);

extern "C" Sframe MYCVAPI findPossiblePlatesSF(Mat in);
//extern "C" int MYCVAPI findPossiblePlates(int in);


extern "C" int MYCVAPI testDll(int a, int b) { return a + b; };


extern "C" Sframe  MYCVAPI  TestArrayPara(BYTE * pArray, int width, int height)
{
	
	Mat test1 = Mat(height+1, width+1, CV_8UC3, pArray).clone();
	try {
		Sframe ret = findPossiblePlatesSF(test1);
		return ret;
	}
	catch (Exception e) {
		Sframe error ;
		error.hasPlateOn = false;
		return error;
	}	
}


struct rigidTransform
{
	rigidTransform(double eular_angle_x, double eular_angle_y, double eular_angle_z, double eular_dis_x, double eular_dis_y, double eular_dis_z) : Eular_angle_x(eular_angle_x), Eular_angle_y(eular_angle_y), Eular_angle_z(eular_angle_z), Eular_dis_x(eular_dis_x), Eular_dis_y(eular_dis_y), Eular_dis_z(eular_dis_z) {}
	double Eular_angle_x;
	double Eular_angle_y;
	double Eular_angle_z;

	double Eular_dis_x;
	double Eular_dis_y;
	double Eular_dis_z;
};
struct Color32
{
	uchar r;
	uchar g;
	uchar b;
	uchar a;
};

extern "C" Sframe MYCVAPI findPose(Color32* img1, Color32* img2, int width, int height)
{
	Mat Img1(height, width, CV_8UC4, img1), Img2(height, width, CV_8UC4, img2);
	Sframe ret = findPossiblePlatesSF(Img1);
	return ret;

}
class ALPR_PWV
{
public:
	ALPR_PWV();
	~ALPR_PWV();
};
#endif 