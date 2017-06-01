#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "FeatureMatching.h"

using namespace std;
using namespace cv;

// added for flann matching
#include "opencv2/opencv_modules.hpp"
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include <fstream>

// added for directory reading
#include <string>
#include <filesystem>
namespace fs = std::experimental::filesystem;

int matchKeypoints(Mat image, string time) {

	//-------------- adding the histogram features to the database
	Mat hsv_base;
	if (image.empty())
	{
		cout << "Can't read the images" << endl;
		return -1;
	}

	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;
	SurfFeatureDetector detector(minHessian);
	vector<KeyPoint> keypoints_1;
	keypoints_1.reserve(10000);
	detector.detect(image, keypoints_1);;

	SurfDescriptorExtractor extractor;
	Mat descriptors_1, descriptors_2;
	extractor.compute(image, keypoints_1, descriptors_1);

	std::string path = "db/";
	int goodMatchesCount = 0;
	int mostMatchedAddId = 0;

	bool isNestedLoopsBreaked = false;
	for (auto & p : fs::directory_iterator(path)) {

		//std::cout << p.path().filename() << std::endl;
		bool outerBreak = false;
		for (auto & ip : fs::directory_iterator(p)) {

			//std::cout << ip << std::endl;

			string filePath = ip.path().relative_path().string();
			string fileName = ip.path().filename().string();
			std::size_t pos = fileName.find(".");
			string descriptorId = fileName.substr(0, 0 + pos);

			FileStorage fs2(filePath, FileStorage::READ);
			FileNode kptFileNode = fs2[descriptorId];
			read(kptFileNode, descriptors_2);
			fs2.release();

			//-- Step 3: Matching descriptor vectors using FLANN matcher
			FlannBasedMatcher matcher;
			std::vector< DMatch > matches;
			matches.reserve(10000);
			matcher.match(descriptors_1, descriptors_2, matches);

			// Evaluate the results
			double max_dist = 0; double min_dist = 100;
			//-- Quick calculation of max and min distances between keypoints
			for (int i = 0; i < descriptors_1.rows; i++)
			{
				double dist = matches[i].distance;
				if (dist < min_dist) min_dist = dist;
				if (dist > max_dist) max_dist = dist;
			}

			if (min_dist == 0 && max_dist == 0) {

				// this means a perfect match is found. No need to search further. loops are breaked.
				// adding the entry to the log
				std::ofstream outfile;

				outfile.open("matched_adds.csv", std::ios_base::app);
				outfile << p.path().filename().string() << "," << time << endl;
				outfile.close();

				outerBreak = true;
				break;
			}

			//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
			//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
			//-- small)
			//-- PS.- radiusMatch can also be used here.
			std::vector< DMatch > good_matches;

			for (int i = 0; i < descriptors_1.rows; i++)
			{
				if (matches[i].distance <= max(2 * min_dist, 0.02))
				{
					good_matches.push_back(matches[i]);
				}
			}
			if (good_matches.size() > goodMatchesCount) {

				goodMatchesCount = good_matches.size();
				mostMatchedAddId = std::stoi(p.path().filename().string());
			}
			//std::cout << good_matches.size() << std::endl;
		}
		if (outerBreak == true) {

			isNestedLoopsBreaked = true;
			break;
		}
	}
	if (isNestedLoopsBreaked != true) {

		// this means there are no any exact matches. search is performed in all the files
		// adding the best match add id to the log
		std::ofstream outfile;
		outfile.open("matched_adds.csv", std::ios_base::app);
		outfile << mostMatchedAddId << "," << time << endl;
		outfile.close();

	}
	//std::cout << "Most feature matched count" << goodMatchesCount << std::endl;
	//std::cout << "Matched add ID " << mostMatchedAddId << std::endl;
	std::cout << "Match finished" << std::endl;
	return 1;
}