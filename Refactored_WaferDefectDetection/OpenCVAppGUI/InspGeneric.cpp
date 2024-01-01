#include "InspGeneric.h"

InspGeneric::InspGeneric()
{
}

InspGeneric::~InspGeneric()
{
}

int InspGeneric::OnPreprocess(const Mat& src)
{
    vLargePadsInChip.clear();
    vSmallPadsInChip.clear();
    vChips.clear();

    vector<Rect> remeber_con;

    Mat ptrn = cv::imread(PATTERN, cv::ImreadModes::IMREAD_GRAYSCALE);

    double obj_PAD_threshold = 130;
    Mat ptrn_Pads_Region;    
    cv::threshold(ptrn, ptrn_Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    cv::findContours(ptrn_Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    
    for (size_t k = 0; k < contours.size(); k++)
    {
        RotatedRect rrt = minAreaRect(contours[k]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
        Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����
        //�� ���簢�� ���� ���� Ư�� ���� ����
        if (13 <= rt.width && rt.width <= 17)//��ü �� small ������ rt���� 13�̻� 17������ ���
        {

            vSmallPadsInChip.push_back(rt); //�� ����Small ��ü ���
            //cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //rrt.boundingRect()������ ���� �簢�� ���� �簢���� �׷���
        }
        if (18 <= rt.width && rt.width <= 22)//��ü �� small ������ rt���� 18�̻� 22������ ���
        {

            vLargePadsInChip.push_back(rt); //�� Large���� ��ü ���
            //cv::rectangle(ptrn_Draw, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //rrt.boundingRect()������ ���� �簢�� ���� �簢���� �׷���
        }
    }

    // Identify the large and small squares in each chip across the wafer under inspection
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
            vChips.push_back(rt);
            //cv::rectangle(drawColor, rrt.boundingRect(), CV_RGB(0, 255, 255), 2); //
        }
    }





    // Calculate average widths and heights of the large and small squares in each chip
    int sum_Width_Large = 0, sum_Height_Large = 0, sum_Width_Small = 0, sum_Height_Small = 0;
    for (size_t k = 0; k < vSmallPadsInChip.size(); k++)
    {
        sum_Width_Small += vSmallPadsInChip[k].width;
        sum_Height_Small += vSmallPadsInChip[k].height;
    }
    avg_Width_Small = static_cast<float>(sum_Width_Small) / vSmallPadsInChip.size();
    avg_Height_Small = static_cast<float>(sum_Height_Small) / vSmallPadsInChip.size();

    for (size_t k = 0; k < vLargePadsInChip.size(); k++)
    {
        sum_Width_Large += vLargePadsInChip[k].width;
        sum_Height_Large += vLargePadsInChip[k].height;
    }
    avg_Width_Large = static_cast<float>(sum_Width_Large) / vLargePadsInChip.size();
    avg_Height_Large = static_cast<float>(sum_Height_Large) / vLargePadsInChip.size();
    return 0;
}

int InspGeneric::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num)
{
    return 0;
}

int InspGeneric::OnPostprocess(const Mat& src)
{
    string msg1 = "Yield = ";
    string msg2;
    string msg3 = "%";
    size_t allChips = vChips.size();
    size_t flaw_num = vErrRegions.size();
    int yield = ((allChips - flaw_num) / allChips) * 100;
    cout << allChips - flaw_num << endl;
    cout << (allChips - flaw_num) / allChips << endl;
    msg2 = to_string(yield);

    putText(drawImg, msg1 + msg2 + msg3, cv::Point(50, 1625), FONT_HERSHEY_SIMPLEX, 2.5, Scalar(0, 0, 0), 3);

    return 0;
}

