/*
 * textorient.h
 *
 *  Created on: May 4, 2015
 *      Author: litton
 */

#ifndef TEXTORIENT_H_
#define TEXTORIENT_H_

#include "textarea.h"

using namespace std;
using namespace cv;

enum TextDirection {NOCHANGE, NINETY, OPPOSITE, R_NINETY};

double textorient = -10;
int INTERVAL = 45;

int dIntegral(vector<int> nums){
	int sum = 0;
	for(int i=0;i<nums.size();i++){
		if(i>0&&fabs(nums[i]-nums[i-1])>10)
			sum+=fabs(nums[i]-nums[i-1]);
	}
	return sum;
}

bool eliminatePointDict(map<int,int>& dict){
	int elimCount = 1;
	bool edgeFail = false;

	while (elimCount > 0) {
		elimCount = 0;
		vector<int> ys, xs;
		for (map<int, int>::iterator it = dict.begin(); it != dict.end();
				it++) {
			ys.push_back(it->first);
			xs.push_back(it->second);
			//cout<<"POINT: "<<it->first<<" "<<it->second<<endl;
		}

		if (xs.size() < 2) {
			edgeFail = true;
			break;
		}

		if (xs[0] - xs[1] > 20) {
			map<int, int>::iterator it = dict.find(ys[0]);
			dict.erase(it);
			elimCount++;
		}

		for (int i = 0; i < ys.size() - 1; i++) {
			if (xs[i + 1] - xs[i] > 20) {
				map<int, int>::iterator it = dict.find(ys[i + 1]);
				if (it != dict.end()) {
					dict.erase(it);
					//cout<<"erase: "<<i+1<<endl;
					elimCount++;
				}
			}

			if (xs[i] - xs[i + 1] > 100) {
				map<int, int>::iterator it = dict.find(ys[i]);
				if (it != dict.end()) {
					dict.erase(it);
					//cout<<"erase: "<<i+1<<endl;
					elimCount++;
				}
			}
		}
	}
	return edgeFail;
}

