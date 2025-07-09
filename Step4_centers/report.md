# Step 4 : Detct Balls using HSV + Hough Circle Transform

## 목적
 - 각 공(빨간 공, 흰 공, 노란 공)의 중심 좌표를 검출
 - HSV 마스크와 Hough Circle 알고리즘을 조합하여 공을 검출
 - 검출된 픽셀 좌표를 실제 당구대(mm) 기준 좌표로 변환
 - 검출 결과를 이미지에 시각화하고 저장

## 사용 개념
 - cv::inRange() : +HSV 범위 : 색상별 공 분리
 - cv::bitwise_and() : 마스크된 공만 분리
 - cv::HoughCircles() : 원 형태의 객체 검출
 - cv::moments() : 사용하지 않음(Hough 기반으로 대체)
 - cv::circle(), cv::putText() : 중심 시각화
 - converToMM() : 픽셀 좌표 -> mm 단위로 변환

## TroubleShooting
| 문제                      | 해결 방법                                                                           |
| ----------------------- | ------------------------------------------------------------------------------- |
| 빨간 공 인식이 되지 않음          | HSV에서 빨간색은 Hue 0~~10과 170~~180의 **두 구간**에 걸쳐 있으므로 `cv::bitwise_or()`로 두 마스크를 결합 |
| 흰공 주변 반사(spot)도 공처럼 인식됨 | HSV의 S/V 범위를 좁게 조정하여 고채도/고명도만 추출                                                |
| 공이 검출되지만 중심이 어긋남        | `HoughCircles()`의 min/max radius, threshold 파라미터 튜닝                             |
| 다른 물체를 공으로 잘못 인식        | 마스크 + Hough 병행으로 false positive 줄임                                              |
| 빨간 공 2개 중 1개만 인식        | Hue 범위 보완 + Hough 탐지 민감도 수정 후 해결됨                                               |


## 에러

![Alt text](/Step4_centers/images/step4_result_error1.jpg)

 - 적구 인식 실패(한개는 인식 나머지는 인식 못함)
 - 흰공 인식 하지만, 당구대 테이블에 반사된 빛과 포인트(점들)에 대해 흰공으로 인식함
 - 노란공 인식 정확함

