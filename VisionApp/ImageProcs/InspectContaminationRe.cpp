#include "InspectContaminationRe.h"


InspectContaminationRe::InspectContaminationRe() {}

InspectContaminationRe::~InspectContaminationRe() {}

int InspectContaminationRe::GetObjectRegionsTemplatMatching(const Mat& src, const Mat& ptrn, std::vector<cv::Rect>& vRegions) { return 0; }

int InspectContaminationRe::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions)
{
    cv::Mat src_gray_search = src;
    cv::Mat src_draw = drawColor;

    cv::Mat src_gray_ptrn = cv::imread(PATTERN, cv::ImreadModes::IMREAD_GRAYSCALE); //?€ê¹??´ë?ì§€ ?‘ë°±?¼ë¡œ ë¡œë”©
    Mat draw_Pads;
    cvtColor(src_gray_ptrn, draw_Pads, COLOR_GRAY2BGR); //draw_Pads
    Mat Pads_Region;
    double obj_PAD_threshold = 130;
    cv::threshold(src_gray_ptrn, Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);
    vector<vector<Point>> contours; //?ˆë¡­ê²?ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ë°±í„° ?¬ì¸??ë³€??contourss ?ì„±
    vector<Vec4i> hierarchy; //ë°±í„° hierarchyyë³€???ì„±
    cv::findContours(Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //ê°ì²´ ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ê°ì²´ ?¸ê³½??ê²€ì¶?

    vector<cv::Rect> vRois_Small, vRois_Large;
    for (size_t k = 0; k < contours.size(); k++)
    {
        RotatedRect rrt = minAreaRect(contours[k]);//ê°ì²´ ?¸ê³½? ì„ ?˜ì? ?Šì„ ë§Œí¼ ???¬ì´ì¦ˆë? wRrt???´ì•„ì¤?
        Rect rt = rrt.boundingRect(); // ê°ì²´???„ì¹˜???ê??†ì´ ?•ë°©???¬ê°???ì„±
        //???•ì‚¬ê°í˜• ?ì„± ?´í›„ ?¹ì • ê¸¸ì´ ì¶”ì¶œ
        if (13 <= rt.width && rt.width <= 17)//ê°ì²´ ê°?small ?¬ì´ì¦?rtê¸¸ì´ 13?´ìƒ 17?´í•˜??ê²½ìš°
        {
            vRois_Small.push_back(rt); //ê°??¸ë?Small ê°ì²´ ì¶œë ¥
            cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //rrt.boundingRect()?„ì—??êµ¬í•œ ?¬ê°??ê°’ì— ?¬ê°?•ì„ ê·¸ë ¤ì¤?
        }
        if (18 <= rt.width && rt.width <= 22)//ê°ì²´ ê°?small ?¬ì´ì¦?rtê¸¸ì´ 18?´ìƒ 22?´í•˜??ê²½ìš°
        {
            vRois_Large.push_back(rt); //ê°?Large?¸ë? ê°ì²´ ì¶œë ¥
            cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //rrt.boundingRect()?„ì—??êµ¬í•œ ?¬ê°??ê°’ì— ?¬ê°?•ì„ ê·¸ë ¤ì¤?
        }
    }

    //find objects
    vector<cv::Rect> vRois; //?„ì²´ ?´ë?ì§€ ë°±í„° ë°°ì—´ë¡??€??
    Mat obj_Region;
    Point ptThrehold = Point(888, 150); //?´ì§„?”ëœ ë°°ê²½ ?‰ë³´??ë°ì„ ê²½ìš° ì°¨ì´ë¥?ì£¼ê¸° ?„í•´ ë°°ê²½ ?„ì¹˜ ê°??¬ì¸?°ë¡œ ?¤ì • 
    double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5; //ë°°ê²½?‰ë³´??ë°ì„ ë¶€ë¶??¤ì •?˜ì—¬
    cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region???´ì§„???‘ì—…
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
            cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //
        }
    }

    //sub chips
    vector<cv::Rect> vRois_Small_Err, vRois_Large_Err; // ?¤ëª° ê°ì²´, ?¼ì? ê°ì²´ ê²€ì¶??€?¥ì„ ?„í•œ ë°±í„° ?¤ì •
    for (size_t k = 0; k < vRois.size(); k++) // ?„ì²´ ?´ë?ì§€ ê°ì²´vRois.size()??ê°œìˆ˜ë§Œí¼ forë¬¸ì„ ?Œë¦°??(?ˆë¡­ê²??•ì˜??ê°ì²´ ?¬ì´ì¦??? 
    {
        Mat sub_img = src_gray_search(vRois[k]).clone(); // vRois[k]ë²ˆì§¸ ê°ì²´ sub_img???½ì… 
        Mat sub_img_draw;
        cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR); //
        //large pad
        int inflate = 7; //ê°ì²´ rect ì¢Œìš° ?„ì¹˜ ?¤ì°¨ë¥?ì¤„ì´ê¸??„í•´ 7ë§Œí¼ ??¸°ê¸??„í•œ ë³€???¤ì •
        for (size_t i = 0; i < vRois_Large.size(); i++) // ?¸ë? ?¼ì? ê°ì²´ ?¬ì´ì¦ˆë§Œ??ë°˜ë³µ
        {
            Rect rt = vRois_Large[i]; // ?¸ë? ?¼ì?ê°ì²´ ië²ˆì§¸ rt???´ìŒ
            rt.x -= inflate; // ?¸ë? ?¼ì?ê°ì²´ ië²ˆì§¸??x?„ì¹˜?ì„œ 7ë§Œí¼ ë¹¼ì„œ ì¢Œë¡œ ?´ë™
            rt.width += inflate * 2; // ê°€ë¡œì‚¬?´ì¦ˆ ?”í´??5ë¥?2ë°°ë¡œ ?˜ë ¤ ?¤ê³½???¡ì•„ì¤€??
            rt.height += inflate + 4;//// ?¸ë¡œ?¬ì´ì¦??”í´??5ë¥?2ë°°ë¡œ ?˜ë ¤ ?¤ê³½???¡ì•„ì¤€??rt.height += inflate * 2

            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //sub_img_draw??rtê°ì²´???•ë³´ë§Œí¼ ?¬ê°??ê·¸ë ¤ì¤?

            Mat Pad = sub_img(rt).clone(); // ((?¸ë? ê°ì²´ ?¬ê°???•ë³´ë¥??°ì„œ Pad??ë³µì‚¬))
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY); //?´ì§„??

            contours.clear(); //clearë¥??¨ì„œ ?ˆë¡¬ê²?ì´ˆê¸°??
            hierarchy.clear(); //clearë¥??¨ì„œ ?ˆë¡¬ê²?ì´ˆê¸°??
            cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Pad_Bin??ê°?ê°ì²´ ?¸ê³½??ê²€ì¶?
            for (size_t p = 0; p < contours.size(); p++) //Pad_Bin??ê°?ê°ì²´ ?˜ë§Œ??forë¬¸ì„ ?Œë¦°?? 
            {
                double area = contourArea(contours[p]); //ê²€ì¶œëœ ê°ì²´ ?„ì²´ ë©´ì  ê°?area???€??
                if (area < 10) continue; //areaê°€ 10???˜ì? ?Šìœ¼ë©?ì¢…ë£Œ
                RotatedRect rrt = minAreaRect(contours[p]);//ê°ì²´ ?¸ê³½? ì„ ?˜ì? ?Šì„ ë§Œí¼ ???¬ì´ì¦ˆë? rrt???´ì•„ì¤?
                Rect rt = rrt.boundingRect(); // ê°ì²´???„ì¹˜???ê??†ì´ ?•ë°©???¬ê°???ì„±?˜ì—¬ rt???´ì•„ì¤?

                if (18 <= rt.width && rt.width <= 22)//large ?¬ì´ì¦??¼ì • ì¡°ê±´ ê¸¸ì´ ì¶”ì¶œ  //rt ê²€ì¶œëœ ê°ì²´ ê¸¸ì´ 18 <= rt.width && rt.width <= 22
                {

                }
                else
                {
                    Rect rtSubErrRgn = rt; //ì¡°ê±´?? ì¶©ì¡±?˜ì? ?Šì? rtê°?rtSubErrRgn???€??
                    rtSubErrRgn.x += vRois_Large[i].x - inflate; // x???„ì¹˜??-7ë§Œí¼ ?„ì¹˜ ì¡°ì •
                    rtSubErrRgn.y += vRois_Large[i].y; //y???„ì¹˜??ê·¸ë?ë¡?
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //?„ì¹˜ê°??°ì„œ ?¬ê°??ê·¸ë ¤ì¤?

                    Rect rtDraw = rt;
                    rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate;
                    rtDraw.y += vRois_Large[i].y + vRois[k].y;
                    vRois_Large_Err.push_back(rtDraw);
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn?ì„œ ì°¾ì•„??ê°ì²´???¸ê³½??ë¶€ë¶„ì˜ wR2f.tl() ì¢Œìƒ ê¼???ê³¼
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

            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 2); //src_openn?ì„œ ì°¾ì•„??ê°ì²´???¸ê³½??ë¶€ë¶„ì˜ wR2f.tl() ì¢Œìƒ ê¼???ê³¼

            Mat Pad = sub_img(rt).clone();
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

            contours.clear(); //?ˆë¡­ê²?ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ë°±í„° ?¬ì¸??ë³€??contourss ?ì„± 
            hierarchy.clear(); //ë°±í„° hierarchyyë³€???ì„±
            cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //ê°ì²´ ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ê°ì²´ ?¸ê³½??ê²€ì¶?
            for (size_t p = 0; p < contours.size(); p++) //contourss.size();???¬ê¸°ë§Œí¼ forë¬¸ì„ ?Œë¦°??(?ˆë¡­ê²??•ì˜??ê°ì²´ ?¬ì´ì¦??? 
            {
                double area = contourArea(contours[p]);
                if (area < 10) continue;
                RotatedRect rrt = minAreaRect(contours[p]);//ê°ì²´ ?¸ê³½? ì„ ?˜ì? ?Šì„ ë§Œí¼ ???¬ì´ì¦ˆë? wRrt???´ì•„ì¤?
                Rect rt = rrt.boundingRect(); // ê°ì²´???„ì¹˜???ê??†ì´ ?•ë°©???¬ê°???ì„±

                if (10 <= rt.width && rt.width <= 20)//small
                {
                }
                else {
                    Rect rtSubErrRgn = rt;
                    rtSubErrRgn.x += vRois_Small[i].x - inflate;
                    rtSubErrRgn.y += vRois_Small[i].y;
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //src_openn?ì„œ ì°¾ì•„??ê°ì²´???¸ê³½??ë¶€ë¶„ì˜ wR2f.tl() ì¢Œìƒ ê¼???ê³¼

                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //src_openn?ì„œ ì°¾ì•„??ê°ì²´???¸ê³½??ë¶€ë¶„ì˜ wR2f.tl() ì¢Œìƒ ê¼???ê³¼
                    Rect rtDraw = rt;//chips
                    rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate;
                    rtDraw.y += vRois_Small[i].y + vRois[k].y;
                    vRois_Small_Err.push_back(rtDraw);
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //src_openn?ì„œ ì°¾ì•„??ê°ì²´???¸ê³½??ë¶€ë¶„ì˜ wR2f.tl() ì¢Œìƒ ê¼???ê³¼
                }
            }
        }
    }

    //display result :: error rectangle
    for (size_t i = 0; i < vRois_Large_Err.size(); i++)
    {
        cv::rectangle(src_draw, vRois_Large_Err[i], CV_RGB(255, 0, 255), 2); //src_openn?ì„œ ì°¾ì•„??ê°ì²´???¸ê³½??ë¶€ë¶„ì˜ wR2f.tl() ì¢Œìƒ ê¼???ê³¼
    }
    for (size_t i = 0; i < vRois_Small_Err.size(); i++)
    {
        cv::rectangle(src_draw, vRois_Small_Err[i], CV_RGB(255, 0, 255), 2); //src_openn?ì„œ ì°¾ì•„??ê°ì²´???¸ê³½??ë¶€ë¶„ì˜ wR2f.tl() ì¢Œìƒ ê¼???ê³¼
    }
    int a = 0;

    return 1;
}
