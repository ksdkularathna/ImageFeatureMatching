#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <fstream>
#include "FeatureMatching.h" 
#include <string>
#include <direct.h>

#include "opencv2/opencv_modules.hpp"
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"

using namespace std;
using namespace cv;

int getLastAddId() {

	ifstream ip("add_index.csv");

	if (!ip.is_open()) std::cout << "ERROR: add_index.csv File Open" << '\n';

	string addIdString;
	string addName;

	ip.seekg(-1, ios_base::end);

	// reading the end of the csv file and getting the last add id
	bool keepLooping = true;					// go to one spot before the EOF

	while (keepLooping) {
		char ch;
		ip.get(ch);								// Get current byte's data

		if ((int)ip.tellg() <= 1) {             // If the data was at or before the 0th byte
			ip.seekg(0);						// The first line is the last line
			keepLooping = false;                // So stop there
		}
		else if (ch == '\n') {                  // If the data was a newline
			keepLooping = false;                // Stop at the current position.
		}
		else {                                  // If the data was neither a newline nor at the 0 byte
			ip.seekg(-2, ios_base::cur);        // Move to the front of that data, then to the front of the data before it
		}
	}

	getline(ip, addIdString, ',');
	getline(ip, addName, '\n');
	int addId = std::stoi(addIdString);			// last add id

												//std::cout << "Add Id: " << addId << " Add Name " << addName << '\n';
	ip.close();
	return addId;
}

int addANewAdvertisement(int addId, string addName) {

	std::ofstream outfile;

	outfile.open("add_index.csv", std::ios_base::app);
	outfile << '\n' << addId << "," << addName;
	std::cout << "id " << addId << " advertisement added." << '\n';
	outfile.close();

	// creating a directories for newly added addvertisement
	// TODO : create other directories too
	string directory1 = "db/" + std::to_string(addId);
	//string directory2 = "db/" + std::to_string(addId) + "/hist_features";
	//string directory3 = "db/" + std::to_string(addId) + "/surf_features";
	_mkdir(directory1.c_str());
	//_mkdir(directory2.c_str());
	//_mkdir(directory3.c_str());

	std::cout << "New directory created " << '\n';
	return 0;
}

int extractFeaturesAndCreateFeatureFile(int addId, Mat image2, string frameName) {

	//-------------- adding the histogram features to the database
	/*
	Mat hsv_base;
	if (image1.empty())
	{
		cout << "Can't read the images" << endl;
		return -1;
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

	// save calculated histogram of the frame to the feature file
	cv::FileStorage fs1("db/" + std::to_string(addId) + "/hist_features/" + frameName + ".yml", cv::FileStorage::WRITE);
	fs1 << frameName << hist_base;
	fs1.release();

	cout << "Histogram features added to the feature file" << endl;
	*/
	
	//--------------- adding surf features to the database
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);

	std::vector<KeyPoint> keypoints;
	keypoints.reserve(10000);
	detector.detect(image2, keypoints);

	SurfDescriptorExtractor extractor;
	
	// feature vectors
	Mat descriptors_1, descriptors_2;
	extractor.compute(image2, keypoints, descriptors_1);

	// sotring the key points to a file
	cv::FileStorage fs2("db/" + std::to_string(addId) + "/" + frameName + ".yml", cv::FileStorage::WRITE);
	write(fs2, frameName, descriptors_1);
	fs2.release();

	cout << "Surf features added to the feature file" << endl;
	return 0;
}