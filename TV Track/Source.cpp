#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <Windows.h>
#include <fstream>
#include "FeatureMatching.h" 
#include <string>
#include <sstream>
#include <opencv2/ml.hpp>

using namespace cv;
using namespace std;

typedef struct {
	float sd[4];
	float mean[4];
	float mode[4];
	float eudis[4];
	float sdf[4][4];
	int array[4][256];
}wrap;

wrap histogramcalculation(const Mat &Image)
{
	wrap w;

	int rows = Image.rows;
	int cols = Image.cols;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 256; j++)
			w.array[i][j] = 0;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			w.sdf[i][j] = 0;
	}

	for (int j = 0; j < 4; j++) {

		int x1, x2, y1, y2;

		if (j == 0) { x1 = 0; y1 = 0; x2 = rows / 2; y2 = cols / 2; }
		else if (j == 2) { x1 = rows / 2; y1 = 0; x2 = rows; y2 = cols / 2; }
		else if (j == 3) { x1 = 0; y1 = cols / 2; x2 = rows / 2; y2 = cols; }
		else { x1 = rows / 2; y1 = cols / 2; x2 = rows; y2 = cols; }

		w.mean[j] = 0;
		w.sd[j] = 0;

		for (int i = x1; i < x2; i++) {
			for (int c = y1; c < y2; c++) {
				int m = (int)Image.at<uchar>(i, c);
				w.array[j][m] = w.array[j][m] + 1;
				w.mean[j] += m;
			}
		}

		w.mean[j] = w.mean[j] / ((rows / 2)*(cols / 2));

		for (int i = x1; i < x2; i++) {
			for (int c = y1; c < y2; c++) {
				int m = (int)Image.at<uchar>(i, c);
				w.sd[j] += pow((m - w.mean[j]), 2);
			}
		}

		w.sd[j] = sqrt((w.sd[j] / ((rows / 2)*(cols / 2))));

		int temp = 0;

		for (int i = 0; i < 256; i++) {
			if (temp < w.array[j][i]) {
				w.mode[j] = i;
				temp = w.array[j][i];
			}
		}

		for (int i = 0; i < 4; i++)
			w.eudis[i] = 0;
	}
	return w;
}

wrap currentHist;
wrap prevHist;

int winsize = 10;
int rounds = 2;

float window[2][4][4][10];
/*******************
i - round
j - frame part
k - feature
l - window
********************/

