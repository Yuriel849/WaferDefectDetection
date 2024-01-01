#include "InspectContamination.h"


InspectContamination::InspectContamination() {}

InspectContamination::~InspectContamination() {}

int InspectContamination::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>* vRegions, int& flaw_num)
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
        RotatedRect rrt = minAreaRect(contours[k]);
        Rect rt = rrt.boundingRect();
        
        if (13 <= rt.width && rt.width <= 17)
        {
            vRois_Small.push_back(rt);
            cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 255, 0), 1);
        }
        if (18 <= rt.width && rt.width <= 22)
        {
            vRois_Large.push_back(rt);
            cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 0, 255), 1);
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

    int a = 0;

    // Identify the large and small squares in each chip across the wafer under inspection
    vector<cv::Rect> vRois; //��ü �̹��� ���� �迭�� ����
    Mat obj_Region;
    Point ptThrehold = Point(888, 150); //����ȭ�� ��� ������ ���� ��� ���̸� �ֱ� ���� ��� ��ġ �� �����ͷ� ���� 
    double min_threshold = src.data[ptThrehold.y * src.cols + ptThrehold.x] + 5; //�������� ���� �κ� �����Ͽ�
    cv::threshold(src, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region�� ����ȭ �۾�
    contours.clear(); //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ����
    hierarchy.clear(); //���� hierarchyy���� ����
    cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
    for (size_t k = 0; k < contours.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
    {
        double area = contourArea(contours[k]);

        RotatedRect rrt = minAreaRect(contours[k]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
        Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

        if (160 <= rt.width && rt.width <= 200)
        {
            vRois.push_back(rt);
            cv::rectangle(drawColor, rrt.boundingRect(), CV_RGB(0, 255, 255), 2); //
        }
    }

    //sub chips
    vector<cv::Rect> vRois_Small_Err, vRois_Large_Err; // ���� ��ü, ���� ��ü ���� ������ ���� ���� ����
    vector<int> remember;
    for (size_t k = 4; k < vRois.size(); k++) // ��ü �̹��� ��üvRois.size()�� ������ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
    {
        Mat sub_img = src(vRois[k]).clone(); // vRois[k]��° ��ü sub_img�� ���� 
        Mat sub_img_draw;
        cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR);

        //large pad
        int inflate = 7; //��ü rect �¿� ��ġ ������ ���̱� ���� 7��ŭ �ű�� ���� ���� ����
        for (size_t i = 0; i < vRois_Large.size(); i++) // ���� ���� ��ü �����ŭ �ݺ�
        {
            Rect rt = vRois_Large[i];
            rt.x -= inflate;
            rt.width += inflate * 2;
            rt.height += inflate + 4;
            //     
            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1);

            Mat Pad = sub_img(rt).clone();
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

            int kernelSz = 2;
            int shape = MorphShapes::MORPH_RECT;
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1);
            Mat SE = cv::getStructuringElement(shape, sz);
            Mat Pad_Open;
            int type = MorphTypes::MORPH_OPEN;
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);

            contours.clear();
            hierarchy.clear();
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
            for (size_t p = 0; p < contours.size(); p++)
            {
                double area = contourArea(contours[p]);
                if (area < 10) continue;
                RotatedRect rrt = minAreaRect(contours[p]);
                Rect rt = rrt.boundingRect();

                if (((avg_Width_Large * 0.9) <= rt.width && rt.width <= (avg_Width_Large * 1.1))
                    && ((avg_Height_Large * 0.9) <= rt.height && rt.height <= (avg_Height_Large * 1.1))) {}
                else
                {
                    vRegions->push_back(vRois[k]);
                    cnt_Large_Err++;
                    Rect rtSubErrRgn = rt;
                    rtSubErrRgn.x += vRois_Large[i].x - inflate;
                    rtSubErrRgn.y += vRois_Large[i].y;
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1);

                    Rect rtDraw = rt;
                    rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate;
                    rtDraw.y += vRois_Large[i].y + vRois[k].y;
                    vRois_Large_Err.push_back(rtDraw);
                    remember.push_back(k);
                }
            }
        }
        //small pad   
        for (size_t i = 0; i < vRois_Small.size(); i++) //small pads �ϳ��� ���鼭 ����ȭ, ���� Ȯ��
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

            //morpology ������ ���� �κ�
            int kernelSz = 2; //������ ũ��
            int shape = MorphShapes::MORPH_RECT; //
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //��lSz + 1) 
            Mat SE = cv::getStructuringElement(shape, sz); // 
            Mat Pad_Open; //����� ���ŵ� ����.
            int type = MorphTypes::MORPH_OPEN; // ����� �����ϴ� ��� MORPH_OPEN�� type�� ��ڴ�.
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);// cv::morphologyEx(src_bin(�Է�), src_open(���), type, SE);//morphologyEx ����� �����ϰڴ�. 

            contours.clear(); //
            hierarchy.clear(); //
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
            for (size_t p = 0; p < contours.size(); p++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
            {
                double area = contourArea(contours[p]);
                if (area < 10) continue;
                RotatedRect rrt = minAreaRect(contours[p]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� rrt�� �����
                Rect rt = rrt.boundingRect(); // rt ��ü�� ��ġ�� ������� ������ �簢�� ����

                if (((avg_Width_Small * 0.8) <= rt.width && rt.width <= (avg_Width_Small * 1.2))
                    && ((avg_Height_Small * 0.8) <= rt.height && rt.height <= (avg_Height_Small * 1.2))) {}
                else
                {
                    vRegions->push_back(vRois[k]);
                    Rect rtSubErrRgn = rt;
                    rtSubErrRgn.x += vRois_Small[i].x - inflate;
                    rtSubErrRgn.y += vRois_Small[i].y;
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 123), 2); //

                    // cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //
                    Rect rtDraw = rt;//chips
                    rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate; //rtDraw.x�� vRois[k].x��ü ũ�� �߰� ��ü ���� ��ǥ�� ���ϰ� ���ΰ�ü x��ǥ vRois_Small[i].x�� ��� ���������� �ش� ���ΰ�ü x��ǥ�� ���������� ����Ű�� ��.
                    rtDraw.y += vRois_Small[i].y + vRois[k].y;
                    vRois_Small_Err.push_back(rtDraw);
                    cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //
                    //��ĥ�� ĭ ����ϱ� 
                    remember.push_back(k);
                }
            }
        }
        if (remember.size() > 0)
        {
            //��ĥ�� ĭ ����ϱ� 
            remeber_con.push_back(vRois[k]);
            remember.clear();
        }
    }

    //display result :: error rectangle
    for (size_t i = 0; i < remeber_con.size(); i++)
    {
        string msg;
        msg = to_string(con_num + 1);
        // �簢�� ������ �ȼ� ���� Ȯ��
        cv::Scalar currentPixelColor = drawColor.at<cv::Vec3b>(remeber_con[i].y + remeber_con[i].height / 2,
            remeber_con[i].x + remeber_con[i].width / 2);
        if (currentPixelColor == CV_RGB(255, 0, 0) || currentPixelColor == CV_RGB(255, 255, 0))
        {
            // int check112 = 0;
        }
        else
        {
            Scalar color = CV_RGB(255, 0, 255);
            con_num += 1;
            cv::putText(drawColor, msg, remeber_con[i].br(), FONT_HERSHEY_SIMPLEX, 1, color, 2, 8);		//0, 255, 255
            cv::putText(drawColor, contamination, remeber_con[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
            cv::rectangle(drawColor, remeber_con[i], color, CV_FILLED);
            flaw_num += 1;      // ���� ���� ���� 
        }
    }
    return 0;
}