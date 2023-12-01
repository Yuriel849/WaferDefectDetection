#include "InspectContaminationRe.h"


InspectContaminationRe::InspectContaminationRe() {}

InspectContaminationRe::~InspectContaminationRe() {}

int InspectContaminationRe::GetObjectRegionsTemplatMatching(const Mat& src, const Mat& ptrn, std::vector<cv::Rect>& vRegions) { return 0; }

int InspectContaminationRe::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions)
{
    cv::Mat src_gray_search = src;
    cv::Mat src_draw = drawColor;

    cv::Mat src_gray_ptrn = cv::imread(PATTERN, cv::ImreadModes::IMREAD_GRAYSCALE); //?��??��?지 ?�백?�로 로딩
    Mat draw_Pads;
    cvtColor(src_gray_ptrn, draw_Pads, COLOR_GRAY2BGR); //draw_Pads
    Mat Pads_Region;
    double obj_PAD_threshold = 130;
    cv::threshold(src_gray_ptrn, Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);
    vector<vector<Point>> contours; //?�롭�?범위값을 구하�??�해 백터 ?�인??변??contourss ?�성
    vector<Vec4i> hierarchy; //백터 hierarchyy변???�성
    cv::findContours(Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하�??�해 객체 ?�곽??검�?

    vector<cv::Rect> vRois_Small, vRois_Large;
    for (size_t k = 0; k < contours.size(); k++)
    {
        RotatedRect rrt = minAreaRect(contours[k]);//객체 ?�곽?�을 ?��? ?�을 만큼 ???�이즈�? wRrt???�아�?
        Rect rt = rrt.boundingRect(); // 객체???�치???��??�이 ?�방???�각???�성
        //???�사각형 ?�성 ?�후 ?�정 길이 추출
        if (13 <= rt.width && rt.width <= 17)//객체 �?small ?�이�?rt길이 13?�상 17?�하??경우
        {
            vRois_Small.push_back(rt); //�??��?Small 객체 출력
            cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //rrt.boundingRect()?�에??구한 ?�각??값에 ?�각?�을 그려�?
        }
        if (18 <= rt.width && rt.width <= 22)//객체 �?small ?�이�?rt길이 18?�상 22?�하??경우
        {
            vRois_Large.push_back(rt); //�?Large?��? 객체 출력
            cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //rrt.boundingRect()?�에??구한 ?�각??값에 ?�각?�을 그려�?
        }
    }

    //find objects
    vector<cv::Rect> vRois; //?�체 ?��?지 백터 배열�??�??
    Mat obj_Region;
    Point ptThrehold = Point(888, 150); //?�진?�된 배경 ?�보??밝을 경우 차이�?주기 ?�해 배경 ?�치 �??�인?�로 ?�정 
    double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5; //배경?�보??밝을 부�??�정?�여
    cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region???�진???�업
    contours.clear(); //?�롭�?범위값을 구하�??�해 백터 ?�인??변??contourss ?�성
    hierarchy.clear(); //백터 hierarchyy변???�성
    cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하�??�해 객체 ?�곽??검�?
    for (size_t k = 0; k < contours.size(); k++) //contourss.size();???�기만큼 for문을 ?�린??(?�롭�??�의??객체 ?�이�??? 
    {
        double area = contourArea(contours[k]);
        RotatedRect rrt = minAreaRect(contours[k]);//객체 ?�곽?�을 ?��? ?�을 만큼 ???�이즈�? wRrt???�아�?
        Rect rt = rrt.boundingRect(); // 객체???�치???��??�이 ?�방???�각???�성

        if (160 <= rt.width && rt.width <= 200)
        {
            vRois.push_back(rt);
            cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //
        }
    }

    //sub chips
    vector<cv::Rect> vRois_Small_Err, vRois_Large_Err; // ?�몰 객체, ?��? 객체 검�??�?�을 ?�한 백터 ?�정
    for (size_t k = 0; k < vRois.size(); k++) // ?�체 ?��?지 객체vRois.size()??개수만큼 for문을 ?�린??(?�롭�??�의??객체 ?�이�??? 
    {
        Mat sub_img = src_gray_search(vRois[k]).clone(); // vRois[k]번째 객체 sub_img???�입 
        Mat sub_img_draw;
        cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR); //
        //large pad
        int inflate = 7; //객체 rect 좌우 ?�치 ?�차�?줄이�??�해 7만큼 ??���??�한 변???�정
        for (size_t i = 0; i < vRois_Large.size(); i++) // ?��? ?��? 객체 ?�이즈만??반복
        {
            Rect rt = vRois_Large[i]; // ?��? ?��?객체 i번째 rt???�음
            rt.x -= inflate; // ?��? ?��?객체 i번째??x?�치?�서 7만큼 빼서 좌로 ?�동
            rt.width += inflate * 2; // 가로사?�즈 ?�폴??5�?2배로 ?�려 ?�곽???�아준??
            rt.height += inflate + 4;//// ?�로?�이�??�폴??5�?2배로 ?�려 ?�곽???�아준??rt.height += inflate * 2

            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //sub_img_draw??rt객체???�보만큼 ?�각??그려�?

            Mat Pad = sub_img(rt).clone(); // ((?��? 객체 ?�각???�보�??�서 Pad??복사))
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY); //?�진??

            contours.clear(); //clear�??�서 ?�롬�?초기??
            hierarchy.clear(); //clear�??�서 ?�롬�?초기??
            cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Pad_Bin??�?객체 ?�곽??검�?
            for (size_t p = 0; p < contours.size(); p++) //Pad_Bin??�?객체 ?�만??for문을 ?�린?? 
            {
                double area = contourArea(contours[p]); //검출된 객체 ?�체 면적 �?area???�??
                if (area < 10) continue; //area가 10???��? ?�으�?종료
                RotatedRect rrt = minAreaRect(contours[p]);//객체 ?�곽?�을 ?��? ?�을 만큼 ???�이즈�? rrt???�아�?
                Rect rt = rrt.boundingRect(); // 객체???�치???��??�이 ?�방???�각???�성?�여 rt???�아�?

                if (18 <= rt.width && rt.width <= 22)//large ?�이�??�정 조건 길이 추출  //rt 검출된 객체 길이 18 <= rt.width && rt.width <= 22
                {

                }
                else
                {
                    Rect rtSubErrRgn = rt; //조건?? 충족?��? ?��? rt�?rtSubErrRgn???�??
                    rtSubErrRgn.x += vRois_Large[i].x - inflate; // x???�치??-7만큼 ?�치 조정
                    rtSubErrRgn.y += vRois_Large[i].y; //y???�치??그�?�?
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //?�치�??�서 ?�각??그려�?

                    Rect rtDraw = rt;
                    rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate;
                    rtDraw.y += vRois_Large[i].y + vRois[k].y;
                    vRois_Large_Err.push_back(rtDraw);
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn?�서 찾아??객체???�곽??부분의 wR2f.tl() 좌상 �???�과
                }
            }
        }
        //small pad   
        for (size_t i = 0; i < vRois_Small.size(); i++)
        {
            Rect rt = vRois_Small[i];
            rt.x -= inflate;
            rt.y -= inflate - 4;
            rt.width += inflate * 2;
            rt.height += inflate + 2;

            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 2); //src_openn?�서 찾아??객체???�곽??부분의 wR2f.tl() 좌상 �???�과

            Mat Pad = sub_img(rt).clone();
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

            contours.clear(); //?�롭�?범위값을 구하�??�해 백터 ?�인??변??contourss ?�성 
            hierarchy.clear(); //백터 hierarchyy변???�성
            cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하�??�해 객체 ?�곽??검�?
            for (size_t p = 0; p < contours.size(); p++) //contourss.size();???�기만큼 for문을 ?�린??(?�롭�??�의??객체 ?�이�??? 
            {
                double area = contourArea(contours[p]);
                if (area < 10) continue;
                RotatedRect rrt = minAreaRect(contours[p]);//객체 ?�곽?�을 ?��? ?�을 만큼 ???�이즈�? wRrt???�아�?
                Rect rt = rrt.boundingRect(); // 객체???�치???��??�이 ?�방???�각???�성

                if (10 <= rt.width && rt.width <= 20)//small
                {
                }
                else {
                    Rect rtSubErrRgn = rt;
                    rtSubErrRgn.x += vRois_Small[i].x - inflate;
                    rtSubErrRgn.y += vRois_Small[i].y;
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //src_openn?�서 찾아??객체???�곽??부분의 wR2f.tl() 좌상 �???�과

                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //src_openn?�서 찾아??객체???�곽??부분의 wR2f.tl() 좌상 �???�과
                    Rect rtDraw = rt;//chips
                    rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate;
                    rtDraw.y += vRois_Small[i].y + vRois[k].y;
                    vRois_Small_Err.push_back(rtDraw);
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //src_openn?�서 찾아??객체???�곽??부분의 wR2f.tl() 좌상 �???�과
                }
            }
        }
    }

    //display result :: error rectangle
    for (size_t i = 0; i < vRois_Large_Err.size(); i++)
    {
        cv::rectangle(src_draw, vRois_Large_Err[i], CV_RGB(255, 0, 255), 2); //src_openn?�서 찾아??객체???�곽??부분의 wR2f.tl() 좌상 �???�과
    }
    for (size_t i = 0; i < vRois_Small_Err.size(); i++)
    {
        cv::rectangle(src_draw, vRois_Small_Err[i], CV_RGB(255, 0, 255), 2); //src_openn?�서 찾아??객체???�곽??부분의 wR2f.tl() 좌상 �???�과
    }
    int a = 0;

    return 1;
}
