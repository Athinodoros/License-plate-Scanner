#include "ALPR_PWV.h"



using namespace cv;
using namespace std;

Sframe sf;
Mat src_gray;
Mat new_image;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
Mat input;
Mat contFixed;
Mat src;

/// Function header
//void thresh_callback(int, void*);

int main() {
	return 0;
}
vector<string> get_all_files_names_within_folder(string folder)
{
	vector<string> names;
	string search_path = folder + "/*.*";
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}


void findPossiblePlates(Mat& in, Mat& out) {
	Mat input = in;
	//Mat gray = imread("testttt.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	Mat src = input;
	Mat contFixed;

	imshow("1 original", src);
	cvtColor(src, src, CV_BGR2GRAY);

	//imshow("2 gray scale", src);

	//imshow("bw", src);
	equalizeHist(src, src);

	equalizeHist(src, contFixed);

	//imshow("3 histogram equalized", src);
	blur(src, src, Size(20, 1));

	//imshow("4 blur ", src);

	Sobel(src, src, CV_8U, 1, 0, 3, 1, 0);

	//imshow("5 sobel", src);

	threshold(src, src, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	//imshow("6 thresshold ", src);
	Mat element = getStructuringElement(MORPH_RECT, Size(30, 10));
	morphologyEx(src, src, CV_MOP_CLOSE, element);

	//imshow("7 morph", src);
	vector< vector< Point> > contours2;
	findContours(src, contours2, // a vector of contours
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); // all pixels of each contour


	//GammaCorrection(src, src, 2.5);
	

								  //Start to iterate to each contour found 
	vector<vector<Point> >::iterator itc;
	vector<RotatedRect> rects;

	if (contours2.size() > 0) {
		 itc = contours2.begin();
	}
	//Remove patch that has no inside limits of aspect ratio and area.
	while (itc != contours2.end()) {
		//Create bounding rect of object
		RotatedRect mr = minAreaRect(Mat(*itc));
		mr.size = Size(mr.size.width*1.8,mr.size.height*1.8); 
		if (!verifySizes(mr)) { // || !isContourConvex(contours2)
			itc = contours2.erase(itc);
		}
		else {
			++itc;
			rects.push_back(mr);
		}
	}



	std::vector<cv::Vec4i> hierarchy;
	Mat test = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours2.size(); i++)
	{


		cv::Scalar color = cv::Scalar(200, 100, 0);
		//drawContours(test, contours2, i, color, 1, 8, hierarchy, 0, cv::Point());
	}
	//imshow("8 contours draw", test);


	float mostStraight;
	RotatedRect msr;
	if (rects.size() > 0) {
		msr = rects[0];
		(rects[0].angle <0) ? mostStraight = rects[0].angle *(-1) : mostStraight = rects[0].angle;

	}
	

	for (int j = 0; j < rects.size(); j++) {
		cv::RotatedRect boundingBox = rects[j];
		cv::Point2f corners[4];
		boundingBox.points(corners);
		cv::line(contFixed, corners[0], corners[1], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[1], corners[2], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[2], corners[3], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[3], corners[0], cv::Scalar(0, 255, 255));
		if (boundingBox.angle < 0) {
			if (mostStraight >(boundingBox.angle + 90)) {
				mostStraight = boundingBox.angle;
				msr = boundingBox;
			}
		}
		else {
			if (mostStraight > boundingBox.angle) {
				mostStraight = boundingBox.angle;
				msr = boundingBox;
			}
		}
		cout << to_string(mostStraight) << endl;
		// cvDrawRect(input,corners[0], (50, 50), (0, 255, 0), 2)
	}

	cv::Point2f corners[4];
	msr.points(corners);
	cv::line(contFixed, corners[0], corners[1], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[1], corners[2], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[2], corners[3], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[3], corners[0], cv::Scalar(100, 50, 255));


	
	if (rects.size() >= 1) {
		for (int i = 0; i < rects.size(); i++) {
			Point2f pts[4];
			msr.points(pts);

			// Does the order of the points matter? I assume they do NOT.
			// But if it does, is there an easy way to identify and order 
			// them as topLeft, topRight, bottomRight, bottomLeft?

			cv::Point2f src_vertices[3];
			//putText(in, to_string(mostStraight), pts[0],FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "0-" + to_string(pts[0].x), pts[0], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "1-" + to_string(pts[1].x), pts[1], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "2-" + to_string(pts[2].x), pts[2], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "3-" + to_string(pts[3].x), pts[3], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			if (pts[0].x > pts[2].x) {
				src_vertices[0] = pts[0];
				src_vertices[1] = pts[1];
				src_vertices[2] = pts[3];
			}
			else {
				src_vertices[0] = pts[3];
				src_vertices[1] = pts[0];
				src_vertices[2] = pts[2];
			}
			//src_vertices[3] = not_a_rect_shape[3];

			Point2f dst_vertices[3];
			dst_vertices[0] = Point(0, 0);
			dst_vertices[1] = Point(msr.boundingRect().width - 1, 0);
			dst_vertices[2] = Point(0, msr.boundingRect().height - 1);

			Mat warpAffineMatrix = getAffineTransform(src_vertices, dst_vertices);

			cv::Mat rotated;
			cv::Size size(msr.boundingRect().width, msr.boundingRect().height);
			//warpAffine(contFixed, rotated, warpAffineMatrix, size, INTER_LINEAR, BORDER_CONSTANT);
			Point2f src_center(rotated.cols / 2.0F, rotated.rows / 2.0F);
			Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.0);
			//warpAffine(rotated, rotated, rot_mat, size);
			out = src;
			//imshow("rot plate ", rotated);
		}
		// Load Face cascade (.xml file)


		/*
		CascadeClassifier face_cascade;
		face_cascade.load("C:/Users/Athinodoros/Desktop/Internship/classifier/cl 2/88 2kn 1kp/cascade.xml");

		// Detect plates
		std::vector<Rect> platesdetected;
		face_cascade.detectMultiScale(contFixed, platesdetected, 1.1, 2, 0 | CASCADE_SCALE_IMAGE);
		Mat newRound;
		// Draw circles on the detected faces
		for (int i = 0; i < platesdetected.size(); i++)
		{

		Point center(platesdetected[i].x + platesdetected[i].width*0.5, platesdetected[i].y + platesdetected[i].height*0.5);
		ellipse(contFixed, center, Size(platesdetected[i].width*0.5, platesdetected[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		}
		*/
		//imshow("sqares ", contFixed);
	}


	/*

	if (contours2.size() >0) {


	}
	*/
}


