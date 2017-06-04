#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include "FeatureMatching.h" 

using namespace std;
using namespace cv;

// added for directory reading
#include <string>
#include <filesystem>

#include <iostream>
#include <fstream>
namespace fs = std::experimental::filesystem;

int histogramMatching() {

	Mat src_base, hsv_base;
	Mat src_test1, hsv_test1;

	ofstream myfile;
	myfile.open("historgram_comparison_data.csv", std::ios_base::app);
	int i = 1;
	std::string path = "1/";
	for (auto & p : fs::directory_iterator(path)) {
		//std::cout << p.path().filename().string() << endl;
		src_base = imread("1/" + p.path().filename().string(), IMREAD_COLOR);
		src_test1 = imread("2/" + p.path().filename().string(), IMREAD_COLOR);

		if (src_base.empty() || src_test1.empty())
		{
			cout << "Can't read one of the images" << endl;
			return -1;
		}

		const clock_t begin_time = clock();
		

		/// Convert to HSV
		cvtColor(src_base, hsv_base, COLOR_BGR2HSV);
		cvtColor(src_test1, hsv_test1, COLOR_BGR2HSV);

		/// Using 50 bins for hue and 60 for saturation
		int h_bins = 50; int s_bins = 60;
		int histSize[] = { h_bins, s_bins };

		// hue varies from 0 to 179, saturation from 0 to 255
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 256 };

		const float* ranges[] = { h_ranges, s_ranges };

		// Use the o-th and 1-st channels
		int channels[] = { 0, 1 };

		/// Histograms
		MatND hist_base;
		MatND hist_test1;
		MatND hist_test2;

		/// Calculate the histograms for the HSV images
		calcHist(&hsv_base, 1, channels, Mat(), hist_base, 2, histSize, ranges, true, false);
		normalize(hist_base, hist_base, 0, 1, NORM_MINMAX, -1, Mat());

		calcHist(&hsv_test1, 1, channels, Mat(), hist_test1, 2, histSize, ranges, true, false);
		normalize(hist_test1, hist_test1, 0, 1, NORM_MINMAX, -1, Mat());

		/// Apply the histogram comparison methods
		/*for (int i = 0; i < 4; i++)
		{
			int compare_method = i;
			double base_base = compareHist(hist_base, hist_base, compare_method);
			double base_test1 = compareHist(hist_base, hist_test1, compare_method);
			myfile << "a,b,c,\n";
			printf(" Method [%d] Perfect, Base-Test(1) : %f, %f \n", i, base_base, base_test1);
		}
		*/
		int compare_method = 3;
		double base_base = compareHist(hist_base, hist_base, compare_method);
		double base_test1 = compareHist(hist_base, hist_test1, compare_method);
		//myfile << "a,b,c,\n";
		printf(" Method [%d] Perfect, Base-Test(1) : %f, %f %f \n", compare_method, base_base, base_test1, float(clock() - begin_time) / CLOCKS_PER_SEC);
		myfile << i << "," << base_base << "," << base_test1 << "," << float(clock() - begin_time) / CLOCKS_PER_SEC << "\n";
		i++;
	}
	myfile.close();
	printf("Done \n");
	return 0;
}