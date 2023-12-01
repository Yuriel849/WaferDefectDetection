#include "InspectContamination.h"


InspectContamination::InspectContamination()
{

}

InspectContamination::~InspectContamination()
{

}

int InspectContamination::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions)
{
    float avg_Width_Large, avg_Height_Large, avg_Width_Small, avg_Height_Small;
    vector<Rect> remeber_con;

    Mat ptrn = cv::imread(PATTERN, cv::ImreadModes::IMREAD_GRAYSCALE);
    Mat ptrn_Draw;
    cvtColor(ptrn, ptrn_Draw, COLOR_GRAY2BGR); //draw_Pads
    Mat ptrn_Pads_Region;
    double obj_PAD_threshold = 130;
    cv::threshold(ptrn, ptrn_Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    cv::findContours(ptrn_Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<cv::Rect> vRois_Small, vRois_Large;
    for (size_t k = 0; k < contours.size(); k++)
    {
        RotatedRect rrt = minAreaRect(contours[k]);//객체 ?�곽?�을 ?��? ?�을 만큼 ???�이즈�? wRrt???�아�?
        Rect rt = rrt.boundingRect(); // 객체???�치???��??�이 ?�방???�각???�성
        //???�사각형 ?�성 ?�후 ?�정 길이 추출
        if (13 <= rt.width && rt.width <= 17)//객체 �?small ?�이�?rt길이 13?�상 17?�하??경우
        {

            vRois_Small.push_back(rt); //�??��?Small 객체 출력
            cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //rrt.boundingRect()?�에??구한 ?�각??값에 ?�각?�을 그려�?
        }
        if (18 <= rt.width && rt.width <= 22)//객체 �?small ?�이�?rt길이 18?�상 22?�하??경우
        {

            vRois_Large.push_back(rt); //�?Large?��? 객체 출력
            cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //rrt.boundingRect()?�에??구한 ?�각??값에 ?�각?�을 그려�?
        }

    }
    int checking0 = 0;
    // Calculate average widths and heights of the large and small squares in each chip
    int sum_Width_Large = 0, sum_Height_Large = 0, sum_Width_Small = 0, sum_Height_Small = 0;
    for (size_t k = 0; k < vRois_Small.size(); k++)
    {
        sum_Width_Small += vRois_Small[k].width;
        sum_Height_Small += vRois_Small[k].height;
    }
    avg_Width_Small = static_cast<float>(sum_Width_Small) / vRois_Small.size();
    avg_Height_Small = static_cast<float>(sum_Height_Small) / vRois_Small.size();
    for (size_t k = 0; k < vRois_Large.size(); k++)
    {
        sum_Width_Large += vRois_Large[k].width;
        sum_Height_Large += vRois_Large[k].height;
    }
    avg_Width_Large = static_cast<float>(sum_Width_Large) / vRois_Large.size();
    avg_Height_Large = static_cast<float>(sum_Height_Large) / vRois_Large.size();

    // Identify the large and small squares in each chip across the wafer under inspection
    vector<cv::Rect> vRois; //?�체 ?��?지 백터 배열�??�??
    Mat obj_Region;
    Point ptThrehold = Point(888, 150); //?�진?�된 배경 ?�보??밝을 경우 차이�?주기 ?�해 배경 ?�치 �??�인?�로 ?�정 
    double min_threshold = src.data[ptThrehold.y * src.cols + ptThrehold.x] + 5; //배경?�보??밝을 부�??�정?�여
    cv::threshold(src, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region???�진???�업
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
            cv::rectangle(drawColor, rrt.boundingRect(), CV_RGB(0, 255, 255), 2); //
        }
        int checking1 = 0;
    }

    //sub chips
    vector<cv::Rect> vRois_Small_Err, vRois_Large_Err; // ?�몰 객체, ?��? 객체 검�??�?�을 ?�한 백터 ?�정
    vector<int> remember;
    for (size_t k = 0; k < vRois.size(); k++) // ?�체 ?��?지 객체vRois.size()??개수만큼 for문을 ?�린??(?�롭�??�의??객체 ?�이�??? 
    {
        Mat sub_img = src(vRois[k]).clone(); // vRois[k]번째 객체 sub_img???�입 
        Mat sub_img_draw;
        cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR);

        int cnt_Large = 0, cnt_Small = 0, cnt_Large_Err = 0, cnt_Small_Err = 0;

        //large pad
        int inflate = 7; //객체 rect 좌우 ?�치 ?�차�?줄이�??�해 7만큼 ??���??�한 변???�정
        for (size_t i = 0; i < vRois_Large.size(); i++) // ?��? ?��? 객체 ?�이즈만??반복
        {
            Rect rt = vRois_Large[i]; // ?��? ?��?객체 i번째 rt???�음
            rt.x -= inflate; // ?��? ?��?객체 i번째??x?�치?�서 7만큼 빼서 좌로 ?�동 (?�몰객체 x 좌표??-7만큼 ?�쪽?�로 x�??�동)
            rt.width += inflate * 2; // 가로사?�즈 ?�폴??5�?2배로 ?�려 ?�곽???�아준??//가�?길이 7*2=14만큼 rt.width길이 ?�정?�여 범위 ?�정
            rt.height += inflate + 4;// ?�로?�이�??�폴??5�?2배로 ?�려 ?�곽???�아준??rt.height += inflate * 2(?�크?�치까�? ?�아줘서  + 4�??�기 조정
            //     
            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //sub_img_draw??rt객체???�보만큼 ?�각??그려�?

            Mat Pad = sub_img(rt).clone(); // ((?��? 객체 ?�각???�보�??�서 Pad??복사))
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY); //?�진??

            //morpology ?�이�??�거 부�?
            int kernelSz = 2; //?�이�??�기
            int shape = MorphShapes::MORPH_RECT; //
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //?�lSz + 1) 
            Mat SE = cv::getStructuringElement(shape, sz); // 
            Mat Pad_Open; //?�이즈�? ?�거???�태.
            int type = MorphTypes::MORPH_OPEN; // ?�이즈�? ?�거?�는 기능 MORPH_OPEN??type???�겠??
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);// cv::morphologyEx(src_bin(?�력), src_open(출력), type, SE);//morphologyEx ?�이즈�? ?�거?�겠?? 

            contours.clear(); //clear�??�서 ?�롬�?초기??
            hierarchy.clear(); //clear�??�서 ?�롬�?초기??
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Pad_Bin??�?객체 ?�곽??검�?
            for (size_t p = 0; p < contours.size(); p++) //Pad_Bin??�?객체 ?�만??for문을 ?�린?? 
            {
                double area = contourArea(contours[p]); //검출된 객체 ?�체 면적 �?area???�??
                if (area < 10) continue; //area가 10???��? ?�으�?종료
                RotatedRect rrt = minAreaRect(contours[p]);//객체 ?�곽?�을 ?��? ?�을 만큼 ???�이즈�? rrt???�아�?
                Rect rt = rrt.boundingRect(); // 객체???�치???��??�이 ?�방???�각???�성?�여 rt???�아�?

                //large ?�이�??�정 조건 길이 추출  //rt 검출된 객체 길이 18 <= rt.width && rt.width <= 22
                if (((avg_Width_Large * 0.9) <= rt.width && rt.width <= (avg_Width_Large * 1.1))
                    && ((avg_Height_Large * 0.9) <= rt.height && rt.height <= (avg_Height_Large * 1.1)))
                {
                    cnt_Large++;
                }
                else
                {
                    vRegions->push_back(vRois[k]);
                    cnt_Large_Err++;
                    Rect rtSubErrRgn = rt; //조건?? 충족?��? ?��? rt�?rtSubErrRgn???�??
                    rtSubErrRgn.x += vRois_Large[i].x - inflate; // x???�치??-7만큼 ?�치 조정
                    rtSubErrRgn.y += vRois_Large[i].y; //y???�치??그�?�?
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //?�치�??�서 ?�각??그려�?

                    Rect rtDraw = rt;
                    rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate; //rtDraw.x??vRois[k].x?�체 ?�기 중간 객체 추출 좌표�??�하�??��?객체 x좌표 vRois_Small[i].x�??�아 ?�과?�으�??�당 ?��?객체 x좌표�?직접?�으�?가리키�???
                    rtDraw.y += vRois_Large[i].y + vRois[k].y; //rtDraw.y??vRois[k].y?�체 ?�기 중간 객체 추출 좌표�??�하�??��?객체 y좌표 vRois_Small[i].y�??�아 ?�과?�으�??�당 ?��?객체 y좌표�?직접?�으�?가리키�???
                    vRois_Large_Err.push_back(rtDraw); // 직접?�으�?가리킨 좌표 
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //
                    //?�칠??�?기억?�기 
                    remember.push_back(k);
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
        for (size_t i = 0; i < vRois_Small.size(); i++) //small pads ?�나??보면???�진?? ?�류 ?�인
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

            //morpology ?�이�??�거 부�?
            int kernelSz = 2; //?�이�??�기
            int shape = MorphShapes::MORPH_RECT; //
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //?�lSz + 1) 
            Mat SE = cv::getStructuringElement(shape, sz); // 
            Mat Pad_Open; //?�이즈�? ?�거???�태.
            int type = MorphTypes::MORPH_OPEN; // ?�이즈�? ?�거?�는 기능 MORPH_OPEN??type???�겠??
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);// cv::morphologyEx(src_bin(?�력), src_open(출력), type, SE);//morphologyEx ?�이즈�? ?�거?�겠?? 

            contours.clear(); //
            hierarchy.clear(); //
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하�??�해 객체 ?�곽??검�?
            for (size_t p = 0; p < contours.size(); p++) //contourss.size();???�기만큼 for문을 ?�린??(?�롭�??�의??객체 ?�이�??? 
            {
                double area = contourArea(contours[p]);
                if (area < 10) continue;
                RotatedRect rrt = minAreaRect(contours[p]);//객체 ?�곽?�을 ?��? ?�을 만큼 ???�이즈�? rrt???�아�?
                Rect rt = rrt.boundingRect(); // rt 객체???�치???��??�이 ?�방???�각???�성

                if (((avg_Width_Small * 0.8) <= rt.width && rt.width <= (avg_Width_Small * 1.2))
                    && ((avg_Height_Small * 0.8) <= rt.height && rt.height <= (avg_Height_Small * 1.2)))
                {
                    cnt_Small++;
                }
                else
                {
                    vRegions->push_back(vRois[k]);
                    cnt_Small_Err++;
                    Rect rtSubErrRgn = rt;
                    rtSubErrRgn.x += vRois_Small[i].x - inflate;
                    rtSubErrRgn.y += vRois_Small[i].y;
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 123), 2); //

                    // cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //
                    Rect rtDraw = rt;//chips
                    rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate; //rtDraw.x??vRois[k].x?�체 ?�기 중간 객체 추출 좌표�??�하�??��?객체 x좌표 vRois_Small[i].x�??�아 ?�과?�으�??�당 ?��?객체 x좌표�?직접?�으�?가리키�???
                    rtDraw.y += vRois_Small[i].y + vRois[k].y;
                    vRois_Small_Err.push_back(rtDraw);
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //
                    //?�칠??�?기억?�기 
                    remember.push_back(k);
                }
                int checking4 = 0;
            }
            int checking5 = 0;
        }
        int checking6 = 0;
        /*if (cnt_Small == 10 && cnt_Small_Err != 0)
        {
            vRois_Small_Err.resize(vRois_Small_Err.size() - cnt_Small_Err);
        }*/
        if (remember.size() > 0)
        {
            //?�칠??�?기억?�기 
            remeber_con.push_back(vRois[k]);
            remember.clear();
        }

    }

    int con_num = 1;
    string msg;
    msg = to_string(con_num);
    //display result :: error rectangle
    for (size_t i = 0; i < remeber_con.size(); i++)
    {
        // ?�각???��????��? ?�상 ?�인
        cv::Scalar currentPixelColor = drawColor.at<cv::Vec3b>(remeber_con[i].y + remeber_con[i].height / 2,
            remeber_con[i].x + remeber_con[i].width / 2);
        if (currentPixelColor == CV_RGB(255, 0, 0) || currentPixelColor == CV_RGB(255, 255, 0)) {
            int check112 = 0;
        }
        else
        {
            Scalar color = CV_RGB(255, 0, 255);
            con_num += 1;
            cv::putText(drawColor, msg, remeber_con[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);		//0, 255, 255
            cv::putText(drawColor, "Contamination", remeber_con[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
            cv::rectangle(drawColor, remeber_con[i], color, CV_FILLED); //
            int check119 = 0;
        }
        int b = 0;
    }
    //for (size_t i = 0; i < vRois_Large_Err.size(); i++)
    //{
    //    cv::rectangle(drawColor, vRois_Large_Err[i], CV_RGB(255, 0, 255), 2); //
    //}
    //for (size_t i = 0; i < vRois_Small_Err.size(); i++)
    //{
    //    cv::rectangle(drawColor, vRois_Small_Err[i], CV_RGB(255, 0, 255), 2); //
    //}


    return 1;
}
