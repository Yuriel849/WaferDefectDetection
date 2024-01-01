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
    for (size_t k = 0; k < vChips.size(); k++) // ��ü �̹��� ��üvRois.size()�� ������ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
    {
        Mat sub_img = src(vChips[k]).clone(); // vRois[k]��° ��ü sub_img�� ���� 
        Mat sub_img_draw;
        cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR);

        //int cnt_Large = 0, cnt_Small = 0, cnt_Large_Err = 0, cnt_Small_Err = 0;

        //large pad
        int inflate = 7; //��ü rect �¿� ��ġ ������ ���̱� ���� 7��ŭ �ű�� ���� ���� ����
        for (size_t i = 0; i < vRois_Large.size(); i++) // ���� ���� ��ü �����ŭ �ݺ�
        {
            Rect rt = vRois_Large[i]; // ���� ������ü i��° rt�� ����
            rt.x -= inflate; // ���� ������ü i��°�� x��ġ���� 7��ŭ ���� �·� �̵� (������ü x ��ǥ�� -7��ŭ �������� x�� �̵�)
            rt.width += inflate * 2; // ���λ����� ����Ʈ 5�� 2��� �÷� ������ ����ش�.//���� ���� 7*2=14��ŭ rt.width���� �����Ͽ� ���� ����
            rt.height += inflate + 4;// ���λ����� ����Ʈ 5�� 2��� �÷� ������ ����ش�.rt.height += inflate * 2(��ũ��ġ���� ����༭  + 4�� ũ�� ����
            //     
            cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //sub_img_draw�� rt��ü�� ������ŭ �簢�� �׷���

            Mat Pad = sub_img(rt).clone(); // ((���� ��ü �簢�� ������ ���� Pad�� ����))
            Mat Pad_Bin;
            cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY); //����ȭ

            //morpology ������ ���� �κ�
            int kernelSz = 2; //������ ũ��
            int shape = MorphShapes::MORPH_RECT; //
            cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //��lSz + 1) 
            Mat SE = cv::getStructuringElement(shape, sz); // 
            Mat Pad_Open; //����� ���ŵ� ����.
            int type = MorphTypes::MORPH_OPEN; // ����� �����ϴ� ��� MORPH_OPEN�� type�� ��ڴ�.
            cv::morphologyEx(Pad_Bin, Pad_Open, type, SE);// cv::morphologyEx(src_bin(�Է�), src_open(���), type, SE);//morphologyEx ����� �����ϰڴ�. 

            contours.clear(); //clear�� �Ἥ ���Ұ� �ʱ�ȭ
            hierarchy.clear(); //clear�� �Ἥ ���Ұ� �ʱ�ȭ
            cv::findContours(Pad_Open, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Pad_Bin�� �� ��ü �ܰ��� ����
            for (size_t p = 0; p < contours.size(); p++) //Pad_Bin�� �� ��ü ����ŭ for���� ������. 
            {
                double area = contourArea(contours[p]); //����� ��ü ��ü ���� �� area�� ����
                if (area < 10) continue; //area�� 10�� ���� ������ ����
                RotatedRect rrt = minAreaRect(contours[p]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� rrt�� �����
                Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� �����Ͽ� rt�� �����

                //large ������ ���� ���� ���� ����  //rt ����� ��ü ���� 18 <= rt.width && rt.width <= 22
                if (((avg_Width_Large * 0.9) <= rt.width && rt.width <= (avg_Width_Large * 1.1))
                    && ((avg_Height_Large * 0.9) <= rt.height && rt.height <= (avg_Height_Large * 1.1)))
                {
                    //cnt_Large++;
                }
                else
                {
                    //vErrRegions.push_back(vChips[k]);
                    //cnt_Large_Err++;
                    Rect rtSubErrRgn = rt; //���ǿ�  �������� ���� rt�� rtSubErrRgn�� ����
                    rtSubErrRgn.x += vRois_Large[i].x - inflate; // x�� ��ġ�� -7��ŭ ��ġ ����
                    rtSubErrRgn.y += vRois_Large[i].y; //y�� ��ġ�� �״��
                    cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //��ġ�� ���� �簢�� �׷���

                    Rect rtDraw = rt;
                    rtDraw.x += vRois_Large[i].x + vChips[k].x - inflate; //rtDraw.x�� vRois[k].x��ü ũ�� �߰� ��ü ���� ��ǥ�� ���ϰ� ���ΰ�ü x��ǥ vRois_Small[i].x�� ��� ���������� �ش� ���ΰ�ü x��ǥ�� ���������� ����Ű�� ��.
                    rtDraw.y += vRois_Large[i].y + vChips[k].y; //rtDraw.y�� vRois[k].y��ü ũ�� �߰� ��ü ���� ��ǥ�� ���ϰ� ���ΰ�ü y��ǥ vRois_Small[i].y�� ��� ���������� �ش� ���ΰ�ü y��ǥ�� ���������� ����Ű�� ��.

                    DefectInfo defectInfo;
                    defectInfo.rt = rtDraw;
                    defectInfo.Judge = eJudge_Contamination;
                    vRois_Large_Err.push_back(defectInfo); // ���������� ����Ų ��ǥ 
                    if (vErrRegions[k].Judge == eJudge_OK)
                    {
                        vErrRegions[k] = defectInfo;
                        //cv::rectangle(drawColor, vChips[k], CV_RGB(0, 255, 125), FILLED); //
                        cv::rectangle(drawColor, rtDraw, CV_RGB(0, 255, 125), FILLED); //
                    }
                    //cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //
                    
                    //��ĥ�� ĭ ����ϱ� 
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
                    rtDraw.x += vRois_Small[i].x + vChips[k].x - inflate; //rtDraw.x�� vRois[k].x��ü ũ�� �߰� ��ü ���� ��ǥ�� ���ϰ� ���ΰ�ü x��ǥ vRois_Small[i].x�� ��� ���������� �ش� ���ΰ�ü x��ǥ�� ���������� ����Ű�� ��.
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


                    //��ĥ�� ĭ ����ϱ� 
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
    //    // �簢�� ������ �ȼ� ���� Ȯ��
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
    //        flaw_num += 1;      // ���� ���� ���� 
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