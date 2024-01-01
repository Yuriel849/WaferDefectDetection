#include "InspectScratch.h"



InspectScratch::InspectScratch()
{
    _InspectName = "Scratch";
}

InspectScratch::~InspectScratch()
{
}

int InspectScratch::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num)
{

    // find          MechingMethod()로 결함 없는 이미지를 템플릿 탐색하여 얻은, 올바른 좌표 배열 -> vRegions
    // search_img    탐색할 이미지 IMREAD_GRAYSCALE -> src
    // drawColor       사각형 그릴 이미지 IMREAD_ANYCOLOR ->  X
    // fill_img        색 채울 이미지 IMREAD_ANYCOLOR -> drawColor

   ///////////////////////////////////////////////////////////////////////////////////////////////
    RNG rng(12345);
    Scalar color_scratch(0, 0, 255);

    for (size_t i = 0; i < vChips.size(); i++)
    {
        

        Scalar color;
        Mat binScratch;
        Mat subImgScratch = src(vChips[i]).clone();

        //double thres_scratch = 210;
        cv::Point ptThreshold(817, 250);
        double thres_scratch = src.at<uchar>(ptThreshold.y, ptThreshold.x) + 60;
        cv::threshold(subImgScratch, binScratch, thres_scratch, 255, ThresholdTypes::THRESH_BINARY);
        cv::findContours(binScratch, contours_scratch, hierarchy_scratch, RETR_TREE, CHAIN_APPROX_SIMPLE);

       

        if (contours_scratch.empty())
        {
            // color = color_flawless;
        }

        else
        {
            for (size_t k = 0; k < contours_scratch.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
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
                        pt.x = contours_scratch[k].at(p).x + vChips[i].x;
                        pt.y = contours_scratch[k].at(p).y + vChips[i].y;
                        contours_scratch_error.push_back(pt);
                    }
                    contours_scratch_error_all.push_back(contours_scratch_error);
                    drawContours(drawColor, contours_scratch_error_all, 0, color_scratch, 3);

                    color = color_scratch;
                    string msg;
                    msg = to_string(vErrRegions.size() + 1);
                    cv::putText(drawColor, msg, vChips[i].br(), FONT_HERSHEY_SIMPLEX, 1, color, 2, 8);      //0, 255, 255
                    cv::putText(drawColor, _InspectName, vChips[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
                    //cv::rectangle(drawColor, vChips[i], color, CV_FILLED);

                    DefectInfo defectInfo;
                    defectInfo.rt = vChips[i];
                    defectInfo.Judge = eJudge_Scratch;
                    if (vErrRegions[k].Judge == eJudge_OK)
                        vErrRegions[i] = defectInfo; //crack 오류 사각형의 데이터를 저장. 


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