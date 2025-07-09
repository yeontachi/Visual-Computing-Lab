vector<cv::Point> findBallCenters_HSV_Hough_Red(const string& colorName) {
    vector<cv::Point> centers;
    cv::Mat mask1, mask2, mask, masked;

    // 두 범위 결합 (빨강은 Hue 0과 180 근처에 분포)
    cv::inRange(hsvImage, cv::Scalar(0, 70, 70), cv::Scalar(10, 255, 255), mask1);
    cv::inRange(hsvImage, cv::Scalar(170, 70, 70), cv::Scalar(180, 255, 255), mask2);
    cv::bitwise_or(mask1, mask2, mask);

    // 마스킹 → 그레이 변환 → 블러 → Hough
    cv::bitwise_and(img, img, masked, mask);
    cv::Mat gray;
    cv::cvtColor(masked, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2);

    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, 20, 100, 20, 10, 30);

    for (const auto& c : circles) {
        centers.push_back(cv::Point(cvRound(c[0]), cvRound(c[1])));
    }

    return centers;
}

vector<cv::Point> findBallCenters_HSV_Hough(cv::Scalar lower, cv::Scalar upper, const string& colorName) {
    vector<cv::Point> centers;
    cv::Mat mask, masked;

    cv::inRange(hsvImage, lower, upper, mask);
    cv::bitwise_and(img, img, masked, mask);
    cv::Mat gray;
    cv::cvtColor(masked, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2);

    vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, 20, 100, 20, 10, 30);

    for (const auto& c : circles) {
        centers.push_back(cv::Point(cvRound(c[0]), cvRound(c[1])));
    }

    return centers;
}

cv::Point2f convertToMM(const cv::Point& p, const vector<cv::Point>& corners) {
    float w_mm = 2448.0f, h_mm = 1224.0f;
    float w_px = cv::norm(corners[0] - corners[1]);
    float h_px = cv::norm(corners[0] - corners[3]);
    float dx = p.x - corners[0].x;
    float dy = p.y - corners[0].y;
    return { dx * (w_mm / w_px), dy * (h_mm / h_px) };
}

