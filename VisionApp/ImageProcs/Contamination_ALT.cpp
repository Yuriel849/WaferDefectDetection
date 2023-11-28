#include "Common.h"

void main()
{
    {
        std::string filePath_Ptrn = "./res/img/wafer_template.png"; // Target image
        cv::Mat src_gray_ptrn = cv::imread(filePath_Ptrn, cv::ImreadModes::IMREAD_GRAYSCALE); // Load target image in grayscale
        Mat draw_Pads;
        cvtColor(src_gray_ptrn, draw_Pads, COLOR_GRAY2BGR); // Convert grayscale to color to draw on

        Mat Pads_Region;
        double obj_PAD_threshold = 130;
        cv::threshold(src_gray_ptrn, Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        cv::findContours(Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

        vector<cv::Rect> vRois_Small, vRois_Large;
        for (size_t k = 0; k < contours.size(); k++)
        {
            RotatedRect rrt = minAreaRect(contours[k]); // Minimum area rectangle encompassing the contour area
            Rect rt = rrt.boundingRect(); // Straight rectangle encompassing the minimum area rectangle
            // Create a rectangle then extract specific lengths
            if (13 <= rt.width && rt.width <= 17) // Width of small squares at the bottom of the target image
            {

                vRois_Small.push_back(rt);
                cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 255, 0), 1);
            }
            if (18 <= rt.width && rt.width <= 22) // Width of large squares at the top of the target image
            {

                vRois_Large.push_back(rt);
                cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 0, 255), 1);
            }
        }

        std::string filePath_Search = "./res/img/edge_location_black.png"; // Image to search

        cv::Mat src_draw = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_ANYCOLOR); // Load image in color
        cv::Mat src_gray_search = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE); // Load image in grayscale

        // Find target images in the image to search
        vector<cv::Rect> vRois;
        Mat obj_Region;
        Point ptThrehold = Point(888, 150); // Use the background color (green) as the threshold value for binarization
        double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5; // The color value at the point + 5
        cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);
        contours.clear(); // Clear vector to reuse
        hierarchy.clear(); // Clear vector to reuse
        cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); // Find contours of objects (target images)
        for (size_t k = 0; k < contours.size(); k++)
        {
            double area = contourArea(contours[k]);
            RotatedRect rrt = minAreaRect(contours[k]);
            Rect rt = rrt.boundingRect();

            if (160 <= rt.width && rt.width <= 180)
            {3.

                vRois.push_back(rt);
                cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1);
            }
        }

        // Sub chips
        vector<cv::Rect> vRois_Small_Err, vRois_Large_Err; // Vectors to store located small and large ROIs
        for (size_t k = 0; k < vRois.size(); k++)
        {
            Mat sub_img = src_gray_search(vRois[k]).clone();
            Mat sub_img_draw;
            cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR); //
            // Large pad
            int inflate = 7; // 객체 rect 좌우 위치 오차를 줄이기 위해 7만큼 옮기기 위한 변수 설정
            for (size_t i = 0; i < vRois_Large.size(); i++) // Repeat as many times as the large objects exist
            {
                Rect rt = vRois_Large[i]; // 세부 라지객체 i번째 rt에 담음
                rt.x -= inflate; // 세부 라지객체 i번째의 x위치에서 7만큼 빼서 좌로 이동
                rt.width += inflate * 2; // 가로사이즈 디폴트 5를 2배로 늘려 윤곽을 잡아준다.
                rt.height += inflate * 2;//// 세로사이즈 디폴트 5를 2배로 늘려 윤곽을 잡아준다.

                cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //sub_img_draw에 rt객체의 정보만큼 사각형 그려줌

                Mat Pad = sub_img(rt).clone(); // ((세부 객체 사각형 정보를 따서 Pad에 복사))
                Mat Pad_Bin;
                cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY); //이진화

                contours.clear(); //clear를 써서 새롬게 초기화
                hierarchy.clear(); //clear를 써서 새롬게 초기화
                cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Pad_Bin의 각 객체 외곽선 검출
                for (size_t p = 0; p < contours.size(); p++) //Pad_Bin의 각 객체 수만큼 for문을 돌린다. 
                {
                    double area = contourArea(contours[p]); //검출된 객체 전체 면적 값 area에 저장
                    if (area < 10) continue; //area가 10을 넘지 않으면 종료
                    RotatedRect rrt = minAreaRect(contours[p]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 rrt에 담아줌
                    Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성하여 rt에 담아줌

                    if (18 <= rt.width && rt.width <= 22)//large 사이즈 일정 조건 길이 추출  //rt 검출된 객체 길이
                    {

                    }
                    else
                    {
                        Rect rtSubErrRgn = rt; //조건에  충족하지 않은 rt값 rtSubErrRgn에 저장
                        rtSubErrRgn.x += vRois_Large[i].x - inflate; // x점 위치에 -7만큼 위치 조정
                        rtSubErrRgn.y += vRois_Large[i].y; //y점 위치는 그대로
                        cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //위치값 따서 사각형 그려줌

                        Rect rtDraw = rt;
                        rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate;
                        rtDraw.y += vRois_Large[i].y + vRois[k].y;
                        vRois_Large_Err.push_back(rtDraw);
                        cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
                    }
                }
            }
            //small pad   
            for (size_t i = 0; i < vRois_Small.size(); i++)
            {
                Rect rt = vRois_Small[i];
                rt.x -= inflate;
                rt.y -= inflate-3;
                rt.width += inflate * 2;
                rt.height += inflate + 2;

                cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과

                Mat Pad = sub_img(rt).clone();
                Mat Pad_Bin;
                cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

                contours.clear(); //새롭게 범위값을 구하기 위해 백터 포인트 변수 contourss 생성 
                hierarchy.clear(); //백터 hierarchyy변수 생성
                cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
                for (size_t p = 0; p < contours.size(); p++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
                {
                    double area = contourArea(contours[p]);
                    if (area < 10) continue;
                    RotatedRect rrt = minAreaRect(contours[p]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
                    Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

                    if (13 <= rt.width && rt.width <= 17)//small
                    {
                    }
                    else {
                        Rect rtSubErrRgn = rt;
                        rtSubErrRgn.x += vRois_Small[i].x - inflate;
                        rtSubErrRgn.y += vRois_Small[i].y;
                        cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과


                        cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
                        Rect rtDraw = rt;//chips
                        rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate;
                        rtDraw.y += vRois_Small[i].y + vRois[k].y;
                        vRois_Small_Err.push_back(rtDraw);
                        cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
                    }
                }

            }
        }



        //display result :: error rectangle
        for (size_t i = 0; i < vRois_Large_Err.size(); i++)
        {
            cv::rectangle(src_draw, vRois_Large_Err[i], CV_RGB(255, 0, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
        }
        for (size_t i = 0; i < vRois_Small_Err.size(); i++)
        {
            cv::rectangle(src_draw, vRois_Small_Err[i], CV_RGB(255, 0, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
        }
        int a = 0;
    }
    return;
}