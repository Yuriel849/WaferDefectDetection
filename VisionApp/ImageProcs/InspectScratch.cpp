#include "InspectScratch.h"



InspectScratch::InspectScratch()
{
}

InspectScratch::~InspectScratch()
{
}

int InspectScratch::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions)
{
    // find          MechingMethod()ë¡?ê²°í•¨ ?†ëŠ” ?´ë?ì§€ë¥??œí”Œë¦??ìƒ‰?˜ì—¬ ?»ì?, ?¬ë°”ë¥?ì¢Œí‘œ ë°°ì—´ -> vRegions
    // search_img    ?ìƒ‰???´ë?ì§€ IMREAD_GRAYSCALE -> src
    // drawColor       ?¬ê°??ê·¸ë¦´ ?´ë?ì§€ IMREAD_ANYCOLOR ->  X
    // fill_img        ??ì±„ìš¸ ?´ë?ì§€ IMREAD_ANYCOLOR -> drawColor

    ////Identify the large and small squares in each chip across the wafer under inspection///////////
    vector<cv::Rect> vRois; //?„ì²´ ?´ë?ì§€ ë°±í„° ë°°ì—´ë¡??€??
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Mat obj_Region;
    Point ptThrehold = Point(888, 150); //?´ì§„?”ëœ ë°°ê²½ ?‰ë³´??ë°ì„ ê²½ìš° ì°¨ì´ë¥?ì£¼ê¸° ?„í•´ ë°°ê²½ ?„ì¹˜ ê°??¬ì¸?°ë¡œ ?¤ì • 
    double min_threshold = src.data[ptThrehold.y * src.cols + ptThrehold.x] + 5; //ë°°ê²½?‰ë³´??ë°ì„ ë¶€ë¶??¤ì •?˜ì—¬
    cv::threshold(src, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region???´ì§„???‘ì—…
    contours.clear(); //?ˆë¡­ê²?ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ë°±í„° ?¬ì¸??ë³€??contourss ?ì„±
    hierarchy.clear(); //ë°±í„° hierarchyë³€???ì„±
    cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //ê°ì²´ ë²”ìœ„ê°’ì„ êµ¬í•˜ê¸??„í•´ ê°ì²´ ?¸ê³½??ê²€ì¶?
    for (size_t k = 0; k < contours.size(); k++) //contourss.size();???¬ê¸°ë§Œí¼ forë¬¸ì„ ?Œë¦°??(?ˆë¡­ê²??•ì˜??ê°ì²´ ?¬ì´ì¦??? 
    {
        double area = contourArea(contours[k]);

        RotatedRect rrt = minAreaRect(contours[k]);//ê°ì²´ ?¸ê³½? ì„ ?˜ì? ?Šì„ ë§Œí¼ ???¬ì´ì¦ˆë? wRrt???´ì•„ì¤?
        Rect rt = rrt.boundingRect(); // ê°ì²´???„ì¹˜???ê??†ì´ ?•ë°©???¬ê°???ì„±

        if (160 <= rt.width && rt.width <= 200)
        {
            vRois.push_back(rt);
            cv::rectangle(drawColor, rrt.boundingRect(), CV_RGB(0, 0, 255), 2); //
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    RNG rng(12345);
    Scalar color_scratch(0, 0, 255);

    for (size_t i = 0; i < vRois.size(); i++)
    {
        Scalar color;
        Mat binScratch;
        Mat subImgScratch = src(vRois[i]).clone();

        //double thres_scratch = 210;
        cv::Point ptThreshold(817, 250);
        double thres_scratch = src.at<uchar>(ptThreshold.y, ptThreshold.x) + 60;
        cv::threshold(subImgScratch, binScratch, thres_scratch, 255, ThresholdTypes::THRESH_BINARY);
        cv::findContours(binScratch, contours_scratch, hierarchy_scratch, RETR_TREE, CHAIN_APPROX_SIMPLE);

        string msg;
        msg = to_string(scratch_num);

        if (contours_scratch.empty())
        {
            // color = color_flawless;
        }

        else
        {
            for (size_t k = 0; k < contours_scratch.size(); k++) //contourss.size();???¬ê¸°ë§Œí¼ forë¬¸ì„ ?Œë¦°??(?ˆë¡­ê²??•ì˜??ê°ì²´ ?¬ì´ì¦??? 
            {
                double area = contourArea(contours_scratch[k]);

                double area_min = 10 * 10;
                if (area_min <= area)
                {
                    vector<vector<Point>> contours_scratch_error_all;
                    vector<Point> contours_scratch_error;
                    for (size_t p = 0; p < contours_scratch[k].size(); p++)
                    {
                        Point pt;
                        pt.x = contours_scratch[k].at(p).x + vRois[i].x;
                        pt.y = contours_scratch[k].at(p).y + vRois[i].y;
                        contours_scratch_error.push_back(pt);
                    }
                    contours_scratch_error_all.push_back(contours_scratch_error);
                    drawContours(drawColor, contours_scratch_error_all, 0, color_scratch, 3);


                    color = color_scratch;
                    scratch_num += 1;
                    cv::putText(drawColor, msg, vRois[i].br(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255, 255, 0), 1.5, 8);      //0, 255, 255
                    cv::putText(drawColor, scratch, vRois[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
                    cv::rectangle(drawColor, vRois[i], color, CV_FILLED);

                    vRegions->push_back(vRois[i]); //scratch ?¤ë¥˜ ?¬ê°?•ì˜ ?°ì´?°ë? ?€?? 

                    //cv::Point rtPts[4] = { vRegions[i].tl(), Point(vRegions[i].tl().x + vRegions[i].width, vRegions[i].tl().y),
                    //vRegions[i].br(), Point(vRegions[i].tl().x, vRegions[i].br().y) };
                    //for (size_t c = 0; c < 4; c++)
                    //{
                    //    draw_line(drawColor, rtPts[c % 4], rtPts[(c + 1) % 4], color, 2, "", 3);
                    //}
                }
            }

        }
    }
    return 0;
}

void InspectScratch::draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap)
{
#if 1
    float dx = pt1.x - pt2.x;
    float dy = pt1.y - pt2.y;

    float dist = sqrt(dx * dx + dy * dy);

    std::vector<cv::Point> pts;
    for (int i = 0; i < dist; i += gap)
    {
        float r = (float)i / dist;
        int x = int((pt1.x * (1.0 - r) + pt2.x * r) + .5);
        int y = int((pt1.y * (1.0 - r) + pt2.y * r) + .5);
        cv::Point p = cv::Point(x, y);
        pts.push_back(p);
    }

    int pts_size = pts.size();

    if (style == "dotted")
    {
        for (int i = 0; i < pts_size; i++)
        {
            cv::circle(img, pts[i], thickness, color, -1);
        }
    }
    else
    {
        cv::Point s = pts[0];
        cv::Point e = pts[0];

        //int count = 0;

        for (int i = 0; i < pts_size; i++)
        {
            s = e;
            e = pts[i];

            if (i % 2 == 1)
            {
                cv::line(img, s, e, color, thickness);
            }
        }
    }
#endif
}