int main()
{
	try {

		std::ifstream infile("featureExtractBegin.txt");
		string a, b;
		infile >> a >> b;
		infile.close();

		int count = 0;

		VideoCapture cap("D:\\Projects\\ads\\recording\\Output.mp4"); // open video
		//VideoCapture cap("D:\\Projects\\ads\\ad1\\ad1.mp4"); // open video
		//VideoCapture cap("D:\\Projects\\ads\\recording\\recording.mp4");

		//VideoCapture cap(a);

		if (!cap.isOpened())  // check if we succeeded
			return -1;

		//fstream result;
		//result.open("C:\\Users\\RaminduS\\Desktop\\result.csv");

		int skip = 0;

		//int addId = getLastAddId();
		//addANewAdvertisement(addId + 1, b);

		int matchedAdId = 0;
		int status = 1;
		adDetails ad;
		int currentAddLastFrameId = 0;
		int matchCount = 0;
		int startingFrameNumber = 0;

		while (1)
		{
			Mat frame;
			cap >> frame; // get a new frame

			int frameNo = cap.get(CV_CAP_PROP_POS_FRAMES);

			imshow("video", frame);
			cvtColor(frame, frame, CV_BGR2GRAY);

			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 4; j++)
					for (int k = 0; k < 4; k++)
						for (int l = 0; l < winsize; l++)
							window[i][j][k][l] = 0;

			currentHist = histogramcalculation(frame);

			float sdsvm[16];

			if (count > 0) {
				int success[4] = { 0,0,0,0 };
				for (int j = 0; j < 4; j++) {
					for (int b = 0; b < 4; b++) {
						for (int a = 0; a < winsize - 1; a++) {
							window[0][j][b][a] = window[0][j][b][a + 1];
						}
					}

					float eudis = 0;

					for (int arri = 0; arri < 256; arri++) {
						eudis += pow(currentHist.array[j][arri] - prevHist.array[j][arri], 2);
					}

					eudis = sqrt(eudis);
					//eudis = eudis / currentHist.mean[j];

					float mean[4] = { 0,0,0,0 };
					float sd[4] = { 0,0,0,0 };

					window[0][j][0][winsize - 1] = currentHist.mean[j] - prevHist.mean[j];
					window[0][j][1][winsize - 1] = currentHist.sd[j] - prevHist.sd[j];
					window[0][j][2][winsize - 1] = (3 * (currentHist.mean[j] - currentHist.mode[j]) / (currentHist.sd[j])) - (3 * (prevHist.mean[j] - currentHist.mode[j]) / (prevHist.sd[j]));
					window[0][j][3][winsize - 1] = eudis;

					currentHist.eudis[j] = window[0][j][3][winsize - 1];

					for (int k = 0; k < 4; k++) {
						for (int i = 0; i < winsize; i++) {
							mean[k] += window[0][j][k][i];
						}
						mean[k] /= winsize;
					}

					for (int k = 0; k < 4; k++) {
						for (int i = 0; i < winsize; i++) {
							sd[k] = pow(window[0][j][k][i] - mean[k], 2);
						}
						sd[k] /= winsize;
						sd[k] = sqrt(sd[k]);
					}

					int xeudis = 0;
					if (eudis - prevHist.eudis[3] > 0)
						xeudis = eudis - prevHist.eudis[3];
					//for(int rr=0;rr<4;rr++)

					for (int i = 0; i < 4; i++)
						currentHist.sdf[j][i] = sd[i];

					/*******************************************************************************/

					float mean2[10];
					float sd2[10];

					for (int b = 0; b < 4; b++) {
						for (int a = 0; a < winsize - 1; a++) {
							window[1][j][b][a] = window[1][j][b][a + 1];
						}
					}

					for (int i = 0; i < 4; i++)
						window[1][j][i][winsize - 1] = currentHist.sdf[j][i] - prevHist.sdf[j][i];

					for (int k = 0; k < 4; k++) {
						for (int i = 0; i < winsize; i++) {
							mean2[k] += window[1][j][k][i];
						}
						mean2[k] /= winsize;
					}

					for (int k = 0; k < 4; k++) {
						for (int i = 0; i < winsize; i++) {
							sd2[k] = pow(window[1][j][k][i] - mean2[k], 2);
						}
						sd2[k] /= winsize;
						sd2[k] = sqrt(sd2[k]);
						//result << sd2[k] << ",";
						sdsvm[(j * 4) + k] = sd2[k];
					}

				}
			}
			else {

			}

			/*CvSVM svm;
			//Call train function
			//svm.train(trainDataMat, lablesMat, cv::Mat(), cv::Mat(), params);
			//svm.save("asdddd","hhhh");
			svm.load("asdddd", "hhhh");

			//float testData[] = { 0.0139165,	0.00480201,	0.000317721,	2.66071,	0.004797,	0.00270302,	0.000281183,	13.2843,	0.00990877,	0.00689875,	0.00000446,	185.648,	0.0126204,	0.0267251,	0.00103999,	390.978 };

			cv::Mat testDataMat(16, 1, CV_32FC1, sdsvm);
			//Predict the class labele for test data sample
			float predictLable = svm.predict(testDataMat);
			if(predictLable>0.5)
			printf("\t\t%d\t\t\n", frameNo);
			*/

			float score = sdsvm[0] + (sdsvm[1] * 5 / 3) + (sdsvm[2] * 100 / 3) + (sdsvm[3] / 1500)
				+ sdsvm[4] + (sdsvm[5] * 5 / 3) + (sdsvm[6] * 100 / 3) + (sdsvm[7] / 1500)
				+ sdsvm[8] + (sdsvm[9] * 5 / 3) + (sdsvm[10] * 100 / 3) + (sdsvm[11] / 1500)
				+ sdsvm[12] + (sdsvm[13] * 5 / 3) + (sdsvm[14] * 100 / 3) + (sdsvm[15] / 1500);

			printf("%d\t\t\%f\n", frameNo, score);


			if (score > 7 && skip <= 0) {
				printf("*********************************************************");
				if (status == 2) {
					// this means second phase of the search. peform the search in the choosen directory
					ad = matchKeypoints(frame, "12", 2, matchedAdId);
					if (ad.adId == 0) {
						// means match is not found in the choosen director
						// then perform a status 1 search to check whether a new ad started or not
						ad = matchKeypoints(frame, "12", 1, 0);
						if (ad.adId != 0) {
							// means a new ad started. 
							// change the status as 2 to perform the choosen directory search
							matchedAdId = ad.adId;
							status = 2;

							float duration = ((float)frameNo - (float)startingFrameNumber) / 30;

							std::cout << "Frame match count " << matchCount << endl;
							std::cout << "Add duration " << duration << endl;
							matchCount = 0;
							startingFrameNumber = frameNo;
						}
						else {
							// nor a new add is started. may be a frame miss match
							// skip to the next frame and continue the status 2 search
							status = 2;
						}
					}
					else {
						// match is found in the direcotory. make the status 2 to continue the searching in the directory
						matchedAdId = ad.adId;
						status = 2;
						matchCount++;
					}
					std::cout << " Add id " << ad.adId << " Frame Id  " << ad.frameId << " status 2 " << " duration " << (float)currentAddLastFrameId / 30 << std::endl;
				}

				if (status == 1) {
					// this means initial status of search. perform the search in only first two files of the directories.
					ad = matchKeypoints(frame, "12", 1, 0);
					if (ad.adId != 0) {
						// a match is found in a directory. make the status as 2 to perform the search inside the direcotry
						matchedAdId = ad.adId;
						status = 2;
						matchCount++;
						startingFrameNumber = frameNo;
					}
					else {
						// match is not found. continue status 1 search.
						status = 1;
					}
					std::cout << " Add id " << ad.adId << " Frame Id " << ad.frameId << " from status 1 " << std::endl;
				}
				skip = 5;
			}

			prevHist = currentHist;
			count++;
			skip--;
			waitKey(5);
			//if(frameNo>340)
			//Sleep(100);
			//printf("%d",frameNo);
		}
	}
	catch (cv::Exception& e) {
		return 0;
	}
	return 1;
}

