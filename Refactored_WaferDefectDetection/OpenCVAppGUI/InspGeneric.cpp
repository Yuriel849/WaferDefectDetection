#include "InspGeneric.h"

InspGeneric::InspGeneric()
{
}

InspGeneric::~InspGeneric()
{
}

int InspGeneric::OnPreprocess(const Mat& src)
{
    vLargePadsInChip.clear();
    vSmallPadsInChip.clear();
    vChips.clear();

    vector<Rect> remeber_con;

    Mat ptrn = cv::imread(PATTERN, cv::ImreadModes::IMREAD_GRAYSCALE);

    double obj_PAD_threshold = 130;
    Mat ptrn_Pads_Region;    
    cv::threshold(ptrn, ptrn_Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    cv::findContours(ptrn_Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    
    for (size_t k = 0; k < contours.size(); k++)
    {
        RotatedRect rrt = minAreaRect(contours[k]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
        Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성
        //위 정사각형 생성 이후 특정 길이 추출
        if (13 <= rt.width && rt.width <= 17)//객체 각 small 사이즈 rt길이 13이상 17이하인 경우
        {

            vSmallPadsInChip.push_back(rt); //각 세부Small 객체 출력
            //cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //rrt.boundingRect()위에서 구한 사각형 값에 사각형을 그려줌
        }
        if (18 <= rt.width && rt.width <= 22)//객체 각 small 사이즈 rt길이 18이상 22이하인 경우
        {

            vLargePadsInChip.push_back(rt); //각 Large세부 객체 출력
            //cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //rrt.boundingRect()위에서 구한 사각형 값에 사각형을 그려줌
        }
    }

    // Identify the large and small squares in each chip across the wafer under inspection
    Mat obj_Region;
    Point ptThrehold = Point(888, 150); //이진화된 배경 색보다 밝을 경우 차이를 주기 위해 배경 위치 값 포인터로 설정 
    double min_threshold = src.data[ptThrehold.y * src.cols + ptThrehold.x] + 5; //배경색보다 밝을 부분 설정하여
    cv::threshold(src, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region에 이진화 작업
    contours.clear(); //새롭게 범위값을 구하기 위해 백터 포인트 변수 contourss 생성
    hierarchy.clear(); //백터 hierarchyy변수 생성
    cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
    for (size_t k = 0; k < contours.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
    {
        double area = contourArea(contours[k]);

        RotatedRect rrt = minAreaRect(contours[k]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
        Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

        if (160 <= rt.width && rt.width <= 200)
        {
            vChips.push_back(rt);
            //cv::rectangle(drawColor, rrt.boundingRect(), CV_RGB(0, 255, 255), 2); //
        }
    }





    // Calculate average widths and heights of the large and small squares in each chip
    int sum_Width_Large = 0, sum_Height_Large = 0, sum_Width_Small = 0, sum_Height_Small = 0;
    for (size_t k = 0; k < vSmallPadsInChip.size(); k++)
    {
        sum_Width_Small += vSmallPadsInChip[k].width;
        sum_Height_Small += vSmallPadsInChip[k].height;
    }
    avg_Width_Small = static_cast<float>(sum_Width_Small) / vSmallPadsInChip.size();
    avg_Height_Small = static_cast<float>(sum_Height_Small) / vSmallPadsInChip.size();

    for (size_t k = 0; k < vLargePadsInChip.size(); k++)
    {
        sum_Width_Large += vLargePadsInChip[k].width;
        sum_Height_Large += vLargePadsInChip[k].height;
    }
    avg_Width_Large = static_cast<float>(sum_Width_Large) / vLargePadsInChip.size();
    avg_Height_Large = static_cast<float>(sum_Height_Large) / vLargePadsInChip.size();
    return 0;
}

int InspGeneric::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num)
{
    return 0;
}

int InspGeneric::OnPostprocess(const Mat& src)
{
    string msg1 = "Yield = ";
    string msg2;
    string msg3 = "%";
    size_t allChips = vChips.size();
    size_t flaw_num = vErrRegions.size();
    int yield = ((allChips - flaw_num) / allChips) * 100;
    cout << allChips - flaw_num << endl;
    cout << (allChips - flaw_num) / allChips << endl;
    msg2 = to_string(yield);

    putText(drawImg, msg1 + msg2 + msg3, cv::Point(50, 1625), FONT_HERSHEY_SIMPLEX, 2.5, Scalar(0, 0, 0), 3);

    return 0;
}

