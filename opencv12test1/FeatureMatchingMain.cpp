
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
	
	/*cv::initModule_nonfree();
	int a = getLastAddId();
	std::cout << "Last Add Id: " << a << endl;
	
	addANewAdvertisement(a+1, "add three");
	Mat img2 = imread("source.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	extractFeaturesAndCreateFeatureFile(a+1,img2, "f1");
	*/
	Mat img2 = imread("source.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	matchKeypoints(img2, "dfs");

	//histogramMatching();
	//templateMatching();
	//flannMatching();
	//bruteForceMatching();
	//system("pause");
	return 0;
}

