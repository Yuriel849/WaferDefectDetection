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
        RotatedRect rrt = minAreaRect(contours[k]);//ê°ì²´ ?¸ê³½? ì„ ?˜ì? ?Šì„ ë§Œí¼ ???¬ì´ì¦ˆë? wRrt???´ì•„ì¤?
        Rect rt = rrt.boundingRect(); // ê°ì²´???„ì¹˜???ê??†ì´ ?•ë°©???¬ê°???ì„±
        //???•ì‚¬ê°í˜• ?ì„± ?´í›„ ?¹ì • ê¸¸ì´ ì¶”ì¶œ
        if (13 <= rt.width && rt.width <= 17)//ê°ì²´ ê°?small ?¬ì´ì¦?rtê¸¸ì´ 13?´ìƒ 17?´í•˜??ê²½ìš°
        {

            vRois_Small.push_back(rt); //ê°??¸ë?Small ê°ì²´ ì¶œë ¥
            cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //rrt.boundingRect()?„ì—??êµ¬í•œ ?¬ê°??ê°’ì— ?¬ê°?•ì„ ê·¸ë ¤ì¤?
        }
        if (18 <= rt.width && rt.width <= 22)//ê°ì²´ ê°?small ?¬ì´ì¦?rtê¸¸ì´ 18?´ìƒ 22?´í•˜??ê²½ìš°
        {

            vRois_Large.push_back(rt); //ê°?Large?¸ë? ê°ì²´ ì¶œë ¥
            cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //rrt.boundingRect()?„ì—??êµ¬í•œ ?¬ê°??ê°’ì— ?¬ê°?•ì„ ê·¸ë ¤ì¤?
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
    vector<cv::Rect> vRois; //?„ì²´ ?´ë?ì§€ ë°±í„° ë°°ì—´ë¡??€??
    Mat obj_Region;
    Point ptThrehold = Point(888, 150); //?´ì§„?”ëœ ë°°ê²½ ?‰ë³´??ë°ì„ ê²½ìš° ì°¨ì´ë¥?ì£¼ê¸° ?„í•´ ë°°ê²½ ?„ì¹˜ ê°??¬ì¸?°ë¡œ ?¤ì • 
    double min_threshold = src.data[ptThrehold.y * src.cols + ptThrehold.x] + 5; //ë°°ê²½?‰ë³´??ë°ì„ ë¶€ë¶??¤ì •?˜ì—¬
    cv::threshold(src, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region???´ì§„???‘ì—…
    contours.clear(); //?ˆë¡­ê²?ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ë°±í„° ?¬ì¸??ë³€??contourss ?ì„±
    hierarchy.clear(); //ë°±í„° hierarchyyë³€???ì„±
    cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //ê°ì²´ ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ê°ì²´ ?¸ê³½??ê²€ì¶?
    for (size_t k = 0; k < contours.size(); k++) //contourss.size();???¬ê¸°ë§Œí¼ forë¬¸ì„ ?Œë¦°??(?ˆë¡­ê²??•ì˜??ê°ì²´ ?¬ì´ì¦??? 
    {
        double area = contourArea(contours[k]);

        RotatedRect rrt = minAreaRect(contours[k]);//ê°ì²´ ?¸ê³½? ì„ ?˜ì? ?Šì„ ë§Œí¼ ???¬ì´ì¦ˆë? wRrt???´ì•„ì¤?
        Rect rt = rrt.boundingRect(); // ê°ì²´???„ì¹˜???ê??†ì´ ?•ë°©???¬ê°???ì„±

        if (160 <= rt.width && rt.width <= 200)
        {
            vRois.push_back(rt);
            cv::rectangle(drawColor, rrt.boundingRect(), CV_RGB(0, 255, 255), 2); //
        }
        int checking1 = 0;
    }

    //sub chips
    vector<cv::Rect> vRois_Small_Err, vRois_Large_Err; // ?¤ëª° ê°ì²´, ?¼ì? ê°ì²´ ê²€ì¶??€?¥ì„ ?„í•œ ë°±í„° ?¤ì •
    vector<int> remember;
    for (size_t k = 0; k < vRois.size(); k++) // ?„ì²´ ?´ë?ì§€ ê°ì²´vRois.size()??ê°œìˆ˜ë§Œí¼ forë¬¸ì„ ?Œë¦°??(?ˆë¡­ê²??•ì˜??ê°ì²´ ?¬ì´ì¦??? 
    {
        Mat sub_img = src(vRois[k]).clone(); // vRois[k]ë²ˆì§¸ ê°ì²´ sub_img???½ì… 
        Mat sub_img_draw;
        cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR);

        int cnt_Large = 0, cnt_Small = 0, cnt_Large_Err = 0, cnt_Small_Err = 0;

        //large pad
        int inflate = 7; //ê°ì²´ rect ì¢Œìš° ?„ì¹˜ ?¤ì°¨ë¥?ì¤„ì´ê¸??„í•´ 7ë§Œí¼ ??¸°ê¸??„í•œ ë³€???¤ì •
        for (size_t i = 0; i < vRois_Large.size(); i++) // ?¸ë? ?¼ì? ê°ì²´ ?¬ì´ì¦ˆë§Œ??ë°˜ë³µ
        {
            Rect rt = vRois_Large[i]; // ?¸ë? ?¼ì?ê°ì²´ ië²ˆì§¸ rt???´ìŒ
            rt.x -= inflate; // ?¸ë? ?¼ì?ê°ì²´ ië²ˆì§¸??x?„ì¹˜?ì„œ 7ë§Œí¼ ë¹¼ì„œ ì¢Œë¡œ ?´ë™ (?¤ëª°ê°ì²´ x ì¢Œí‘œ??-7ë§Œí¼ ?¼ìª½?¼ë¡œ xì¶??´ë™)
            rt.width += inflate * 2; // ê°€ë¡œì‚¬?´ì¦ˆ ?”í´??5ë¥?2ë°°ë¡œ ?˜ë ¤ ?¤ê³½???¡ì•„ì¤€??//ê°€ë¡?ê¸¸ì´ 7*2=14ë§Œí¼ rt.widthê¸¸ì´ ?¤ì •?˜ì—¬ ë²”ìœ„ ?¤ì •
            rt.height += inflate + 4;// ?¸ë¡œ?¬ì´ì¦??”í´??5ë¥?2ë°°ë¡œ ?˜ë ¤ ?¤ê³½???¡ì•„ì¤€??rt.height += inflate * 2(?¤í¬?˜ì¹˜ê¹Œì? ?¡ì•„ì¤˜ì„œ  + 4ë¡??¬ê¸° ì¡°ì •
            //     
            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //sub_img_draw??rtê°ì²´???•ë³´ë§Œí¼ ?¬ê°??ê·¸ë ¤ì¤?

            Mat Pad = sub_img(rt).clone(); // ((?¸ë? ê°ì²´ ?¬ê°???•ë³´ë¥??°ì„œ Pad??ë³µì‚¬))
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY); //?´ì§„??

            //morpology ?¸ì´ì¦??œê±° ë¶€ë¶?
            int kernelSz = 2; //?¸ì´ì¦??¬ê¸°
            int shape = MorphShapes::MORPH_RECT; //
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //?•lSz + 1) 
            Mat SE = cv::getStructuringElement(shape, sz); // 
            Mat Pad_Open; //?¸ì´ì¦ˆê? ?œê±°???íƒœ.
            int type = MorphTypes::MORPH_OPEN; // ?¸ì´ì¦ˆë? ?œê±°?˜ëŠ” ê¸°ëŠ¥ MORPH_OPEN??type???´ê² ??
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);// cv::morphologyEx(src_bin(?…ë ¥), src_open(ì¶œë ¥), type, SE);//morphologyEx ?¸ì´ì¦ˆë? ?œê±°?˜ê² ?? 

            contours.clear(); //clearë¥??¨ì„œ ?ˆë¡¬ê²?ì´ˆê¸°??
            hierarchy.clear(); //clearë¥??¨ì„œ ?ˆë¡¬ê²?ì´ˆê¸°??
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Pad_Bin??ê°?ê°ì²´ ?¸ê³½??ê²€ì¶?
            for (size_t p = 0; p < contours.size(); p++) //Pad_Bin??ê°?ê°ì²´ ?˜ë§Œ??forë¬¸ì„ ?Œë¦°?? 
            {
                double area = contourArea(contours[p]); //ê²€ì¶œëœ ê°ì²´ ?„ì²´ ë©´ì  ê°?area???€??
                if (area < 10) continue; //areaê°€ 10???˜ì? ?Šìœ¼ë©?ì¢…ë£Œ
                RotatedRect rrt = minAreaRect(contours[p]);//ê°ì²´ ?¸ê³½? ì„ ?˜ì? ?Šì„ ë§Œí¼ ???¬ì´ì¦ˆë? rrt???´ì•„ì¤?
                Rect rt = rrt.boundingRect(); // ê°ì²´???„ì¹˜???ê??†ì´ ?•ë°©???¬ê°???ì„±?˜ì—¬ rt???´ì•„ì¤?

                //large ?¬ì´ì¦??¼ì • ì¡°ê±´ ê¸¸ì´ ì¶”ì¶œ  //rt ê²€ì¶œëœ ê°ì²´ ê¸¸ì´ 18 <= rt.width && rt.width <= 22
                if (((avg_Width_Large * 0.9) <= rt.width && rt.width <= (avg_Width_Large * 1.1))
                    && ((avg_Height_Large * 0.9) <= rt.height && rt.height <= (avg_Height_Large * 1.1)))
                {
                    cnt_Large++;
                }
                else
                {
                    vRegions->push_back(vRois[k]);
                    cnt_Large_Err++;
                    Rect rtSubErrRgn = rt; //ì¡°ê±´?? ì¶©ì¡±?˜ì? ?Šì? rtê°?rtSubErrRgn???€??
                    rtSubErrRgn.x += vRois_Large[i].x - inflate; // x???„ì¹˜??-7ë§Œí¼ ?„ì¹˜ ì¡°ì •
                    rtSubErrRgn.y += vRois_Large[i].y; //y???„ì¹˜??ê·¸ë?ë¡?
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //?„ì¹˜ê°??°ì„œ ?¬ê°??ê·¸ë ¤ì¤?

                    Rect rtDraw = rt;
                    rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate; //rtDraw.x??vRois[k].x?„ì²´ ?¬ê¸° ì¤‘ê°„ ê°ì²´ ì¶”ì¶œ ì¢Œí‘œë¥??”í•˜ê³??¸ë?ê°ì²´ xì¢Œí‘œ vRois_Small[i].xë¥??´ì•„ ?¬ê³¼?ìœ¼ë¡??´ë‹¹ ?¸ë?ê°ì²´ xì¢Œí‘œë¥?ì§ì ‘?ìœ¼ë¡?ê°€ë¦¬í‚¤ê²???
                    rtDraw.y += vRois_Large[i].y + vRois[k].y; //rtDraw.y??vRois[k].y?„ì²´ ?¬ê¸° ì¤‘ê°„ ê°ì²´ ì¶”ì¶œ ì¢Œí‘œë¥??”í•˜ê³??¸ë?ê°ì²´ yì¢Œí‘œ vRois_Small[i].yë¥??´ì•„ ?¬ê³¼?ìœ¼ë¡??´ë‹¹ ?¸ë?ê°ì²´ yì¢Œí‘œë¥?ì§ì ‘?ìœ¼ë¡?ê°€ë¦¬í‚¤ê²???
                    vRois_Large_Err.push_back(rtDraw); // ì§ì ‘?ìœ¼ë¡?ê°€ë¦¬í‚¨ ì¢Œí‘œ 
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //
                    //?‰ì¹ ??ì¹?ê¸°ì–µ?˜ê¸° 
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
        for (size_t i = 0; i < vRois_Small.size(); i++) //small pads ?˜ë‚˜??ë³´ë©´???´ì§„?? ?¤ë¥˜ ?•ì¸
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

            //morpology ?¸ì´ì¦??œê±° ë¶€ë¶?
            int kernelSz = 2; //?¸ì´ì¦??¬ê¸°
            int shape = MorphShapes::MORPH_RECT; //
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //?•lSz + 1) 
            Mat SE = cv::getStructuringElement(shape, sz); // 
            Mat Pad_Open; //?¸ì´ì¦ˆê? ?œê±°???íƒœ.
            int type = MorphTypes::MORPH_OPEN; // ?¸ì´ì¦ˆë? ?œê±°?˜ëŠ” ê¸°ëŠ¥ MORPH_OPEN??type???´ê² ??
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);// cv::morphologyEx(src_bin(?…ë ¥), src_open(ì¶œë ¥), type, SE);//morphologyEx ?¸ì´ì¦ˆë? ?œê±°?˜ê² ?? 

            contours.clear(); //
            hierarchy.clear(); //
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //ê°ì²´ ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ê°ì²´ ?¸ê³½??ê²€ì¶?
            for (size_t p = 0; p < contours.size(); p++) //contourss.size();???¬ê¸°ë§Œí¼ forë¬¸ì„ ?Œë¦°??(?ˆë¡­ê²??•ì˜??ê°ì²´ ?¬ì´ì¦??? 
            {
                double area = contourArea(contours[p]);
                if (area < 10) continue;
                RotatedRect rrt = minAreaRect(contours[p]);//ê°ì²´ ?¸ê³½? ì„ ?˜ì? ?Šì„ ë§Œí¼ ???¬ì´ì¦ˆë? rrt???´ì•„ì¤?
                Rect rt = rrt.boundingRect(); // rt ê°ì²´???„ì¹˜???ê??†ì´ ?•ë°©???¬ê°???ì„±

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
                    rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate; //rtDraw.x??vRois[k].x?„ì²´ ?¬ê¸° ì¤‘ê°„ ê°ì²´ ì¶”ì¶œ ì¢Œí‘œë¥??”í•˜ê³??¸ë?ê°ì²´ xì¢Œí‘œ vRois_Small[i].xë¥??´ì•„ ?¬ê³¼?ìœ¼ë¡??´ë‹¹ ?¸ë?ê°ì²´ xì¢Œí‘œë¥?ì§ì ‘?ìœ¼ë¡?ê°€ë¦¬í‚¤ê²???
                    rtDraw.y += vRois_Small[i].y + vRois[k].y;
                    vRois_Small_Err.push_back(rtDraw);
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //
                    //?‰ì¹ ??ì¹?ê¸°ì–µ?˜ê¸° 
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
            //?‰ì¹ ??ì¹?ê¸°ì–µ?˜ê¸° 
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
        // ?¬ê°???´ë????½ì? ?‰ìƒ ?•ì¸
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
