#include "InspectCrack.h"



InspectCrack::InspectCrack()
{
	_InspectName = "Crack";
}

InspectCrack::~InspectCrack()
{
}

int InspectCrack::OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num)
{
	// src			 Ž���� �̹��� IMREAD_GRAYSCALE
	// drawColor	 �簢�� ä�� �̹���
	// vRegions      �� ä�� �̹��� IMREAD_ANYCOLOR

	//////Identify the large and small squares in each chip across the wafer under inspection///////////
	//vector<cv::Rect> vRois; //��ü �̹��� ���� �迭�� ����
	//vector<vector<Point>> contours;
	//vector<Vec4i> hierarchy;
	//Mat obj_Region;
	//Point ptThrehold = Point(888, 150); //����ȭ�� ��� ������ ���� ��� ���̸� �ֱ� ���� ��� ��ġ �� �����ͷ� ���� 
	//double min_threshold = src.data[ptThrehold.y * src.cols + ptThrehold.x] + 5; //�������� ���� �κ� �����Ͽ�
	//cv::threshold(src, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region�� ����ȭ �۾�
	//contours.clear(); //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ����
	//hierarchy.clear(); //���� hierarchy���� ����
	//cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
	//for (size_t k = 0; k < contours.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
	//{
	//	double area = contourArea(contours[k]);

	//	RotatedRect rrt = minAreaRect(contours[k]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
	//	Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

	//	if (160 <= rt.width && rt.width <= 200)
	//	{
	//		vRois.push_back(rt);
	//		cv::rectangle(drawColor, rrt.boundingRect(), CV_RGB(0, 255, 0), 2); //
	//	}
	//}
	/////////////////////////////////////////////////////////////////////////////////////////////////

	RNG rng(12345);
	Scalar color_crack(0, 255, 255);		// 255, 0, 0

	for (size_t i = 0; i < vChips.size(); i++)
	{
		Scalar color;
		Mat binCrack;
		Mat subImgCrack = src(vChips[i]).clone();

		cv::Point ptThreshold(885, 150);
		cv::drawMarker(drawColor, ptThreshold, CV_RGB(255, 255, 255), MarkerTypes::MARKER_CROSS);
		double thres_crack = src.at<uchar>(ptThreshold.y, ptThreshold.x) + 9;					//////////////////// 61
		cv::threshold(subImgCrack, binCrack, thres_crack, 255, ThresholdTypes::THRESH_BINARY_INV);	//INV -> 43���� ��ο� �� + 61 

		//morpology ������ ���� �κ�
		int kernelSz = 2; //������ ũ��
		int shape = MorphShapes::MORPH_RECT; //
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1); //��lSz + 1) 
		Mat SE = cv::getStructuringElement(shape, sz); // 
		Mat src_open; //����� ���ŵ� ����.
		int type = MorphTypes::MORPH_OPEN; // ����� �����ϴ� ��� MORPH_OPEN�� type�� ��ڴ�.
		cv::morphologyEx(binCrack, src_open, type, SE);// cv::morphologyEx(src_bin(�Է�), src_open(���), type, SE);//morphologyEx ����� �����ϰڴ�. 

		cv::findContours(src_open, contours_crack, hierarchy_crack, RETR_TREE, CHAIN_APPROX_SIMPLE);

		string msg;
		msg = to_string(crack_num + 1);

		if (contours_crack.empty())
		{
			// color = color_flawless;
		}

		else
		{
			for (size_t k = 0; k < contours_crack.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
			{
				double area = contourArea(contours_crack[k]);

				double area_min = 10 * 10;
				if (area_min <= area)
				{
					vector<vector<Point>> contours_crack_error_all;
					vector<Point> contours_crack_error;
					for (size_t p = 0; p < contours_crack[k].size(); p++)
					{
						Point pt;
						pt.x = contours_crack[k].at(p).x + vChips[i].x;
						pt.y = contours_crack[k].at(p).y + vChips[i].y;
						contours_crack_error.push_back(pt);
					}
					contours_crack_error_all.push_back(contours_crack_error);
					drawContours(drawColor, contours_crack_error_all, 0, color_crack, 3);

					color = color_crack;
					crack_num += 1;
					cv::putText(drawColor, msg, vChips[i].br(), FONT_HERSHEY_SIMPLEX, 1, color, 2, 8);		//0, 255, 255
					cv::putText(drawColor, _InspectName, vChips[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
					//cv::rectangle(drawColor, vChips[i], color, CV_FILLED);

					DefectInfo defectInfo;
					defectInfo.rt = vChips[i];
					defectInfo.Judge = eJudge_Crack;
					if (vErrRegions[i].Judge == eJudge_OK)
						vErrRegions[i] = defectInfo; //crack ���� �簢���� �����͸� ����. 

					/*int wid = (*vRegions)[i].width; : contamination ������ �Ÿ��� ���� chip���� ���� ���� ���� ������
					int he = (*vRegions)[i].height;
					int checking = 0;*/

					/*cv::Point rtPts[4] = { vRegions[i].tl(), Point(vRegions[i].tl().x + vRegions[i].width, vRegions[i].tl().y),
					vRegions[i].br(), Point(vRegions[i].tl().x, vRegions[i].br().y) };
					for (size_t c = 0; c < 4; c++)
					{
						draw_line(drawColor, rtPts[c % 4], rtPts[(c + 1) % 4], color, 2, "", 3);
					}*/
					flaw_num += 1;      // ���� ���� ���� 
				}
			}
		}
	}

	return 0;
}

void InspectCrack::draw_line(cv::Mat& img, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness, std::string style, int gap)
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