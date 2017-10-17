#include <opencv2/opencv.hpp>
#include <iostream>
#include<cmath>
using namespace std;
using namespace cv;
Mat src, dst, out;
double maxinterval = 20;
struct contourblock {
contourblock(Point a,Point b,Point c):maxpoint(a),minpoint(b),averagepoint(c){}
	Point maxpoint;
	Point minpoint;
	Point averagepoint;
};//������һ�������ṹ��
int getdistance(Point a, Point b) {
	return sqrt(abs(pow((a.x - b.x), 2) - pow((a.y - b.y), 2)));
}//��ȡ�����ص����
int findfirst(vector<contourblock> allblock) {
	int tmp = 640-allblock[0].maxpoint.y ;
	int index = 0;
	for (int i = 1; i<allblock.size() ; i++) {
		if ((640 - allblock[i].maxpoint.y) < tmp) {
			index = i;
			tmp = 640 - allblock[i].maxpoint.y;
		}
	}
	return index;
}//Ѱ�ҵ�һ������
int  findnext(vector<contourblock> allblock, int current) {
	int min = 9999999;
	int index = current;
	for (int i = 0; i < allblock.size(); i++) {
		int distance = getdistance(allblock[i].maxpoint,allblock[current].minpoint);
		if (i != current&&distance < min) {
			index = i;
			min = distance;
		}
	}
	cout << min<< endl;
	if (min>=maxinterval)
		return -1;
	else
		return index;
}//Ѱ����һ������
Point getaverage(vector<Point> item) {
	int sumx=0;
	int sumy=0;
	for (int i = 0; i<item.size(); i++) {
		sumx += item[i].x;
		sumy +=item[i].y;
		//cout << item.size() << endl;
	}
	return Point(sumx/item.size(), sumy/item.size());
}//��ȡ����ƽ��ֵ
Point getmin(vector<Point> item) {
	Point minpoint = item[0];
	for (int i = 1; i < item.size(); i++) {
		if ((item[i].x + item[i].y) < (minpoint.x + minpoint.y))
			minpoint = item[i];
	}
	return  minpoint;
}//��ȡ������Сֵ
Point getmax(vector<Point> item) {
	Point maxpoint = item[0];
	for (int i = 1; i < item.size(); i++) {
		if ((item[i].x + item[i].y) > (maxpoint.x + maxpoint.y))
			maxpoint = item[i];
	}
	return maxpoint;
}//��ȡ�������ֵ

void Threshold_Demo(int, void*) {

}

int main(int argc, char** argv) {
	//src = imread("D:/test.jpg");
	IplImage *cvSrc= cvLoadImage("D:/test4.jpg");
	src = cvarrToMat(cvSrc);
	IplImage *yuv = cvCreateImage(cvSize(cvSrc->width, cvSrc->height), IPL_DEPTH_8U, 3);
	IplImage *cvDst= cvCreateImage(cvSize(cvSrc->width, cvSrc->height), IPL_DEPTH_8U, 1);;
	cvCvtColor(cvSrc, yuv, CV_RGB2YUV);
	IplImage *cvGray = cvCreateImage(cvSize(cvSrc->width, cvSrc->height), IPL_DEPTH_8U, 1);
	cvSplit(yuv, cvGray, nullptr, nullptr, nullptr);
	cvThreshold(cvGray, cvDst, 220, 255, CV_THRESH_BINARY);//��ֵ��
	dst= cvarrToMat(cvDst);
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	imshow("input", src);      
	namedWindow("output1", CV_WINDOW_AUTOSIZE);
	namedWindow("output2", CV_WINDOW_AUTOSIZE);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5,1), Point(-1, -1));
	morphologyEx(dst,dst, CV_MOP_OPEN, kernel);//������
	vector<vector<Point>> contours;
	vector<Vec4i> hierachy;
	medianBlur(dst, dst,7);//ȥ��
	findContours(dst, contours, hierachy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));//Ѱ������
	out = Mat(src);
	for (size_t i = 0; i < contours.size(); i++) {		
		drawContours(dst, contours, i, Scalar(255,0,0), 2, 8, hierachy, 0, Point(0, 0));
	}
	vector<contourblock> allblock;
	for (int i = 0; i < contours.size(); i++)
		if(contours[i].size()>10)
			allblock.push_back(contourblock(getmax(contours[i]), getmin(contours[i]), getaverage(contours[i])));//��������ṹ��
	vector<Point> myroute;
	int index = findfirst(allblock);//Ѱ�ҵ�һ������
	myroute.push_back(Point(allblock[index].averagepoint.x, 640));
	myroute.push_back(allblock[index].averagepoint);
	while(findnext(allblock, index)!=-1) {
		index = findnext(allblock, index);
		myroute.push_back(allblock[index].averagepoint);
	}//Ѱ����һ������
	if (myroute.size() == 2) {
		myroute.push_back(allblock[index].minpoint);
		vector<Point>::iterator itr = myroute.begin();
		
		myroute.erase(itr);
		myroute.insert(itr, Point(allblock[index].maxpoint.x, 640));
	}//���ֻ�ҵ�һ�����������Ż�����
	for (int i = 0; i < myroute.size(); i++) {
		cout << myroute[i]<<endl;
		//circle(out, myroute[i], 3, Scalar(255, 0, 0));
		if(i<myroute.size()-1)
			line(out, myroute[i], myroute[i+1], Scalar(0, 0, 255), 2, 8);
	}
	dst = Mat(src.rows, src.cols, CV_8UC1,Scalar(255,255,255));
	for (int i = 0; i < myroute.size(); i++) {
		cout << myroute[i] << endl;
		//circle(out, myroute[i], 3, Scalar(255, 0, 0));
		if (i<myroute.size() - 1)
			line(dst, myroute[i], myroute[i + 1], Scalar(127, 127, 127), 2, 8);
	}
	imshow("output1", dst);
	imshow("output2", out);
	waitKey(0);
	
}