//0: no need to turn
//1: turn left
//2: turn right
//3: turn 180
int getTextOrient(Mat src){
	double result = 0;

	Mat image, grad_x, abs_grad_x, grad_y, abs_grad_y, grad;
	image = src.clone();
	Mat grey;
	cv::cvtColor(image, grey, CV_BGR2GRAY);

	int ddepth = 3;

	cv::Sobel(grey, grad_x, ddepth, 1, 0);
	cv::convertScaleAbs(grad_x, abs_grad_x);

	cv::Sobel(grey, grad_y, ddepth, 0, 1);
	cv::convertScaleAbs(grad_y, abs_grad_y);

	cv::addWeighted(abs_grad_x, 1, abs_grad_y, 1, 0, grad);
	cv::threshold(grad, grad, 40.0, 255, CV_THRESH_TOZERO);

	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(grad, lines, 1, CV_PI / 180, 100, 70, 20);

	//imshow("grad", grad);
	//waitKey();

	int vsize = 0;
	vector<double> tpK(1000);//slope cluster
	vector<int> tpN(1000);//element number of each cluster
	vector<vector<int> > tpV(1000);//store the line indices of each cluster

	int vertN = 0;//vertical lines (without slope)
	vector<int> verts;

	for (int i = 0; i < lines.size(); i++) {
		cv::Vec4i v = lines[i];
		if (v[0] >= v[2] - 2 && v[0] <= v[2] + 2) {
			vertN++;
			verts.push_back(i);
		} else {
			double k = (0.0 + v[3] - v[1]) / (0.0 + v[2] - v[0]);
			bool found = false;
			for (int j = 0; j < vsize; j++) {
				if (fabs(k - tpK[j]) < 0.18) {
					found = true;
					tpK[j] = (tpK[j] * tpN[j] + k) / (tpN[j] + 1);
					tpN[j] = tpN[j] + 1;
					tpV[j].push_back(i);
				}
			}
			if (!found) {
				tpK[vsize] = k;
				tpN[vsize] = 1;
				vector<int> v(1000);
				v.push_back(i);
				tpV.push_back(v);
				vsize++;
			}
		}

		//line( grad, cv::Point(v[0], v[1]), cv::Point(v[2], v[3]), CV_RGB(255,255,255));
	}
	//cout<<lines.size()<<endl;
	//imshow("grad", grad);
	//waitKey();

	int maxTp = -1;
	int maxCt = 0;

	int secTp = -1;
	int secCt = 0;

	//find max cluster
	for (int i = 0; i < vsize; i++) {
		if (tpN[i] > maxCt) {
			if (maxCt > secCt) {
				secCt = maxCt;
				secTp = maxTp;
			}
			maxCt = tpN[i];
			maxTp = i;
		} else if (tpN[i] > secCt) {
			//second largest cluster
			secCt = tpN[i];
			secTp = i;
		}
	}

	if (vertN > maxCt) {
		if (maxCt > secCt) {
			secCt = maxCt;
			secTp = maxTp;
		}
		maxCt = vertN;
		maxTp = -1;//Vertical lines is the biggest cluster
	} else {
		if (vertN > secCt) {
			secCt = vertN;
			secTp = -1;
		}
	}

	//Test Paper-photo (main part is words) or graph photo (main part is not words)
	cout << "is paper? " << maxCt << " " << secCt << " " << (maxCt >= 2 * secCt)
			<< endl;
	if (maxTp != -1 && fabs(tpK[maxTp]) > 0.5
			&& (secTp == -1 || fabs(tpK[secTp]) < 0.2) && secCt > 200) {
		maxTp = secTp;
		maxCt = secCt;
		secTp = 0;
		secCt = 0;
	}

//	if (/*maxCt > 600 || maxCt < 90 ||*/ maxCt < 1.5 * secCt
//			|| !(maxTp == -1 || fabs(tpK[maxTp]) < 0.5)) {
//		return 0.0;//Return: NOT paper-photo
//	}

	//vertical direction is the main word direction
	if (maxTp == -1) {
		cout << "vertical max tp " << maxTp << " " << secTp << " " << vertN << " "
				<< verts.size() << endl;

		Mat src3 = Mat::zeros(src.rows, src.cols, CV_8UC1);
		vector<int> block = verts;//tpV[maxTp];

		for (int i = 0; i < block.size(); i++) {

			Vec4i myline = lines[block[i]];

			line(src3, cv::Point(myline[0], myline[1]),
					cv::Point(myline[2], myline[3]), CV_RGB(255, 255, 255));

		}


		Mat eroElm = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
		Mat src4;
		erode(src3, src4, eroElm);

		Mat eroElm2 = getStructuringElement(MORPH_RECT, Size(INTERVAL, INTERVAL), Point(1, 1));
		dilate(src4, src4, eroElm2);
//		imshow("image2", src3);
//		waitKey();
		//Canny( src4, src4, 100, 200, 3 );
		Mat nonZeroCoordinates;
		findNonZero(src4, nonZeroCoordinates);
		map<int, int> dict;

		int up = 9999, bt = -1, left = 9999, right = -1;
		int upx, btx, lefty, righty;

		for (int i = 0; i < nonZeroCoordinates.total(); i++) {
			//cout << "Zero#" << i << ": " << nonZeroCoordinates.at<Point>(i).x << ", " << nonZeroCoordinates.at<Point>(i).y << endl;
			Point p = nonZeroCoordinates.at<Point>(i);
			int x = p.x;
			int y = p.y;
			if(x>0.1*src4.cols&&x<0.9*src4.cols){
				if (dict.find(x / INTERVAL) != dict.end()) {
					if (y < dict[x / INTERVAL])
						dict[x / INTERVAL] = y;
				} else {
					dict[x / INTERVAL] = y;
				}
			}
		}


		vector<int> vec1;
//		cout<<"vec1:"<<endl;
		for(map<int,int>::iterator i=dict.begin();i!=dict.end();i++){
//			cout<<i->second<<endl;
			vec1.push_back(i->second);//#include "workflow/processor.h"
			//
			//using namespace cv;
			//using namespace std;
			//
			//int main2(int argc, char** argv) {
			//	Processor::process_main(argc, argv);
			//}

		}
		int sumDiff1 = dIntegral(vec1)/vec1.size();

//		Mat src5 = Mat::zeros(src4.rows,src4.cols,CV_8UC1);
//		for(map<int,int>::iterator it=dict.begin();it!=dict.end();it++){
//			circle(src5, Point(it->second,20*it->first), 3, CV_RGB(255,255,255), 1);
//		}
//		imshow("image2", src5);
//		waitKey();

		float kt = 99999.9;
		if (!eliminatePointDict(dict))
			kt = slope(dict, 1);

		dict.clear();

		for (int i = 0; i < nonZeroCoordinates.total(); i++) {
			//cout << "Zero#" << i << ": " << nonZeroCoordinates.at<Point>(i).x << ", " << nonZeroCoordinates.at<Point>(i).y << endl;
			Point p = nonZeroCoordinates.at<Point>(i);
			int x = p.x;
			int y = p.y;
			if(x>0.1*src4.cols&&x<0.9*src4.cols){
				if (dict.find(x / INTERVAL) != dict.end()) {
					if (y > dict[x / INTERVAL])
						dict[x / INTERVAL] = y;
				} else {
					dict[x / INTERVAL] = y;
				}
			}
		}

		vector<int> vec2;
//		cout<<"vec1:"<<endl;
		for(map<int,int>::iterator i=dict.begin();i!=dict.end();i++){
			vec2.push_back(i->second);
//			cout<<i->second<<endl;
		}
		int sumDiff2 = dIntegral(vec2)/vec2.size();

//		Mat src6 = Mat::zeros(src4.rows,src4.cols,CV_8UC1);
//		for(map<int,int>::iterator it=dict.begin();it!=dict.end();it++){
//			cout<<"right out: "<<it->first<<" "<<it->second<<endl;
//			circle(src6, Point(it->second,20*it->first), 3, CV_RGB(255,255,255), 1);
//		}
//		imshow("image3", src6);

		float kb = 99999.9;
		if (!eliminatePointDict(dict))
			kb = slope(dict, 1);
//		cout<<"slope: "<<kl<<" "<<kr<<endl;
//		waitKey();

		cout<<"sumDiff1: "<<sumDiff1<<"; sumDiff2: "<<sumDiff2<<endl;
		if(sumDiff1>sumDiff2)
		{
			result = kb;
//			cout<<"KB SECLECTED; TURN LEFT"<<endl;
			return 2;
		}
		else
		{
			result = kt;
//			cout<<"KB SECLECTED; TURN RIGHT"<<endl;
			return 1;
		}

	}

	//horizontal direction, we only find blocks for such kinds of words
	if (maxTp != -1) {

		cout << "horizontal max tp " << maxTp << " " << secTp << " " << vertN << " "
				<< verts.size() << endl;
		Mat src3 = Mat::zeros(src.rows, src.cols, CV_8UC1);

		vector<int> block = tpV[maxTp];
		for (int i = 0; i < block.size(); i++) {

			Vec4i myline = lines[block[i]];

			line(src3, cv::Point(myline[0], myline[1]),
					cv::Point(myline[2], myline[3]), CV_RGB(255, 255, 255));

		}

		Mat eroElm = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
		Mat src4;
		erode(src3, src4, eroElm);

		Mat eroElm2 = getStructuringElement(MORPH_RECT, Size(INTERVAL, INTERVAL), Point(1, 1));
		dilate(src4, src4, eroElm2);
//		Mat dst4;
//		Size size4(src4.cols/3 ,src4.rows/3);
//		resize(src4, dst4, size4);
//		imshow("image2", dst4);
//		waitKey();
		//Canny( src4, src4, 100, 200, 3 )//;
		Mat nonZeroCoordinates;
		findNonZero(src4, nonZeroCoordinates);
		map<int, int> dict;

		int up = 9999, bt = -1, left = 9999, right = -1;
		int upx, btx, lefty, righty;

		for (int i = 0; i < nonZeroCoordinates.total(); i++) {
			//cout << "Zero#" << i << ": " << nonZeroCoordinates.at<Point>(i).x << ", " << nonZeroCoordinates.at<Point>(i).y << endl;
			Point p = nonZeroCoordinates.at<Point>(i);
			int x = p.x;
			int y = p.y;
			if(y>0.1*src4.rows&&y<0.9*src4.rows){
				if (dict.find(y / INTERVAL) != dict.end()) {
					if (x < dict[y / INTERVAL])
						dict[y / INTERVAL] = x;
				} else {
					dict[y / INTERVAL] = x;
				}
			}
		}

		vector<int> vec3;
//		cout<<"vec3:"<<endl;
		for(map<int,int>::iterator i=dict.begin();i!=dict.end();i++){
			vec3.push_back(i->second);
//			cout<<i->second<<endl;
		}
		int sumDiff3 = dIntegral(vec3)/vec3.size();//summarize different?

		Mat src5 = Mat::zeros(src4.rows,src4.cols,CV_8UC1);
		for(map<int,int>::iterator it=dict.begin();it!=dict.end();it++){
			circle(src5, Point(it->second,INTERVAL*it->first), 3, CV_RGB(255,255,255), 1);
		}

//		Mat dst5;
//		Size size5(src5.cols/3 ,src5.rows/3);
//		resize(src5, dst5, size5);
//
//		imshow("image2", dst5);
//		waitKey();

		float kl = 99999.9;
		if (!eliminatePointDict(dict))
			kl = slope(dict, 1);

		dict.clear();

		for (int i = 0; i < nonZeroCoordinates.total(); i++) {
			//cout << "Zero#" << i << ": " << nonZeroCoordinates.at<Point>(i).x << ", " << nonZeroCoordinates.at<Point>(i).y << endl;
			Point p = nonZeroCoordinates.at<Point>(i);
			int x = p.x;
			int y = p.y;
			if(y>0.1*src4.rows&&y<0.9*src4.rows){
				if (dict.find(y / INTERVAL) != dict.end()) {
					if (x > dict[y / INTERVAL])
						dict[y / INTERVAL] = x;
				} else {
					dict[y / INTERVAL] = x;
				}
			}
		}

		vector<int> vec4;
//		cout<<"vec4:"<<endl;
		for(map<int,int>::iterator i=dict.begin();i!=dict.end();i++){
			vec4.push_back(i->second);
//			cout<<i->second<<endl;
		}
		int sumDiff4 = dIntegral(vec4)/vec4.size();

//		Mat src6 = Mat::zeros(src4.rows,src4.cols,CV_8UC1);
//		for(map<int,int>::iterator it=dict.begin();it!=dict.end();it++){
//			cout<<"right out: "<<it->first<<" "<<it->second<<endl;
//			circle(src6, Point(it->second,20*it->first), 3, CV_RGB(255,255,255), 1);
//		}
//		imshow("image3", src6);

		float kr = 99999.9;
		if (!eliminatePointDict(dict))
			kr = slope(dict, 1);
//		cout<<"slope: "<<kl<<" "<<kr<<endl;
//		waitKey();

		cout<<"sumDiff3: "<<sumDiff3<<"; sumDiff4: "<<sumDiff4<<endl;
		if(sumDiff3>sumDiff4)
		{
//			cout<<"KR SELECTED; NONEED TO TURN"<<endl;
			result = kr;

			if(fabs(kr)>1)
				return 3;
			else{
				if(kr>=0&&kr<=1)
					return 1;
				return 2;
			}
		}
		else{
//			cout<<"KL SELECTED; TURN 180 DEGREE"<<endl;
			result = kl;
			if(fabs(kl)>1)
				return 0;
			if(kl<=0&&kl>-1)
				return 1;
			return 2;
		}


		//Mat src7 = src.clone();

//		myDrawLine(0, upx, up - 10, k0, src.cols, src.rows);
//		myDrawLine(1, btx, bt + 10, k0, src.cols, src.rows);
//		myDrawLine(2, left - 10, lefty, kl, src.cols, src.rows);
//		myDrawLine(3, right + 10, righty, kr, src.cols, src.rows);
//		if (!border)
//			textBorder(orig, src, rst);

//		imshow("imageF",src7);
//		waitKey();
	}
//	return atan(result);
	return 0;
}

int getDirection(Mat src){
	if(textorient==-10)
		textorient = getTextOrient(src);
	 cout<<"text angle: "<<textorient<<endl;
	if(fabs(textorient)>=0 && fabs(textorient) <=CV_PI/4)
	{
		cout<<"no change"<<endl;
		return NOCHANGE;
	}
	if(textorient>CV_PI/4 && textorient <=3*CV_PI/4)
	{
		cout<<"turn left"<<endl;
		return NINETY;
	}
	if(textorient>3*CV_PI/4 && textorient <=5*CV_PI/4)
	{
		cout<<"upset down"<<endl;
		return OPPOSITE;
	}
	if(textorient>5*CV_PI/4 && textorient <=7*CV_PI/4)
	{
		cout<<"turn right"<<endl;
		return R_NINETY;
	}
	if(textorient<-CV_PI/4 && textorient >=-3*CV_PI/4)
	{
		cout<<"turn right"<<endl;
		return R_NINETY;
	}
	cout<<"NOT SURE"<<endl;
}

#endif /* TEXTORIENT_H_ */
