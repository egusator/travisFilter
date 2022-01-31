#include <iostream>
#include <opencv2/opencv.hpp>
#include <experimental/random> // for randint function
//  NOT "using namespace std;" here, to avoid collisions between the beta variable and std::beta in c++17
using std::cout;
using std::endl;
using namespace cv;

//parameters for
#define ALPHA 90 // contrast
#define BETA 60 // brightness
#define GAMMA 100 // gamma-correction
#define ADD_YELLOW_DEPTH 20 // adding yellow tone
#define GRAIN_DEPTH 50 // grain
#define INPUT_PHOTO_PATH "rappa.jpg"
#define OUTPUT_PHOTO_PATH "output.jpg"

namespace travisFilter {
void correction(Mat &img, double alpha_, int beta_,
		double gamma_);

void addYellow(Mat& img, int depth);

void grain (Mat& img, int depth);
}

int main(int argc, char **argv) {
	try {
	using namespace travisFilter;

	if (argc == 1) {
		std::string path = INPUT_PHOTO_PATH;
	} else {
		std::string path = argv[1];
	}

	Mat img_original = imread(INPUT_PHOTO_PATH), img_corrected;
	cout << "Filtering " << INPUT_PHOTO_PATH << "\n";

	//resize(img_original, img_original, Size(0, 0), 0.5, 0.5, INTER_LINEAR);
	if (img_original.empty()) {
		cout << "Could not open or find the image!\n" << endl;
		return -1;
	}

	img_corrected = img_original.clone();

//brightness and contrast filter
	travisFilter::correction(img_corrected, ALPHA / 100.0, BETA - 100, GAMMA / 100.0);

//adding random noises
	grain(img_corrected, GRAIN_DEPTH);

// adding some yellow tone
	addYellow(img_corrected, ADD_YELLOW_DEPTH);

// blur
	GaussianBlur( img_corrected, img_corrected, Size( 1, 1 ), 0, 0);

	cout << "Results in " << OUTPUT_PHOTO_PATH << "\n";
	imwrite(OUTPUT_PHOTO_PATH, img_corrected);
	return 0;
	} catch (std::exception stdException) {
		cout << stdException.what() << "\n";
	} catch (cv::Exception cvException) {
		cout << cvException.what() << "\n";
	} catch (...) {
		cout << "unknown error\n";
	}
}

void travisFilter::correction(Mat &img,  double alpha_, int beta_,
		double gamma_) {
	Mat res1;
	img.convertTo(res1, -1, alpha_, beta_);
	CV_Assert(gamma_ >= 0);
	Mat lookUpTable(1, 256, CV_8U);
	uchar *p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma_) * 255.0);
	Mat res = img.clone();
	LUT(res1, lookUpTable, res);
	img = res.clone();
}

void travisFilter::grain(Mat& img, int depth) {

	using namespace std::experimental::fundamentals_v2;

	for (int i = 0; i < img.rows; i++)
			for (int j = 0; j < img.cols; j++) {
				(-depth, depth);
				int a = img.at<Vec3b>(i, j)[0]
						+ randint(-1 * depth,	depth),

					b = img.at<Vec3b>(i, j)[1]
						+ randint(-1 * depth, depth)/2,

					c = img.at<Vec3b>(i, j)[2]
						+ randint(-1 * depth, depth)/1.5;

				if (a < 0) 	a = 0;
				if (b < 0) b = 0;
				if (c < 0) c = 0;
				if (a > UCHAR_MAX) a = UCHAR_MAX;
				if (b > UCHAR_MAX) b = UCHAR_MAX;
				if (c > UCHAR_MAX) c = UCHAR_MAX;

				img.at<Vec3b>(i, j)[0] = a;
				img.at<Vec3b>(i, j)[1] = b;
				img.at<Vec3b>(i, j)[2] = c;
			}
}

void travisFilter::addYellow(Mat& img, int depth){
	for (int i = 0; i < img.rows; i++)
				for (int j = 0; j < img.cols; j++) {
					(-depth, depth);
					int a = img.at<Vec3b>(i, j)[0],
							b = img.at<Vec3b>(i, j)[1],
								c = img.at<Vec3b>(i, j)[2];
					int S = (a + b + c) / 3;
					a -= 5;
					b = b + 0.15*(S + depth);
					c = c + 0.30*S;

					if (a < 0) a = 0;
					if (b < 0) b = 0;
					if (c < 0) c = 0;
					if (a > UCHAR_MAX) a = UCHAR_MAX;
					if (b > UCHAR_MAX) b = UCHAR_MAX;
					if (c > UCHAR_MAX) c = UCHAR_MAX;

					img.at<Vec3b>(i, j)[0] = a;
					img.at<Vec3b>(i, j)[1] = b;
					img.at<Vec3b>(i, j)[2] = c;
				}
}
