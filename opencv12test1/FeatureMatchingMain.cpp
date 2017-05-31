
#include "FeatureMatching.h" 
#include <iostream>

using namespace std;

/**
* @function main
*/
int main(int argc, char** argv)
{
	/*int a = getLastAddId();
	std::cout << "Last Add Id: " << a << endl;
	*/
	//addANewAdvertisement(3, "add three");
	//Mat img1 = imread("source.jpg", IMREAD_COLOR);
	//Mat img2 = imread("source.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//extractFeaturesAndCreateFeatureFile(3,img1,img2, "frame 10");

	Mat img1 = imread("source.jpg", IMREAD_COLOR);
	std::vector<int> x = matchHistograms(img1);
	
	//for (std::vector<int>::const_iterator i = x.begin(); i != x.end(); ++i)
	//	std::cout << *i << ' ';
	//std::cout << x.data << endl;

	//histogramMatching();
	//templateMatching();
	//flannMatching();
	//bruteForceMatching();
}