/*
void findPossiblePlates(Mat& in, Mat& out) {
Mat input = in;
//Mat gray = imread("testttt.jpg", CV_LOAD_IMAGE_GRAYSCALE);

Mat src = input;
Mat contFixed;

imshow("1 original", src);
cvtColor(src, src, CV_BGR2GRAY);

imshow("2 gray scale", src);

//imshow("bw", src);
equalizeHist(src, src);

equalizeHist(src, contFixed);

imshow("3 histogram equalized", src);
blur(src, src, Size(20, 1));

imshow("4 blur ", src);

Sobel(src, src, CV_8U, 1, 0, 3, 1, 0);

imshow("5 sobel", src);

threshold(src, src, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

imshow("6 thresshold ", src);
Mat element = getStructuringElement(MORPH_RECT, Size(30, 10));
morphologyEx(src, src, CV_MOP_CLOSE, element);

imshow("7 morph", src);
vector< vector< Point> > contours2;
findContours(src, contours2, // a vector of contours
CV_RETR_EXTERNAL, // retrieve the external contours
CV_CHAIN_APPROX_NONE); // all pixels of each contour


//GammaCorrection(src, src, 2.5);


//Start to iterate to each contour found
vector<vector<Point> >::iterator itc;
vector<RotatedRect> rects;

if (contours2.size() > 0) {
itc = contours2.begin();
}
//Remove patch that has no inside limits of aspect ratio and area.
while (itc != contours2.end()) {
//Create bounding rect of object
RotatedRect mr = minAreaRect(Mat(*itc));
mr.size = Size(mr.size.width*1.8, mr.size.height*1.8);
if (!verifySizes(mr)) { // || !isContourConvex(contours2)
itc = contours2.erase(itc);
}
else {
++itc;
rects.push_back(mr);
}
}



std::vector<cv::Vec4i> hierarchy;
Mat test = Mat::zeros(src.size(), CV_8UC3);
for (int i = 0; i < contours2.size(); i++)
{


cv::Scalar color = cv::Scalar(200, 100, 0);
drawContours(test, contours2, i, color, 1, 8, hierarchy, 0, cv::Point());
}
//imshow("8 contours draw", test);


float mostStraight;
RotatedRect msr;
if (rects.size() > 0) {
msr = rects[0];
(rects[0].angle < 0) ? mostStraight = rects[0].angle *(-1) : mostStraight = rects[0].angle;

}


for (int j = 0; j < rects.size(); j++) {
cv::RotatedRect boundingBox = rects[j];
cv::Point2f corners[4];
boundingBox.points(corners);
cv::line(contFixed, corners[0], corners[1], cv::Scalar(0, 255, 255));
cv::line(contFixed, corners[1], corners[2], cv::Scalar(0, 255, 255));
cv::line(contFixed, corners[2], corners[3], cv::Scalar(0, 255, 255));
cv::line(contFixed, corners[3], corners[0], cv::Scalar(0, 255, 255));
if (boundingBox.angle < 0) {
if (mostStraight > (boundingBox.angle + 90)) {
mostStraight = boundingBox.angle;
msr = boundingBox;
}
}
else {
if (mostStraight > boundingBox.angle) {
mostStraight = boundingBox.angle;
msr = boundingBox;
}
}
cout << to_string(mostStraight) << endl;
// cvDrawRect(input,corners[0], (50, 50), (0, 255, 0), 2)
}

cv::Point2f corners[4];
msr.points(corners);
cv::line(contFixed, corners[0], corners[1], cv::Scalar(100, 50, 255));
cv::line(contFixed, corners[1], corners[2], cv::Scalar(100, 50, 255));
cv::line(contFixed, corners[2], corners[3], cv::Scalar(100, 50, 255));
cv::line(contFixed, corners[3], corners[0], cv::Scalar(100, 50, 255));



if (rects.size() >= 1) {
for (int i = 0; i < rects.size(); i++) {
Point2f pts[4];
msr.points(pts);

// Does the order of the points matter? I assume they do NOT.
// But if it does, is there an easy way to identify and order
// them as topLeft, topRight, bottomRight, bottomLeft?

cv::Point2f src_vertices[3];
//putText(in, to_string(mostStraight), pts[0],FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
putText(src, "0-" + to_string(pts[0].x), pts[0], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
putText(src, "1-" + to_string(pts[1].x), pts[1], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
putText(src, "2-" + to_string(pts[2].x), pts[2], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
putText(src, "3-" + to_string(pts[3].x), pts[3], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
if (pts[0].x > pts[2].x) {
src_vertices[0] = pts[0];
src_vertices[1] = pts[1];
src_vertices[2] = pts[3];
}
else {
src_vertices[0] = pts[3];
src_vertices[1] = pts[0];
src_vertices[2] = pts[2];
}
//src_vertices[3] = not_a_rect_shape[3];

Point2f dst_vertices[3];
dst_vertices[0] = Point(0, 0);
dst_vertices[1] = Point(msr.boundingRect().width - 1, 0);
dst_vertices[2] = Point(0, msr.boundingRect().height - 1);

Mat warpAffineMatrix = getAffineTransform(src_vertices, dst_vertices);

cv::Mat rotated;
cv::Size size(msr.boundingRect().width, msr.boundingRect().height);
warpAffine(contFixed, rotated, warpAffineMatrix, size, INTER_LINEAR, BORDER_CONSTANT);
Point2f src_center(rotated.cols / 2.0F, rotated.rows / 2.0F);
Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.0);
warpAffine(rotated, rotated, rot_mat, size);
out = src;
imshow("rot plate ", rotated);
}
// Load Face cascade (.xml file)



CascadeClassifier face_cascade;
face_cascade.load("C:/Users/Athinodoros/Desktop/Internship/classifier/cl 2/88 2kn 1kp/cascade.xml");

// Detect plates
std::vector<Rect> platesdetected;
face_cascade.detectMultiScale(contFixed, platesdetected, 1.1, 2, 0 | CASCADE_SCALE_IMAGE);
Mat newRound;
// Draw circles on the detected faces
for (int i = 0; i < platesdetected.size(); i++)
{

Point center(platesdetected[i].x + platesdetected[i].width*0.5, platesdetected[i].y + platesdetected[i].height*0.5);
ellipse(contFixed, center, Size(platesdetected[i].width*0.5, platesdetected[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
}

imshow("sqares ", contFixed);
}


}
*/
uchar* MatToBytes(cv::Mat image)
{
	/*
	//class data members of ints
	image_rows = image.rows;
	image_cols = image.cols;
	image_type = image.type();
	*/
	
	int image_size = image.total() * image.elemSize();
	uchar * image_uchar = new uchar[image_size];
	std::memcpy(image_uchar, image.data, image_size * sizeof(uchar));
	return image_uchar;
}
MYCVAPI Sframe findPossiblePlatesSF(Mat in) {
	Mat src = in.clone();
	// Load Plate cascade (.xml file)
	CascadeClassifier face_cascade;
	face_cascade.load("cascade.xml");
	// Detect plates
	std::vector<Rect> platesdetected;
	face_cascade.detectMultiScale(src, platesdetected, 1.1, 3, 0 | CASCADE_SCALE_IMAGE); //edw src
	Mat newRound;
	// Draw circles on the detected plates
	Rect biggest;
	biggest.width = 0;
	if (platesdetected.size()>0) {
		biggest = platesdetected[0];
	}
	for (int i = 0; i < platesdetected.size(); i++)
	{
		try {
			Point center(platesdetected[i].x + platesdetected[i].width*0.5, platesdetected[i].y + platesdetected[i].height*0.5);
			rectangle(src, platesdetected[i], Scalar(255, 80, 0), 1, 8, 0);
		}
		catch (Exception e) {
			std::cout << "dsaf" << e.msg << endl;
		}
		if (biggest.size().width < platesdetected[i].size().width) {
			biggest = platesdetected[i];
		}
	}


	if (biggest.width>0) {
		sf.x1 = biggest.x;
		sf.y1 = biggest.y;
		sf.x2 = biggest.x + biggest.width;
		sf.y2 = biggest.y + biggest.height;

		return sf;
	}
	else {
		sf.x1 = 11;
		sf.y1 = 21;
		sf.x2 = 12;
		sf.y2 = 22;
		return sf;
	}
}

