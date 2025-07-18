# OpenCV 기반 당구대 및 공 인식 단계별 처리 기법 및 개념 분석

## 1. 서론

당구 훈련 시스템에서 비전 기반 분석은 정확한 테이블 경계 및 공의 위치를 검출하는 것이 핵심이다. 본 문서에서는 OpenCV 기반의 객체 검출 파이프라인을 구현하고, 이를 통해 당구대와 공의 위치를 정밀하게 추출하는 과정을 네 가지 단계로 나누어 기술하였다. 이 구조는 논문 "Augmented Reality-based Billiards Training System"의 테이블 및 공 인식 과정을 참고하여 실험적 방식으로 구현되었으며, 단순 색상 분리 이상의 정보 추출을 위해 HSV 색공간, 형태학적 연산, 윤곽선 기반 경계 검출, 원 검출 기법(Hough Transform) 등을 통합하였다.

## 2. 단계별 처리 개요

### Step 1: 이미지 불러오기 및 HSV 변환

![Alt text](/images/step1_original.jpg)

컬러 이미지의 RGB(BGR) 정보는 조명 변화에 민감하므로, 색상 기반 필터링을 위해 HSV(Hue, Saturation, Value) 색공간으로 변환한다. HSV에서 H(Hue)는 색상 정보를 분리해내는 데 가장 효과적이다. OpenCV에서의 Hue는 0\~180 범위로 표현된다.

![Alt text](/images/step1_hsv.jpg)

* 수식: RGB → HSV 변환은 비선형 변환이지만, OpenCV는 `cv::cvtColor(img, hsv, COLOR_BGR2HSV)` 함수로 내부 처리
* 주요 개념: 색상은 Hue로, 밝기와 조도는 Saturation, Value로 분리됨
* 목적: 공 또는 당구대와 같이 색상이 명확한 객체는 HSV로 필터링하는 것이 효과적

### Step 2: 테이블 색상 마스킹 및 노이즈 제거

![Alt text](/images/step2_table_mask_green.jpg)

테이블은 표준 당구대 기준으로 파란색(또는 초록색)으로 칠해져 있으며, 일정 범위의 Hue 값으로 분리 가능하다. 이를 위해 `cv::inRange()`를 사용하여 이진 마스크를 생성하고, `cv::erode`, `cv::dilate`로 노이즈 제거를 수행한다. 이 과정을 통해 테이블 외부 영역(벽, 그림자 등)을 효과적으로 제거할 수 있다.

* 수식:

  * Masking: $M(x, y) = \begin{cases} 1 & \text{if } H_{low} \leq H(x, y) \leq H_{high} \\ 0 & \text{otherwise} \end{cases}$
  * Morphological Opening: $M' = D(E(M))$, where $E$ = erosion, $D$ = dilation
* 시행착오: 테이블 색상이 파란색으로 보였으나 HSV상에서는 초록색에 가까워 초록색 Hue 범위가 더 안정적으로 작동함을 확인

### Step 3: 테이블 외곽 윤곽선 및 꼭짓점 검출

![Alt text](/images/step3_corners_green.jpg)

노이즈가 제거된 마스크에서 외곽선을 검출하기 위해 `cv::findContours()`를 사용하고, 가장 큰 윤곽선(contour)을 선택하여 테이블 외곽으로 가정한다. 이후 `cv::approxPolyDP()`로 윤곽선을 꼭짓점 형태로 근사하고, `cv::convexHull()`로 외곽 다각형을 정제한다.

* 수식:

  * 면적 기준 선택: $A = \max_i \{ \text{Area}(C_i) \}$
  * 꼭짓점 근사: Ramer-Douglas-Peucker 알고리즘
* 목적: 이후 mm 단위로 좌표 변환을 하기 위해 4개의 정확한 꼭짓점이 필요함
* 개념: Convex Hull은 임의의 점 집합에 대해 가장 바깥을 감싸는 볼록 다각형을 생성하는 연산이며, 기하학적 정규화를 위해 유용

### Step 4: 당구공 중심 검출 및 mm 좌표 변환

![Alt text](/images/step4_result_blue.jpg)

공은 일반적으로 색상이 뚜렷하고 구형이며, 이를 기반으로 HSV 마스크로 영역을 추출하고 Hough Circle Transform으로 원을 검출한다. 특히 빨간 공은 HSV상에서 두 개의 Hue 범위(0~~10, 170~~180)를 OR 연산으로 결합하여 처리하였다. 검출된 중심점은 Step 3에서 얻은 4 꼭짓점을 기준으로 실측 크기(2448mm x 1224mm)에 맞춰 좌표를 mm 단위로 변환한다.

* 수식:

  * Hough Circle Transform: $(x, y, r) \in \text{Circle}(I), \nabla^2 I + I = 0$
  * 픽셀 → mm 변환:
    $x_{mm} = (x - x_0) \times \frac{W_{mm}}{W_{px}}, \quad y_{mm} = (y - y_0) \times \frac{H_{mm}}{H_{px}}$
* 시행착오:

  * 흰 공 주변 반사가 공처럼 인식됨 → 채도(S), 명도(V) 범위 조절
  * 빨간 공은 Hue가 양 끝에 존재해 이중 마스크 필요

---

## 3. 결론 및 향후 발전 방향

![Alt text](/images/step4_result_final.jpg)

HSV 색공간 기반 마스킹과 윤곽선 분석, Hough Transform을 단계적으로 결합함으로써 당구대 및 공의 위치를 안정적으로 검출할 수 있었다. 특히 공 검출의 경우 HSV 범위 필터링만으로는 반사광 등의 오차가 컸으나, 원형 검출을 추가함으로써 인식률을 크게 향상시킬 수 있었다.

향후에는 다음과 같은 방향으로 시스템을 확장할 수 있다:

* 검출된 공의 좌표를 활용한 궤적 예측 및 충돌 분석
* 시계방향 기준으로 테이블 꼭짓점 자동 정렬
* 실시간 영상 스트림에서 프레임 단위 인식 구현

본 구현은 실험 중심적 구성으로 진행되었으며, 논문에서 제안된 원리를 실용적으로 재구성한 사례로써 학습 및 시제품 제작 관점에서도 충분히 활용 가능하다.
