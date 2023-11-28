#include "Common.h"

void main()
{

	std::string filePath_Ptrn = "./res/img/wafer_template.png"; //타깃 이미지
	cv::Mat src_gray_ptrn = cv::imread(filePath_Ptrn, cv::ImreadModes::IMREAD_GRAYSCALE); //타깃 이미지 흑백으로 로딩
	Mat draw_Pads;
	cvtColor(src_gray_ptrn, draw_Pads, COLOR_GRAY2BGR);

	Mat Pads_Region;
	double obj_PAD_threshold = 130;
	cv::threshold(src_gray_ptrn, Pads_Region, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);
	vector<vector<Point>> contours; //새롭게 범위값을 구하기 위해 백터 포인트 변수 contourss 생성 
	vector<Vec4i> hierarchy; //백터 hierarchyy변수 생성
	cv::findContours(Pads_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출

	vector<cv::Rect> vRois_Small, vRois_Large;
	for (size_t k = 0; k < contours.size(); k++)
	{
		RotatedRect rrt = minAreaRect(contours[k]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
		Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

		if (13 <= rt.width && rt.width <= 17)//small
		{

			vRois_Small.push_back(rt);
			cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
		}
		if (18 <= rt.width && rt.width <= 22)//large
		{

			vRois_Large.push_back(rt);
			cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
		}
	}




	std::string filePath_Search = "./res/img/edge_location_black.png"; //원본이미지

	cv::Mat src_draw = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_ANYCOLOR); //원본이미지 컬러로 로딩
	cv::Mat src_gray_search = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE); //원본이미지 흑백으로 로딩

	//find objects
	vector<cv::Rect> vRois;
	Mat obj_Region;
	Point ptThrehold = Point(888, 150);
	double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5;
	cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);
	contours.clear(); //새롭게 범위값을 구하기 위해 백터 포인트 변수 contourss 생성 
	hierarchy.clear(); //백터 hierarchyy변수 생성
	cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
	for (size_t k = 0; k < contours.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
	{
		double area = contourArea(contours[k]);

		RotatedRect rrt = minAreaRect(contours[k]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
		Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

		if (160 <= rt.width && rt.width <= 180)
		{

			vRois.push_back(rt);
			cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
		}
	}


	//sub chips
	vector<cv::Rect> vRois_Small_Err, vRois_Large_Err;
	for (size_t k = 0; k < vRois.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
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

			cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과

			Mat Pad = sub_img(rt).clone();
			Mat Pad_Bin;
			threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

			contours.clear(); //새롭게 범위값을 구하기 위해 백터 포인트 변수 contourss 생성 
			hierarchy.clear(); //백터 hierarchyy변수 생성
			cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
			for (size_t p = 0; p < contours.size(); p++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
			{
				double area = contourArea(contours[p]);
				if (area < 10) continue;
				RotatedRect rrt = minAreaRect(contours[p]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
				Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

				if (18 <= rt.width && rt.width <= 22)//large
				{

				}
				else
				{
					Rect rtSubErrRgn = rt;
					rtSubErrRgn.x += vRois_Large[i].x - inflate;
					rtSubErrRgn.y += vRois_Large[i].y;
					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과

					Rect rtDraw = rt;
					rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate;
					rtDraw.y += vRois_Large[i].y + vRois[k].y;
					vRois_Large_Err.push_back(rtDraw);
					cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
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

			cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과

			Mat Pad = sub_img(rt).clone();
			Mat Pad_Bin;
			threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

			contours.clear(); //새롭게 범위값을 구하기 위해 백터 포인트 변수 contourss 생성 
			hierarchy.clear(); //백터 hierarchyy변수 생성
			cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
			for (size_t p = 0; p < contours.size(); p++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
			{
				double area = contourArea(contours[p]);
				if (area < 10) continue;
				RotatedRect rrt = minAreaRect(contours[p]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
				Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

				if (13 <= rt.width && rt.width <= 17)//small
				{
				}
				else {
					Rect rtSubErrRgn = rt;
					rtSubErrRgn.x += vRois_Small[i].x - inflate;
					rtSubErrRgn.y += vRois_Small[i].y;
					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과


					cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
					Rect rtDraw = rt;//chips
					rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate;
					rtDraw.y += vRois_Small[i].y + vRois[k].y;
					vRois_Small_Err.push_back(rtDraw);
					//cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
				}
			}

		}
	}



	//display result :: error rectangle
	for (size_t i = 0; i < vRois_Large_Err.size(); i++)
	{
		cv::rectangle(src_draw, vRois_Large_Err[i], CV_RGB(255, 0, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
	}
	for (size_t i = 0; i < vRois_Small_Err.size(); i++)
	{
		cv::rectangle(src_draw, vRois_Small_Err[i], CV_RGB(255, 0, 255), 1); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과
	}
}