MYCVAPI void findPossiblePlates(Mat & in)
{


	input = in; // Mat(Size(in.width, in.height), CV_8UC1, in.imgData, CV_AUTO_STEP);
	//Mat gray = imread("testttt.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	src = input;
	

	//imshow("1 original", src);
	cvtColor(src, src, CV_BGR2GRAY);

	//imshow("2 gray scale", src);

	//imshow("bw", src);
	equalizeHist(src, src);

	equalizeHist(src, contFixed);

	//imshow("3 histogram equalized", src);
	blur(src, src, Size(20, 1));

	//imshow("4 blur ", src);

	Sobel(src, src, CV_8U, 1, 0, 3, 1, 0);

	//imshow("5 sobel", src);

	threshold(src, src, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	//imshow("6 thresshold ", src);
	Mat element = getStructuringElement(MORPH_RECT, Size(30, 10));
	morphologyEx(src, src, CV_MOP_CLOSE, element);

	//imshow("7 morph", src);
	vector< vector< Point> > contours2;
	findContours(src, contours2, // a vector of contours
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); 
	vector<vector<Point> >::iterator itc;
	vector<RotatedRect> rects;

	if (contours2.size() > 0) {
		itc = contours2.begin();
	}
	//Remove patch that has no inside limits of aspect ratio and area.
	while (itc != contours2.end()) {
		//Create bounding rect of object
		RotatedRect mr = minAreaRect(Mat(*itc));
		mr.size = Size(mr.size.width*1.8, mr.size.height*1.8);
		if (!verifySizes(mr)) { // || !isContourConvex(contours2)
			itc = contours2.erase(itc);
		}
		else {
			++itc;
			rects.push_back(mr);
		}
	}
	std::vector<cv::Vec4i> hierarchy;
	Mat test = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours2.size(); i++)
	{


		cv::Scalar color = cv::Scalar(200, 100, 0);
		//drawContours(test, contours2, i, color, 1, 8, hierarchy, 0, cv::Point());
	}
	//imshow("8 contours draw", test);


	float mostStraight;
	RotatedRect msr;
	if (rects.size() > 0) {
		msr = rects[0];
		(rects[0].angle <0) ? mostStraight = rects[0].angle *(-1) : mostStraight = rects[0].angle;

	}


	for (int j = 0; j < rects.size(); j++) {
		cv::RotatedRect boundingBox = rects[j];
		cv::Point2f corners[4];
		boundingBox.points(corners);
		cv::line(contFixed, corners[0], corners[1], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[1], corners[2], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[2], corners[3], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[3], corners[0], cv::Scalar(0, 255, 255));
		if (boundingBox.angle < 0) {
			if (mostStraight >(boundingBox.angle + 90)) {
				mostStraight = boundingBox.angle;
				msr = boundingBox;
			}
		}
		else {
			if (mostStraight > boundingBox.angle) {
				mostStraight = boundingBox.angle;
				msr = boundingBox;
			}
		}
		cout << to_string(mostStraight) << endl;
		// cvDrawRect(input,corners[0], (50, 50), (0, 255, 0), 2)
	}

	cv::Point2f corners[4];
	msr.points(corners);
	cv::line(contFixed, corners[0], corners[1], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[1], corners[2], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[2], corners[3], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[3], corners[0], cv::Scalar(100, 50, 255));



	if (rects.size() >= 1) {
		for (int i = 0; i < rects.size(); i++) {
			Point2f pts[4];
			msr.points(pts);
			cv::Point2f src_vertices[3];
			//putText(in, to_string(mostStraight), pts[0],FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "0-" + to_string(pts[0].x), pts[0], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "1-" + to_string(pts[1].x), pts[1], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "2-" + to_string(pts[2].x), pts[2], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "3-" + to_string(pts[3].x), pts[3], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			if (pts[0].x > pts[2].x) {
				src_vertices[0] = pts[0];
				src_vertices[1] = pts[1];
				src_vertices[2] = pts[3];
			}
			else {
				src_vertices[0] = pts[3];
				src_vertices[1] = pts[0];
				src_vertices[2] = pts[2];
			}
			//src_vertices[3] = not_a_rect_shape[3];

			Point2f dst_vertices[3];
			dst_vertices[0] = Point(0, 0);
			dst_vertices[1] = Point(msr.boundingRect().width - 1, 0);
			dst_vertices[2] = Point(0, msr.boundingRect().height - 1);

			Mat warpAffineMatrix = getAffineTransform(src_vertices, dst_vertices);

			cv::Mat rotated;
			cv::Size size(msr.boundingRect().width, msr.boundingRect().height);
			//warpAffine(contFixed, rotated, warpAffineMatrix, size, INTER_LINEAR, BORDER_CONSTANT);
			Point2f src_center(rotated.cols / 2.0F, rotated.rows / 2.0F);
			Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.0);
			//warpAffine(rotated, rotated, rot_mat, size);
			//out = src;
			//imshow("rot plate ", rotated);
		}
		imshow("sqares ", contFixed);

		/*
		Sframe retObj;
		retObj.imgData = contFixed.data;
		retObj.hasPlateOn = true;
		retObj.width = contFixed.rows;
		retObj.height = contFixed.cols;

		return retObj ;
		*/

	}

}

