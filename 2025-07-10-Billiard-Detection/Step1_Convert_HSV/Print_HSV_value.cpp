#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;

// 글로벌 변수
cv::Mat img, hsvImage;

// 마우스 콜백 함수
void onMouse(int event, int x, int y, int, void*) {
	if (event == cv::EVENT_LBUTTONDOWN) {
		// 좌표가 이미지 범위 내에 있는지 확인
		if (x >= 0 && x < hsvImage.cols && y >= 0 && y < hsvImage.rows) {
			cv::Vec3b hsv = hsvImage.at<cv::Vec3b>(y, x);
			int H = hsv[0];
			int S = hsv[1];
			int V = hsv[2];
			cout << "Clicked at (" << x << ", " << y << ") - HSV: "
				<< H << ", " << S << ", " << V << endl;

			// 클릭한 위치에 동그라미 표시
			cv::Mat imgCopy = img.clone();
			cv::circle(imgCopy, cv::Point(x, y), 5, cv::Scalar(0, 0, 255), -1);
			cv::imshow("Image (Click to get HSV)", imgCopy);
		}
	}
}

int main() {
	// 이미지 로드
	img = cv::imread("billiard_table.jpg");
	if (img.empty()) {
		cerr << "Image load failed!" << endl;
		return -1;
	}
	cout << "Image loaded successfully." << endl;

	// HSV 변환
	cv::cvtColor(img, hsvImage, cv::COLOR_BGR2HSV);

	// 창 생성 및 마우스 콜백 등록
	cv::namedWindow("Image (Click to get HSV)");
	cv::setMouseCallback("Image (Click to get HSV)", onMouse);

	// 초기 이미지 표시
	cv::imshow("Image (Click to get HSV)", img);
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}