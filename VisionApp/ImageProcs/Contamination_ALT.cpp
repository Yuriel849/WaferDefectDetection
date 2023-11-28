#include "Common.h"

void main()
{

	std::string filePath_Ptrn = "./res/img/wafer_template.png"; //Ÿ�� �̹���
	cv::Mat src_gray_ptrn = cv::imread(filePath_Ptrn, cv::ImreadModes::IMREAD_GRAYSCALE); //Ÿ�� �̹��� ������� �ε�
	Mat draw_Pads;
	cvtColor(src_gray_ptrn, draw_Pads, COLOR_GRAY2BGR);

	Mat Pads_Region;
	double obj_PAD_threshold = 130;
	cv::threshold(src_gray_ptrn, Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);
	vector<vector<Point>> contours; //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ���� 
	vector<Vec4i> hierarchy; //���� hierarchyy���� ����
	cv::findContours(Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����

	vector<cv::Rect> vRois_Small, vRois_Large;
	for (size_t k = 0; k < contours.size(); k++)
	{
		RotatedRect rrt = minAreaRect(contours[k]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
		Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

		if (13 <= rt.width && rt.width <= 17)//small
		{

			vRois_Small.push_back(rt);
			cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
		}
		if (18 <= rt.width && rt.width <= 22)//large
		{

			vRois_Large.push_back(rt);
			cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
		}
	}




	std::string filePath_Search = "./res/img/edge_location_black.png"; //�����̹���

	cv::Mat src_draw = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_ANYCOLOR); //�����̹��� �÷��� �ε�
	cv::Mat src_gray_search = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE); //�����̹��� ������� �ε�

	//find objects
	vector<cv::Rect> vRois;
	Mat obj_Region;
	Point ptThrehold = Point(888, 150);
	double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5;
	cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);
	contours.clear(); //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ���� 
	hierarchy.clear(); //���� hierarchyy���� ����
	cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
	for (size_t k = 0; k < contours.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
	{
		double area = contourArea(contours[k]);

		RotatedRect rrt = minAreaRect(contours[k]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
		Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

		if (160 <= rt.width && rt.width <= 180)
		{

			vRois.push_back(rt);
			cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
		}
	}


	//sub chips
	vector<cv::Rect> vRois_Small_Err, vRois_Large_Err;
	for (size_t k = 0; k < vRois.size(); k++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
	{
		Mat sub_img = src_gray_search(vRois[k]).clone();
		Mat sub_img_draw;
		cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR);
		//large pad
		int inflate = 7;
		for (size_t i = 0; i < vRois_Large.size(); i++)
		{
			Rect rt = vRois_Large[i];
			rt.x -= inflate;
			rt.width += inflate * 2;
			rt.height += inflate * 2;

			cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������

			Mat Pad = sub_img(rt).clone();
			Mat Pad_Bin;
			threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

			contours.clear(); //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ���� 
			hierarchy.clear(); //���� hierarchyy���� ����
			cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
			for (size_t p = 0; p < contours.size(); p++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
			{
				double area = contourArea(contours[p]);
				if (area < 10) continue;
				RotatedRect rrt = minAreaRect(contours[p]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
				Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

				if (18 <= rt.width && rt.width <= 22)//large
				{

				}
				else
				{
					Rect rtSubErrRgn = rt;
					rtSubErrRgn.x += vRois_Large[i].x - inflate;
					rtSubErrRgn.y += vRois_Large[i].y;
					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������

					Rect rtDraw = rt;
					rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate;
					rtDraw.y += vRois_Large[i].y + vRois[k].y;
					vRois_Large_Err.push_back(rtDraw);
					cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
					int a = 0;
				}
			}
		}
		//small pad	
		for (size_t i = 0; i < vRois_Small.size(); i++)
		{
			Rect rt = vRois_Small[i];
			rt.x -= inflate;
			rt.y -= inflate;
			rt.width += inflate * 2;
			rt.height += inflate + 2;

			cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������

			Mat Pad = sub_img(rt).clone();
			Mat Pad_Bin;
			threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

			contours.clear(); //���Ӱ� �������� ���ϱ� ���� ���� ����Ʈ ���� contourss ���� 
			hierarchy.clear(); //���� hierarchyy���� ����
			cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //��ü �������� ���ϱ� ���� ��ü �ܰ��� ����
			for (size_t p = 0; p < contours.size(); p++) //contourss.size();�� ũ�⸸ŭ for���� ������.(���Ӱ� ���ǵ� ��ü ������(��) 
			{
				double area = contourArea(contours[p]);
				if (area < 10) continue;
				RotatedRect rrt = minAreaRect(contours[p]);//��ü �ܰ����� ���� ���� ��ŭ �� ����� wRrt�� �����
				Rect rt = rrt.boundingRect(); // ��ü�� ��ġ�� ������� ������ �簢�� ����

				if (13 <= rt.width && rt.width <= 17)//small
				{
				}
				else {
					Rect rtSubErrRgn = rt;
					rtSubErrRgn.x += vRois_Small[i].x - inflate;
					rtSubErrRgn.y += vRois_Small[i].y;
					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������


					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
					Rect rtDraw = rt;//chips
					rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate;
					rtDraw.y += vRois_Small[i].y + vRois[k].y;
					vRois_Small_Err.push_back(rtDraw);
					//cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
				}
			}

		}
	}



	//display result :: error rectangle
	for (size_t i = 0; i < vRois_Large_Err.size(); i++)
	{
		cv::rectangle(src_draw, vRois_Large_Err[i], CV_RGB(255, 0, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
	}
	for (size_t i = 0; i < vRois_Small_Err.size(); i++)
	{
		cv::rectangle(src_draw, vRois_Small_Err[i], CV_RGB(255, 0, 255), 1); //src_openn���� ã�Ƴ� ��ü�� �ܰ��� �κ��� wR2f.tl() �»� ��������
	}
}