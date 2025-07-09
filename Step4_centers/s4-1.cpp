#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
using namespace std;

// 전역 변수
cv::Mat img, hsvImage;
vector<cv::Point> tableCorners;

// Step 1
bool loadAndConvertImage(const string& filename) {
    img = cv::imread(filename);
    if (img.empty()) return false;
    cv::cvtColor(img, hsvImage, cv::COLOR_BGR2HSV);
    return true;
}

// Step 2
void maskTableArea() {
    cv::Scalar lowerGreen(115, 180, 160), upperGreen(123, 255, 255);
    cv::Mat mask;
    cv::inRange(hsvImage, lowerGreen, upperGreen, mask);
    cv::erode(mask, mask, {}, { -1, -1 }, 2);
    cv::dilate(mask, mask, {}, { -1, -1 }, 2);
    cv::imshow("Step 2 - Table Mask", mask);
    cv::imwrite("step2_table_mask.jpg", mask);
}

// Step 3
void extractTableCorners(const cv::Mat& binaryMask, cv::Mat& visualOutput) {
    vector<vector<cv::Point>> contours;
    vector<cv::Point> approx, hull;
    cv::findContours(binaryMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    double maxArea = 0.0;
    int maxIdx = -1;
    for (int i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea) { maxArea = area; maxIdx = i; }
    }
    if (maxIdx == -1) return;

    cv::approxPolyDP(contours[maxIdx], approx, 20, true);
    cv::convexHull(approx, hull);
    tableCorners = hull;

    visualOutput = img.clone();
    for (size_t i = 0; i < hull.size(); ++i) {
        cv::circle(visualOutput, hull[i], 8, cv::Scalar(0, 0, 255), -1);
        cv::putText(visualOutput, to_string(i), hull[i] + cv::Point(5, -5),
            cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    }
    cv::drawContours(visualOutput, contours, maxIdx, cv::Scalar(255, 0, 0), 2);
    cv::imshow("Step3 - Corners", visualOutput);
    cv::imwrite("step3_corners.jpg", visualOutput);
}

// Step 4: Ball detection
vector<cv::Point> findBallCenters(cv::Scalar lower, cv::Scalar upper, const string& colorName, const string& maskName) {
    vector<cv::Point> centers;
    cv::Mat mask;
    cv::inRange(hsvImage, lower, upper, mask);
    cv::erode(mask, mask, {}, { -1, -1 }, 2);
    cv::dilate(mask, mask, {}, { -1, -1 }, 2);

    // 디버그용 마스크 저장
    cv::imwrite("debug_mask_" + maskName + ".jpg", mask);

    vector<vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    for (const auto& cnt : contours) {
        double area = cv::contourArea(cnt);
        if (area < 30 || area > 1500) continue;

        cv::Moments M = cv::moments(cnt);
        if (M.m00 != 0) {
            int cx = static_cast<int>(M.m10 / M.m00);
            int cy = static_cast<int>(M.m01 / M.m00);
            centers.emplace_back(cx, cy);
            cout << colorName << " ball center: (" << cx << ", " << cy << ")" << endl;
        }
    }
    return centers;
}

// 픽셀 → mm 변환
cv::Point2f convertToMM(const cv::Point& p, const vector<cv::Point>& corners) {
    if (corners.size() != 4) return { -1, -1 };
    float w_mm = 2448.0f, h_mm = 1224.0f;
    float w_px = cv::norm(corners[0] - corners[1]);
    float h_px = cv::norm(corners[0] - corners[3]);
    float scaleX = w_mm / w_px;
    float scaleY = h_mm / h_px;
    float dx = static_cast<float>(p.x - corners[0].x);
    float dy = static_cast<float>(p.y - corners[0].y);
    return { dx * scaleX, dy * scaleY };
}

void drawBallCenters(const vector<cv::Point>& centers, const cv::Scalar& color, const string& label, cv::Mat& output) {
    for (size_t i = 0; i < centers.size(); ++i) {
        cv::circle(output, centers[i], 10, color, 2);
        cv::putText(output, label + to_string(i + 1), centers[i] + cv::Point(5, -5),
            cv::FONT_HERSHEY_SIMPLEX, 0.6, color, 2);
    }
}

// main
int main() {
    string filename = "billiard_table.jpg";
    if (!loadAndConvertImage(filename)) {
        cerr << "Failed to load image." << endl;
        return -1;
    }

    maskTableArea();

    cv::Mat step2Mask = cv::imread("step2_table_mask.jpg", cv::IMREAD_GRAYSCALE);
    if (step2Mask.empty()) {
        cerr << "step2_table_mask.jpg not found!" << endl;
        return -1;
    }

    cv::Mat step3Output;
    extractTableCorners(step2Mask, step3Output);

    // 공 색상별 HSV 범위
    auto redCenters = findBallCenters(cv::Scalar(0, 70, 70), cv::Scalar(10, 255, 255), "Red", "red");
    auto whiteCenters = findBallCenters(cv::Scalar(0, 0, 180), cv::Scalar(180, 70, 255), "White", "white");
    auto orangeCenters = findBallCenters(cv::Scalar(20, 100, 150), cv::Scalar(35, 255, 255), "Orange", "orange");

    // mm 좌표 출력 (예: 빨간 공)
    for (const auto& pt : redCenters) {
        cv::Point2f mm = convertToMM(pt, tableCorners);
        cout << "Red ball mm pos: (" << mm.x << ", " << mm.y << ")" << endl;
    }

    // 시각화
    cv::Mat step4Output = img.clone();
    drawBallCenters(redCenters, cv::Scalar(0, 0, 255), "R", step4Output);
    drawBallCenters(whiteCenters, cv::Scalar(255, 255, 255), "W", step4Output);
    drawBallCenters(orangeCenters, cv::Scalar(0, 165, 255), "O", step4Output);

    cv::imshow("Step 4 - Ball Centers", step4Output);
    cv::imwrite("step4_result.jpg", step4Output);

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

