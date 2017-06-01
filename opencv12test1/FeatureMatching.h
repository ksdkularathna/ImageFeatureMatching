#define FeatureMatching_H
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <string>
using namespace std;
using namespace cv;

int getLastAddId();
int addANewAdvertisement(int addId, std::string addName);
int extractFeaturesAndCreateFeatureFile(int addId, Mat image2, string frameName);


//int templateMatching();
int histogramMatching();
int flannMatching();
int bruteForceMatching();

int  matchKeypoints(Mat image, string time);