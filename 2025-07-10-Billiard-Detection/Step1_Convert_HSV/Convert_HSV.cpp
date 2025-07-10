#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;

cv::Mat img, hsvImage;

int main() {
    // Step 1: Load the image
    img = cv::imread("billiard_table.jpg");
    if (img.empty()) {
        cerr << "[Error] Failed to load image!" << endl;
        return -1;
    }
    cout << "[Step 1] Image loaded successfully." << endl;

    // Step 1: Convert to HSV
    cv::cvtColor(img, hsvImage, cv::COLOR_BGR2HSV);
    cout << "[Step 1] Converted to HSV color space." << endl;

    // Save results (for debugging)
    // (Uncomment when needed)
    /*
    cv::imwrite("step1_original.jpg", img);
    cv::imwrite("step1_hsv.jpg", hsvImage);
    cv::imshow("Original", img);
    cv::imshow("HSV", hsvImage);
    cv::waitKey(0);
    */

    return 0;
}
