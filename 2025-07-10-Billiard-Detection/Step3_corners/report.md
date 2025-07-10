# Step 3: Detect Table Corners using Contours

## 목적
Step 2에서 생성한 마스크 이미지를 기반으로,
당구대 외곽 윤곽선(contour)을 검출하고, 그 윤곽선으로부터
볼록 사각형(convex hull) 형태의 꼭짓점 4개를 추출하여
실제 테이블 좌표 변환 등에 활용한다

## 사용 개념
 - cv::findContours() : 외곽선 검출
 - cv::approxPolyDP() : 윤곽선을 꼭짓점으로 근사화
 - cv::convexHull() : 꼭짓점을 감싸는 볼록 다각형 계산
 - cv::drawContours(), cv::circle(), cv::putText() : 검출된 꼭짓점 시각화

 ## 핵심 원리
 - 당구대 마스크 이미지에서 가장 큰 윤곽선(외곽 테두리)을 선택하고, 이를 approxPolyDP로 단순화하여 꼭짓점 수를 줄인다.
 - 그 후 ConvexHull을 사용항 비틀리거나 휘어있는 형태를 보정하고, 항상 볼록한 4꼭짓점 사각형으로 만든다.
 - 이 꼭짓점은 추후 픽셀 <-> 실세계(mm) 좌표 변환 기준이 된다.

## TroubleShooting
| 문제                         | 해결 방법                                              |
| -------------------------- | -------------------------------------------------- |
| 검출된 윤곽선이 너무 울퉁불퉁하고 꼭짓점이 많음 | `approxPolyDP()`로 윤곽선을 단순화 (epsilon 값 조절: 20픽셀)    |
| 4개의 꼭짓점이 아닌 경우도 발생         | `convexHull()`을 통해 항상 볼록 사각형으로 정제                  |
| 테이블 바깥 그림자, 벽 등이 포함됨       | Step 2에서 **HSV 마스크를 최적화(초록 계열)** 하여 윤곽선 입력 자체를 정제함 |
| 꼭짓점 순서가 랜덤임                | 현재는 순서를 정렬하지 않음 (향후 시계방향 정렬 로직 추가 가능)              |


![Alt text](/Step3_corners/images/step3_corners_blue.jpg)
![Alt text](/Step3_corners/images/step3_corners_green.jpg)