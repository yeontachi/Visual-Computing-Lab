void extractTableCorners(const cv::Mat& binaryMask, cv::Mat& visualOutput) {
    vector<vector<cv::Point>> contours;
    vector<cv::Point> approx, hull;

    // 윤곽선 찾기
    cv::findContours(binaryMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 가장 큰 윤곽선 찾기
    double maxArea = 0.0;
    int maxIdx = -1;
    for (int i = 0; i < contours.size(); ++i) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea) {
            maxArea = area;
            maxIdx = i;
        }
    }

    if (maxIdx == -1) return;

    // 꼭짓점 근사화 및 볼록 다각형 보정
    cv::approxPolyDP(contours[maxIdx], approx, 20, true);
    cv::convexHull(approx, hull);
    tableCorners = hull;  // 전역 변수로 저장

    // 시각화
    visualOutput = img.clone();
    for (size_t i = 0; i < hull.size(); ++i) {
        cv::circle(visualOutput, hull[i], 8, cv::Scalar(0, 0, 255), -1);
        cv::putText(visualOutput, to_string(i), hull[i] + cv::Point(5, -5),
            cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
    }
    cv::drawContours(visualOutput, contours, maxIdx, cv::Scalar(255, 0, 0), 2);

    // 저장
    cv::imwrite("step3_corners.jpg", visualOutput);
    cout << "[Step 3] Saved corner-annotated image as step3_corners.jpg" << endl;
}
