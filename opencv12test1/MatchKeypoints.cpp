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
	for (auto & p : fs::directory_iterator(path)) {
		//std::cout << p << std::endl;

		for (auto & ip : fs::directory_iterator(p)) {
			
			string filePath = ip.path().relative_path().string();
			
			FileStorage fs2(filePath, FileStorage::READ);
			FileNode kptFileNode = fs2[ip.path().filename().string()];
			read(kptFileNode, descriptors_2);
			fs2.release();

			cv::FileStorage fs3("db/2/checking1.yml", cv::FileStorage::WRITE);
			write(fs3, "s1", descriptors_2);
			fs3.release();

			std::cout << "file read complete" << std::endl;
			//-- Step 3: Matching descriptor vectors using FLANN matcher
			FlannBasedMatcher matcher;
			std::vector< DMatch > matches;
			//matcher.match(descriptors_1, descriptors_2, matches);
			std::cout << "match complete" << std::endl;
			
		}
	}

	double max_dist = 0; double min_dist = 100;

	/*
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);

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
	*/
	return 1;
}