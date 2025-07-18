# Step 2: Mask Table Area and Remove Noise

## 목적(Goal)
HSV 색공간 기반으로 파란색 당구대 영역만 마스크로 추출하고,
노이즈 제거를 통해 윤곽선 검출에 적합한 이진 마스크를 생성한다.
이 결과는 Step 3에서 꼭짓점을 검출할 때 사용된다.

## 사용 개념
 - cv:inRange() : HSV 색상 범위 기반으로 마스크 생성
 - cv::erode() + cv::dilate() : 노이즈 제거를 위한 형태학적 연산
 - cv::imwrite() : 마스크 이미지 저장

## 원리
 - OpenCV의 HSV에서 파란색은 대략 H=110~130 범위에 위치함
 - S(채도)와 V(명도)는 너무 낮거나 너무 높지도 않도록 범위 설정(100~255)
 - 형태학 연산을 통해 공의 반사나 주변 잡음을 제거

## TroubleShooting
| 문제                                               | 해결 방법                                                            |
| ------------------------------------------------ | ---------------------------------------------------------------- |
| 초기에 테이블을 초록색으로 잘못 인식하여 HSV 범위를 `H=115~123`으로 설정함 | HSV를 직접 클릭해보니 실제로는 파란색 계열(H=115 근처), 즉 OpenCV 기준 **파랑 영역**임을 확인함 |
| 마스크 결과가 너무 거칠거나 끊어짐                              | 침식 → 팽창(Opening)으로 노이즈 제거하고 큰 외곽 테두리 유지                          |
| 일부 공이나 반사된 조명도 마스크에 포함됨                          | S와 V 범위를 적절히 조정하여 밝은 반사 제거 효과                                    |


### 초록색 vs 파란색 HSV 마스킹 비교 분석

![Alt text](/Step2_table_mask/images/step2_table_mask_green.jpg)
![Alt text](/Step2_table_mask/images/step2_table_mask_blue.jpg)

| 항목                           | 초록색 HSV 설정              | 파란색 HSV 설정               |
| ---------------------------- | ----------------------- | ------------------------ |
| **Step 2 마스크 결과**            | 쿠션 영역까지 잘 포함됨           | 쿠션 영역 일부 누락, 테이블 중앙만 추출됨 |
| **Step 3 윤곽선 추출 결과**         | 깔끔하고 직선에 가까운 사각형 윤곽     | 울퉁불퉁하고 왜곡된 형태            |
| **Step 3의 hull/contour 정확도** | 매우 정확함 (사각형 형태 유지)      | 가장자리 불명확, 윤곽선 왜곡 있음      |


 - 초록색 기준 HSV 마스크에서는 **테이블의 가장자리(쿠션 포함)**까지 넓게 인식됨
 - 파란색 기준 HSV 마스크에서는 **테이블 중앙만 인식**되고, 쿠션 부위가 제외됨
 - 윤곽선 검출도 초록색이 좀 더 깔끔한 사각형, 파란색은 울퉁불퉁하고 노이즈가 더 많이 섞임


| 원인 요소              | 설명                                                                                              |
| ------------------ | ----------------------------------------------------------------------------------------------- |
| **테이블 표면의 색상 편차**  | 실제 테이블이 "파란색"처럼 보이지만, 사진 조명/조도/반사에 따라 **HSV상 Hue가 초록 근처(115\~123)에 있음**                         |
| **카메라 화이트 밸런스 영향** | 실내 조명 환경에서 **카메라 자동 보정**이 들어가 색상이 실제보다 "녹색 쪽"으로 보정될 수 있음                                        |
| **HSV 마스크 범위 적합도** | 초록색 설정(H=115~~123)은 실제 당구대 전체를 넓게 포괄할 수 있는 반면, 파란색 설정(H=110~~130)은 **중앙 외 영역을 포함하지 못하는 경우가 많음** |
| **쿠션의 색상 차이**      | 중앙 테이블과 쿠션의 색이 조금 다를 수 있음. 초록색 마스크가 둘 다 포함되지만, 파란색 마스크는 쿠션을 누락했을 가능성                            |


### 결론 
HSV상 초록색 범위(H=115~123)를 사용하는 것이 더 나은 선택임
 - 쿠션 포함 테이블 전체 마스크 생성 가능
 - 더 깔끔한 윤곽선 -> 정확한 4꼭짓점 추출 가능