#include "InspectContamination.h"


InspectContamination::InspectContamination() 
{
    _InspectName = "Contamination";
}

InspectContamination::~InspectContamination() {}

int InspectContamination::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num)
{



    std::vector<cv::Rect> vRois_Large, vRois_Small;
    vRois_Large = GetLargePadInChipRegions();
    vRois_Small = GetSmallPadInChipRegions();

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    //sub chips

    //vector<int> remember;
    for (size_t k = 0; k < vChips.size(); k++) // 전체 이미지 객체vRois.size()의 개수만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
    {
        Mat sub_img = src(vChips[k]).clone(); // vRois[k]번째 객체 sub_img에 삽입 
        Mat sub_img_draw;
        cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR);

        //int cnt_Large = 0, cnt_Small = 0, cnt_Large_Err = 0, cnt_Small_Err = 0;

        //large pad
        int inflate = 7; //객체 rect 좌우 위치 오차를 줄이기 위해 7만큼 옮기기 위한 변수 설정
        for (size_t i = 0; i < vRois_Large.size(); i++) // 세부 라지 객체 사이즈만큼 반복
        {
            Rect rt = vRois_Large[i]; // 세부 라지객체 i번째 rt에 담음
            rt.x -= inflate; // 세부 라지객체 i번째의 x위치에서 7만큼 빼서 좌로 이동 (스몰객체 x 좌표에 -7만큼 왼쪽으로 x축 이동)
            rt.width += inflate * 2; // 가로사이즈 디폴트 5를 2배로 늘려 윤곽을 잡아준다.//가로 길이 7*2=14만큼 rt.width길이 설정하여 범위 설정
            rt.height += inflate + 4;// 세로사이즈 디폴트 5를 2배로 늘려 윤곽을 잡아준다.rt.height += inflate * 2(스크래치까지 잡아줘서  + 4로 크기 조정
            //     
            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //sub_img_draw에 rt객체의 정보만큼 사각형 그려줌

            Mat Pad = sub_img(rt).clone(); // ((세부 객체 사각형 정보를 따서 Pad에 복사))
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY); //이진화

            //morpology 노이즈 제거 부분
            int kernelSz = 2; //노이즈 크기
            int shape = MorphShapes::MORPH_RECT; //
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //정lSz + 1) 
            Mat SE = cv::getStructuringElement(shape, sz); // 
            Mat Pad_Open; //노이즈가 제거된 상태.
            int type = MorphTypes::MORPH_OPEN; // 노이즈를 제거하는 기능 MORPH_OPEN을 type에 담겠다.
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);// cv::morphologyEx(src_bin(입력), src_open(출력), type, SE);//morphologyEx 노이즈를 제거하겠다. 

            contours.clear(); //clear를 써서 새롬게 초기화
            hierarchy.clear(); //clear를 써서 새롬게 초기화
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Pad_Bin의 각 객체 외곽선 검출
            for (size_t p = 0; p < contours.size(); p++) //Pad_Bin의 각 객체 수만큼 for문을 돌린다. 
            {
                double area = contourArea(contours[p]); //검출된 객체 전체 면적 값 area에 저장
                if (area < 10) continue; //area가 10을 넘지 않으면 종료
                RotatedRect rrt = minAreaRect(contours[p]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 rrt에 담아줌
                Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성하여 rt에 담아줌

                //large 사이즈 일정 조건 길이 추출  //rt 검출된 객체 길이 18 <= rt.width && rt.width <= 22
                if (((avg_Width_Large * 0.9) <= rt.width && rt.width <= (avg_Width_Large * 1.1))
                    && ((avg_Height_Large * 0.9) <= rt.height && rt.height <= (avg_Height_Large * 1.1)))
                {
                    //cnt_Large++;
                }
                else
                {
                    //vErrRegions.push_back(vChips[k]);
                    //cnt_Large_Err++;
                    Rect rtSubErrRgn = rt; //조건에  충족하지 않은 rt값 rtSubErrRgn에 저장
                    rtSubErrRgn.x += vRois_Large[i].x - inflate; // x점 위치에 -7만큼 위치 조정
                    rtSubErrRgn.y += vRois_Large[i].y; //y점 위치는 그대로
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //위치값 따서 사각형 그려줌

                    Rect rtDraw = rt;
                    rtDraw.x += vRois_Large[i].x + vChips[k].x - inflate; //rtDraw.x에 vRois[k].x전체 크기 중간 객체 추출 좌표를 더하고 세부객체 x좌표 vRois_Small[i].x를 담아 투과적으로 해당 세부객체 x좌표를 직접적으로 가리키게 됨.
                    rtDraw.y += vRois_Large[i].y + vChips[k].y; //rtDraw.y에 vRois[k].y전체 크기 중간 객체 추출 좌표를 더하고 세부객체 y좌표 vRois_Small[i].y를 담아 투과적으로 해당 세부객체 y좌표를 직접적으로 가리키게 됨.

                    DefectInfo defectInfo;
                    defectInfo.rt = rtDraw;
                    defectInfo.Judge = eJudge_Contamination;
                    vRois_Large_Err.push_back(defectInfo); // 직접적으로 가리킨 좌표 
                    if (vErrRegions[k].Judge == eJudge_OK)
                    {
                        vErrRegions[k] = defectInfo;
                        //cv::rectangle(drawColor, vChips[k], CV_RGB(0, 255, 125), FILLED); //
                        cv::rectangle(drawColor, rtDraw, CV_RGB(0, 255, 125), FILLED); //
                    }
                    //cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //
                    
                    //색칠할 칸 기억하기 
                    //remember.push_back(k);
                }
                int checking2 = 0;
            }
            int checking3 = 0;
        }
        /*if (cnt_Large == 4 && cnt_Large_Err != 0)
        {
            vRois_Large_Err.resize(vRois_Large_Err.size() - cnt_Large_Err);
        }*/
        //small pad   
        for (size_t i = 0; i < vRois_Small.size(); i++) //small pads 하나씩 보면서 이진화, 오류 확인
        {
            Rect rt = vRois_Small[i];
            rt.x -= inflate;
            rt.y -= inflate - 4;
            rt.width += inflate * 2;
            rt.height += inflate + 2;

            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 2);

            Mat Pad = sub_img(rt).clone();
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

            //morpology 노이즈 제거 부분
            int kernelSz = 2; //노이즈 크기
            int shape = MorphShapes::MORPH_RECT; //
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //정lSz + 1) 
            Mat SE = cv::getStructuringElement(shape, sz); // 
            Mat Pad_Open; //노이즈가 제거된 상태.
            int type = MorphTypes::MORPH_OPEN; // 노이즈를 제거하는 기능 MORPH_OPEN을 type에 담겠다.
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);// cv::morphologyEx(src_bin(입력), src_open(출력), type, SE);//morphologyEx 노이즈를 제거하겠다. 

            contours.clear(); //
            hierarchy.clear(); //
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
            for (size_t p = 0; p < contours.size(); p++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
            {
                double area = contourArea(contours[p]);
                if (area < 10) continue;
                RotatedRect rrt = minAreaRect(contours[p]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 rrt에 담아줌
                Rect rt = rrt.boundingRect(); // rt 객체의 위치에 상관없이 정방향 사각형 생성

                if (((avg_Width_Small * 0.8) <= rt.width && rt.width <= (avg_Width_Small * 1.2))
                    && ((avg_Height_Small * 0.8) <= rt.height && rt.height <= (avg_Height_Small * 1.2)))
                {
                    //cnt_Small++;
                }
                else
                {
                    //vRegions->push_back(vRois[k]);
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //

                    Rect rtSubErrRgn = rt;
                    rtSubErrRgn.x += vRois_Small[i].x - inflate;
                    rtSubErrRgn.y += vRois_Small[i].y;
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 123), 2); //

                    // cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //
                    Rect rtDraw = rt;//chips
                    rtDraw.x += vRois_Small[i].x + vChips[k].x - inflate; //rtDraw.x에 vRois[k].x전체 크기 중간 객체 추출 좌표를 더하고 세부객체 x좌표 vRois_Small[i].x를 담아 투과적으로 해당 세부객체 x좌표를 직접적으로 가리키게 됨.
                    rtDraw.y += vRois_Small[i].y + vChips[k].y;

                    DefectInfo defectInfo;
                    defectInfo.rt = rtDraw;
                    defectInfo.Judge = eJudge_Contamination;
                    vRois_Small_Err.push_back(defectInfo);
                    if (vErrRegions[k].Judge == eJudge_OK)
                    {
                        vErrRegions[k] = defectInfo;
                        //cv::rectangle(drawColor, vChips[k], CV_RGB(0, 255, 125), FILLED); //

                        
                        cv::rectangle(drawColor, rtDraw, CV_RGB(0, 255, 125), FILLED); //
                    }
                    
                    //vRois_Small_Err.push_back(rtDraw);


                    //색칠할 칸 기억하기 
                    //remember.push_back(k);
                }
            }
        }


    }

    //display result :: error rectangle
    //for (size_t i = 0; i < vRois_Small_Err.size(); i++)
    //{
    //    string msg;
    //    msg = to_string(con_num + 1);
    //    // 사각형 내부의 픽셀 색상 확인
    //    cv::Scalar currentPixelColor = drawColor.at<cv::Vec3b>(remeber_con[i].y + remeber_con[i].height / 2,
    //        remeber_con[i].x + remeber_con[i].width / 2);
    //    if (currentPixelColor == CV_RGB(255, 0, 0) || currentPixelColor == CV_RGB(255, 255, 0))
    //    {
    //        // int check112 = 0;
    //    }
    //    else
    //    {
    //        Scalar color = CV_RGB(255, 0, 255);
    //        con_num += 1;
    //        cv::putText(drawColor, msg, remeber_con[i].br(), FONT_HERSHEY_SIMPLEX, 1, color, 2, 8);		//0, 255, 255
    //        cv::putText(drawColor, contamination, remeber_con[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
    //        cv::rectangle(drawColor, remeber_con[i], color, CV_FILLED);
    //        flaw_num += 1;      // 결함 개수 증가 
    //    }
    //}
    //for (size_t i = 0; i < vRois_Large_Err.size(); i++)
    //{
    //    cv::rectangle(drawColor, vRois_Large_Err[i], CV_RGB(255, 0, 255), 2); //
    //}
    //for (size_t i = 0; i < vRois_Small_Err.size(); i++)
    //{
    //    cv::rectangle(drawColor, vRois_Small_Err[i], CV_RGB(255, 0, 255), 2); //
    //}
    return 0;
}