MYCVAPI uchar * findPossiblePlates2(Mat& in) {
	Mat input = in;
	//Mat gray = imread("testttt.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	Mat src = input;
	Mat contFixed;

	//imshow("1 original", src);
	cvtColor(src, src, CV_BGR2GRAY);

	//imshow("2 gray scale", src);

	//imshow("bw", src);
	equalizeHist(src, src);

	equalizeHist(src, contFixed);

	//imshow("3 histogram equalized", src);
	blur(src, src, Size(20, 1));

	//imshow("4 blur ", src);

	Sobel(src, src, CV_8U, 1, 0, 3, 1, 0);

	//imshow("5 sobel", src);

	threshold(src, src, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);

	//imshow("6 thresshold ", src);
	Mat element = getStructuringElement(MORPH_RECT, Size(30, 10));
	morphologyEx(src, src, CV_MOP_CLOSE, element);

	//imshow("7 morph", src);
	vector< vector< Point> > contours2;
	findContours(src, contours2, // a vector of contours
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); // all pixels of each contour


							   //GammaCorrection(src, src, 2.5);


							   //Start to iterate to each contour found 
	vector<vector<Point> >::iterator itc;
	vector<RotatedRect> rects;

	if (contours2.size() > 0) {
		itc = contours2.begin();
	}
	//Remove patch that has no inside limits of aspect ratio and area.
	while (itc != contours2.end()) {
		//Create bounding rect of object
		RotatedRect mr = minAreaRect(Mat(*itc));
		mr.size = Size(mr.size.width*1.8, mr.size.height*1.8);
		if (!verifySizes(mr)) { // || !isContourConvex(contours2)
			itc = contours2.erase(itc);
		}
		else {
			++itc;
			rects.push_back(mr);
		}
	}



	std::vector<cv::Vec4i> hierarchy;
	Mat test = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours2.size(); i++)
	{


		cv::Scalar color = cv::Scalar(200, 100, 0);
		//drawContours(test, contours2, i, color, 1, 8, hierarchy, 0, cv::Point());
	}
	//imshow("8 contours draw", test);


	float mostStraight;
	RotatedRect msr;
	if (rects.size() > 0) {
		msr = rects[0];
		(rects[0].angle <0) ? mostStraight = rects[0].angle *(-1) : mostStraight = rects[0].angle;

	}


	for (int j = 0; j < rects.size(); j++) {
		cv::RotatedRect boundingBox = rects[j];
		cv::Point2f corners[4];
		boundingBox.points(corners);
		cv::line(contFixed, corners[0], corners[1], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[1], corners[2], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[2], corners[3], cv::Scalar(0, 255, 255));
		cv::line(contFixed, corners[3], corners[0], cv::Scalar(0, 255, 255));
		if (boundingBox.angle < 0) {
			if (mostStraight >(boundingBox.angle + 90)) {
				mostStraight = boundingBox.angle;
				msr = boundingBox;
			}
		}
		else {
			if (mostStraight > boundingBox.angle) {
				mostStraight = boundingBox.angle;
				msr = boundingBox;
			}
		}
		cout << to_string(mostStraight) << endl;
		// cvDrawRect(input,corners[0], (50, 50), (0, 255, 0), 2)
	}

	cv::Point2f corners[4];
	msr.points(corners);
	cv::line(contFixed, corners[0], corners[1], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[1], corners[2], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[2], corners[3], cv::Scalar(100, 50, 255));
	cv::line(contFixed, corners[3], corners[0], cv::Scalar(100, 50, 255));



	if (rects.size() >= 1) {
		for (int i = 0; i < rects.size(); i++) {
			Point2f pts[4];
			msr.points(pts);

			// Does the order of the points matter? I assume they do NOT.
			// But if it does, is there an easy way to identify and order 
			// them as topLeft, topRight, bottomRight, bottomLeft?

			cv::Point2f src_vertices[3];
			//putText(in, to_string(mostStraight), pts[0],FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "0-" + to_string(pts[0].x), pts[0], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "1-" + to_string(pts[1].x), pts[1], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "2-" + to_string(pts[2].x), pts[2], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			putText(src, "3-" + to_string(pts[3].x), pts[3], FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
			if (pts[0].x > pts[2].x) {
				src_vertices[0] = pts[0];
				src_vertices[1] = pts[1];
				src_vertices[2] = pts[3];
			}
			else {
				src_vertices[0] = pts[3];
				src_vertices[1] = pts[0];
				src_vertices[2] = pts[2];
			}
			//src_vertices[3] = not_a_rect_shape[3];

			Point2f dst_vertices[3];
			dst_vertices[0] = Point(0, 0);
			dst_vertices[1] = Point(msr.boundingRect().width - 1, 0);
			dst_vertices[2] = Point(0, msr.boundingRect().height - 1);

			Mat warpAffineMatrix = getAffineTransform(src_vertices, dst_vertices);

			cv::Mat rotated;
			cv::Size size(msr.boundingRect().width, msr.boundingRect().height);
			//warpAffine(contFixed, rotated, warpAffineMatrix, size, INTER_LINEAR, BORDER_CONSTANT);
			Point2f src_center(rotated.cols / 2.0F, rotated.rows / 2.0F);
			Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.0);
			//warpAffine(rotated, rotated, rot_mat, size);
			//out = src;
			//imshow("rot plate ", rotated);
		}
		// Load Face cascade (.xml file)


		/*
		CascadeClassifier face_cascade;
		face_cascade.load("C:/Users/Athinodoros/Desktop/Internship/classifier/cl 2/88 2kn 1kp/cascade.xml");

		// Detect plates
		std::vector<Rect> platesdetected;
		face_cascade.detectMultiScale(contFixed, platesdetected, 1.1, 2, 0 | CASCADE_SCALE_IMAGE);
		Mat newRound;
		// Draw circles on the detected faces
		for (int i = 0; i < platesdetected.size(); i++)
		{

		Point center(platesdetected[i].x + platesdetected[i].width*0.5, platesdetected[i].y + platesdetected[i].height*0.5);
		ellipse(contFixed, center, Size(platesdetected[i].width*0.5, platesdetected[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
		}
		*/
		//imshow("sqares ", contFixed);
		return contFixed.data;
			
	}


	/*

	if (contours2.size() >0) {


	}
	*/
}