int main2() {

	float trainarray[11000][16];

	ifstream in("all.csv");

	string line, field;

	for (int i = 0; i < 11000; i++)
	{
		getline(in, line);
		stringstream ss(line);

		for (int j = 0; j < 16; j++)
		{
			getline(ss, field, ',');
			field += field + "0000000000";
			field = field.substr(0, 6);
			try
			{
				trainarray[i][j] = stof(field);
			}
			catch (int x) {
				trainarray[i][j] = 0;
			}
			printf("%f ", trainarray[i][j]);

		}
		printf("\n");
	}

	float labels[11000];

	ifstream in2("results.csv");

	for (int i = 0; i < 11000; i++)
	{
		getline(in2, line);
		labels[i] = stof(line);
		printf("%d\n", i);
	}

	cv::Mat lablesMat(11000, 1, CV_32FC1, labels);
	cv::Mat trainDataMat(11000, 16, CV_32FC1, trainarray);


	//Define parameters for SVM
	CvSVMParams params;
	//SVM type is defined as n-class classification n>=2, allows imperfect separation of classes
	params.svm_type = CvSVM::C_SVC;
	// No mapping is done, linear discrimination (or regression) is done in the original feature space.
	params.kernel_type = CvSVM::LINEAR;
	//Define the termination criterion for SVM algorithm.
	//Here stop the algorithm after the achieved algorithm-dependent accuracy becomes lower than epsilon
	//or run for maximum 100 iterations
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 1e-6);

	CvSVM svm;
	//Call train function
	svm.train(trainDataMat, lablesMat, cv::Mat(), cv::Mat(), params);
	svm.save("asdddd", "hhhh");
	//svm.load("asdddd", "hhhh");

	float testData[] = { 0.0139165,	0.00480201,	0.000317721,	2.66071,	0.004797,	0.00270302,	0.000281183,	13.2843,	0.00990877,	0.00689875,	0.00000446,	185.648,	0.0126204,	0.0267251,	0.00103999,	390.978 };

	cv::Mat testDataMat(16, 1, CV_32FC1, testData);
	//Predict the class labele for test data sample
	float predictLable = svm.predict(testDataMat);

	printf("\n\t\t%f\t\t\n", predictLable);

	waitKey(5);
}
