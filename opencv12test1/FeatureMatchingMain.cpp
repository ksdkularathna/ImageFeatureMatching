
#include "FeatureMatching.h" 
#include <iostream>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;
using namespace cv;

/**
* @function main
*/
int main(int argc, char** argv)
{
	/*
	cv::initModule_nonfree();
	int a = getLastAddId();
	std::cout << "Last Add Id: " << a << endl;
	
	addANewAdvertisement(a+1, "add three");
	//Mat img1 = imread("source.jpg", IMREAD_COLOR);
	Mat img2 = imread("source.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	extractFeaturesAndCreateFeatureFile(a+1,img2, "f1");
	*/
	Mat img2 = imread("source.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	matchKeypoints(img2, "dfs");
	
	//for (std::vector<int>::const_iterator i = x.begin(); i != x.end(); ++i)
	//	std::cout << *i << ' ';
	//std::cout << x.data << endl;

	//histogramMatching();
	//templateMatching();
	//flannMatching();
	//bruteForceMatching();
	//system("pause");
	return 0;
}