void findPossiblePlatesClassifier(Mat& in, Mat& out) {
	Mat input = in;
	//Mat gray = imread("testttt.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	Mat src = input;
	Mat contFixed;

	//imshow("1 original", src);
	cvtColor(src, contFixed, CV_BGR2GRAY);


	// Load Plate cascade (.xml file)
	CascadeClassifier face_cascade;
	face_cascade.load("cascade.xml");

	// Detect plates
	std::vector<Rect> platesdetected;
	face_cascade.detectMultiScale(src, platesdetected, 1.1, 3, 0 | CASCADE_SCALE_IMAGE); //edw src
	Mat newRound;
	// Draw circles on the detected plates
	Rect biggest;
	if (platesdetected.size()>0) {
		biggest = platesdetected[0];
	}
	for (int i = 0; i < platesdetected.size(); i++)
	{
		try {
			//Size deltaSize(platesdetected[i].width , platesdetected[i].height ); // 0.1f = 10/100
			//Size deltaSize(platesdetected[i].width * 0.2f, platesdetected[i].height * 0.2f); // 0.1f = 10/100
			//Point offset(deltaSize.width / 2, deltaSize.height / 2);
			//platesdetected[i] += deltaSize;
			//platesdetected[i] -= offset;

			Point center(platesdetected[i].x + platesdetected[i].width*0.5, platesdetected[i].y + platesdetected[i].height*0.5);
			rectangle(src, platesdetected[i], Scalar(255, 80, 0), 1, 8, 0);
			//ellipse(src, center, Size(platesdetected[i].width*0.5, platesdetected[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0); // edw src
			//ellipse(src, center, Size(platesdetected[i].width, platesdetected[i].height), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0); // edw src
		}
		catch (Exception e) {
			std::cout << "dsaf" << e.msg << endl;
		}
		if (biggest.size().width < platesdetected[i].size().width) {
			biggest = platesdetected[i];
		}
	}

	cv::Mat rotated;
	RotatedRect msr;

	if (platesdetected.size() >= 1) {
		rotated = contFixed(biggest);
		//imshow("rects", rotated);
	}
	cv::Mat rotOut;
	if (!rotated.empty()) {
		findPossiblePlates(in, rotOut);
	}
}


