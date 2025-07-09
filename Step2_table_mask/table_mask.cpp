// Step 2: 파란 테이블 색상 마스크 생성
void maskTableArea() {
    // HSV 기준 파란색 범위 설정
    cv::Scalar lowerBlue(110, 100, 100);
    cv::Scalar upperBlue(130, 255, 255);

    // 마스크 생성
    cv::Mat mask;
    cv::inRange(hsvImage, lowerBlue, upperBlue, mask);

    // 노이즈 제거 (침식 + 팽창)
    cv::erode(mask, mask, {}, { -1, -1 }, 2);
    cv::dilate(mask, mask, {}, { -1, -1 }, 2);

    // 결과 저장
    cv::imwrite("step2_table_mask.jpg", mask);
    cout << "[Step 2] Saved table mask to step2_table_mask.jpg" << endl;
}
