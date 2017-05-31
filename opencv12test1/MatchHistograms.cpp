#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "FeatureMatching.h"

using namespace std;
using namespace cv;


std::vector<int>  matchHistograms(Mat image) {

	//-------------- adding the histogram features to the database
	Mat hsv_base;
	if (image.empty())
	{
		cout << "Can't read the images" << endl;
		std::vector<int> vector(10);
		return vector;
	}

	// Convert to HSV
	cvtColor(image1, hsv_base, COLOR_BGR2HSV);

	// Using 50 bins for hue and 60 for saturation
	int h_bins = 50; int s_bins = 60;
	int histSize[] = { h_bins, s_bins };

	// hue varies from 0 to 179, saturation from 0 to 255
	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges };

	// Use the o-th and 1-st channels
	int channels[] = { 0, 1 };

	// Histograms
	MatND hist_base;

	// Calculate the histograms for the HSV images
	calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, false);
	normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());



	std::vector<int> vector(10);
	vector.push_back(10);
	return vector;
}