void read_directory(const std::string& name, vector<std::string>& v)
{
	std::string pattern(name);
	pattern.append("\\*");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			v.push_back(data.cFileName);
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}

void GammaCorrection(Mat& src, Mat& dst, float fGamma)

{

	unsigned char lut[256];

	for (int i = 0; i < 256; i++)

	{

		lut[i] = saturate_cast<uchar>(pow((float)(i / 255.0), fGamma) * 255.0f);

	}

	dst = src.clone();

	const int channels = dst.channels();

	switch (channels)

	{

	case 1:

	{

		MatIterator_<uchar> it, end;

		for (it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++)

			*it = lut[(*it)];

		break;

	}

	case 3:

	{

		MatIterator_<Vec3b> it, end;

		for (it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++)

		{

			(*it)[0] = lut[((*it)[0])];

			(*it)[1] = lut[((*it)[1])];

			(*it)[2] = lut[((*it)[2])];

		}

		break;

	}

	}

}



bool verifySizes(RotatedRect candidate) {
	float error = 0.4f;
	//Spain car plate size: 52x11 aspect 4,7272
	const float aspect = 4.7272f;
	//Set a min and max area. All other patches are discarded
	int min = (int)(15 * aspect * 15); // minimum area
	int max = (int)(125 * aspect * 125); // maximum area
										 //Get only patches that match to a respect ratio.
	float rmin = aspect - aspect * error;
	float rmax = aspect + aspect * error;
	int area = (int)(candidate.size.height * candidate.size.width);
	float r = (float)candidate.size.width / (float)candidate.size.height;
	if (r < 1)
		r = 1 / r;
	if ((area < min || area > max) || (r < rmin || r > rmax)) {
		return false;
	}
	else {
		return true;
	}
}
