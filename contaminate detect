#include "Common.h"

//원본 이미지와 타깃이미지를 매칭하여 비교하게 하는 기능
void getROIs(const Mat& serch_img, const Mat& ptrn_img, const double& thres, vector<Rect>& rois)
{
	uchar* pSearch = serch_img.data;
	int search_w = serch_img.cols;
	int search_h = serch_img.rows;
	uchar* pPtrn = ptrn_img.data;
	int ptrn_w = ptrn_img.cols;
	int ptrn_h = ptrn_img.rows;

	//Mat tm_img = Mat::zeros(Size(search_w - ptrn_w + 1, search_h - ptrn_h + 1), CV_32FC1);
	//for (size_t row = 0; row < search_h - ptrn_h + 1; row++)
	//{
	//	for (size_t col = 0; col < search_w - ptrn_w + 1; col++)
	//	{
	//		double TM_SQDIFF = 0.0;
	//		double TM_SQDIFF_NORMED = 0.0;
	//		for (size_t prow = 0; prow < ptrn_h; prow++)
	//		{
	//			for (size_t pcol = 0; pcol < ptrn_w; pcol++)
	//			{
	//				int search_index = (row + prow) * search_w + (col + pcol);
	//				int ptrn_index = prow * ptrn_w + pcol;

	//				double diff = pSearch[search_index] - pPtrn[ptrn_index];
	//				TM_SQDIFF += (diff * diff);
	//			}
	//		}
	//		double ptrnSQ = 0., searchSQ = 0.;
	//		for (size_t prow = 0; prow < ptrn_h; prow++)
	//		{
	//			for (size_t pcol = 0; pcol < ptrn_w; pcol++)
	//			{
	//				int search_index = (row + prow) * search_w + (col + pcol);
	//				int ptrn_index = prow * ptrn_w + pcol;
	//				searchSQ += pSearch[search_index] * pSearch[search_index];
	//				ptrnSQ += pPtrn[ptrn_index] * pPtrn[ptrn_index];
	//			}
	//		}

	//		//matching_img.at<double>(row, col) = TM_SQDIFF;
	//		//if (TM_SQDIFF == 0)
	//		//	ptFind.push_back(Point(col, row));

	//		if (ptrnSQ == 0) ptrnSQ = 1;
	//		TM_SQDIFF_NORMED = TM_SQDIFF / sqrt(ptrnSQ * searchSQ);
	//		tm_img.at<float>(row, col) = TM_SQDIFF_NORMED;
	//		//if (TM_SQDIFF_NORMED <= 0.009)
	//		//	rois.push_back(Rect(col, row, ptrn_w, ptrn_h));
	//	}
	//}
	//바로 아래코드 result를 비워내는 작업
	Mat result = Mat::zeros(Size(search_w - ptrn_w + 1, search_h - ptrn_h + 1), CV_32FC1);
	cv::matchTemplate(serch_img, ptrn_img, result, TemplateMatchModes::TM_CCOEFF_NORMED); //serch_img와 ptrn_img를 매칭하여 result에 담아주겠다.
	Mat tm_bin;//tm_bin 빈 파일 이미지 생성
	double min_thr = thres; // 중간색감값 min_thr에 삽입
	double max_thr = 1.0; // 가장 큰 색상값 1.0으로 통일
	cv::threshold(result, tm_bin, min_thr, max_thr, ThresholdTypes::THRESH_BINARY);//이진화 하는 함수
	tm_bin *= 255; //큰 값을 255로 통일 

	tm_bin.convertTo(tm_bin, CV_8UC1); // 흑백을 흑백 이미지로 바꾸는 것.

	vector<vector<Point>> contours; //contours 값을 받기위해 백터 포인터 파일 형성
	vector<Vec4i> hierarchy; //백터 hierarchy 파일 생성
	cv::findContours(tm_bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체들의 외곽선을 검출하는 함수
	//tm_bin에 contours, hierarchy의 RETR_TREE모든 외곽선을 검출하고, CHAIN_APPROX_SIMPLE 외곽선 중에서 수평선, 대각선 성분은 끝점만 저장

	for (size_t i = 0; i < contours.size(); i++)//contours.size만큼 i번 반복
	{
		RotatedRect rrt = minAreaRect(contours[i]); //contours[i]번째 전체 외곽사이즈에서 최소외곽사이즈 정보를 rrt에 저장
		//rois.push_back(rrt.boundingRect());
		rois.push_back(Rect(rrt.boundingRect().tl().x, rrt.boundingRect().tl().y, ptrn_w, ptrn_h));
		//push_back백터에 넣겠다. rrt.boundingRect().tl().x 끝점 x/ rrt.boundingRect().tl().y  의 끝점 y/ ptrn_w 넓이,ptrn_h높이의 정보로
	} //(Rect(rrt // 사각형을 그려주겠다.
}


void main()
{

	{

		std::string filePath_Ptrn = "../KCCImageNet/wafer/wafer_template.png"; //타깃 이미지
		cv::Mat src_gray_ptrn = cv::imread(filePath_Ptrn, cv::ImreadModes::IMREAD_GRAYSCALE); //타깃 이미지 흑백으로 로딩
		Mat draw_Pads;
		cvtColor(src_gray_ptrn, draw_Pads, COLOR_GRAY2BGR); //draw_Pads

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
			//위 정사각형 생성 이후 특정 길이 추출
			if (13 <= rt.width && rt.width <= 17)//객체 각 small 사이즈 rt길이 13이상 17이하인 경우
			{

				vRois_Small.push_back(rt); //각 세부Small 객체 출력
				cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 255, 0), 1); //rrt.boundingRect()위에서 구한 사각형 값에 사각형을 그려줌
			}
			if (18 <= rt.width && rt.width <= 22)//객체 각 small 사이즈 rt길이 18이상 22이하인 경우
			{

				vRois_Large.push_back(rt); //각 Large세부 객체 출력
				cv::rectangle(draw_Pads, rrt.boundingRect(), CV_RGB(0, 0, 255), 1); //rrt.boundingRect()위에서 구한 사각형 값에 사각형을 그려줌
			}
		}




		//std::string filePath_Search = "../KCCImageNet/wafer/donut_black.png"; //원본이미지
		std::string filePath_Search = "../KCCImageNet/wafer/Donut_navy.png"; //원본이미지

		cv::Mat src_draw = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_ANYCOLOR); //원본이미지 컬러로 로딩
		cv::Mat src_gray_search = cv::imread(filePath_Search, cv::ImreadModes::IMREAD_GRAYSCALE); //원본이미지 흑백으로 로딩

		//find objects
		vector<cv::Rect> vRois; //전체 이미지 백터 배열로 저장
		Mat obj_Region;
		Point ptThrehold = Point(888, 150); //이진화된 배경 색보다 밝을 경우 차이를 주기 위해 배경 위치 값 포인터로 설정 
		double min_threshold = src_gray_search.data[ptThrehold.y * src_gray_search.cols + ptThrehold.x] + 5; //배경색보다 밝을 부분 설정하여
		cv::threshold(src_gray_search, obj_Region, min_threshold, 255, ThresholdTypes::THRESH_BINARY);// obj_Region에 이진화 작업
		contours.clear(); //새롭게 범위값을 구하기 위해 백터 포인트 변수 contourss 생성
		hierarchy.clear(); //백터 hierarchyy변수 생성
		cv::findContours(obj_Region, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
		for (size_t k = 0; k < contours.size(); k++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
		{
			double area = contourArea(contours[k]);

			RotatedRect rrt = minAreaRect(contours[k]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 wRrt에 담아줌
			Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성

			if (160 <= rt.width && rt.width <= 200)
			{

				vRois.push_back(rt);
				cv::rectangle(src_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //
			}
		}



		//sub chips
		vector<cv::Rect> vRois_Small_Err, vRois_Large_Err; // 스몰 객체, 라지 객체 검출 저장을 위한 백터 설정
		for (size_t k = 0; k < vRois.size(); k++) // 전체 이미지 객체vRois.size()의 개수만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
		{
			Mat sub_img = src_gray_search(vRois[k]).clone(); // vRois[k]번째 객체 sub_img에 삽입 
			Mat sub_img_draw;
			cvtColor(sub_img, sub_img_draw, COLOR_GRAY2BGR); //
			//large pad
			int inflate = 7; //객체 rect 좌우 위치 오차를 줄이기 위해 7만큼 옮기기 위한 변수 설정
			for (size_t i = 0; i < vRois_Large.size(); i++) // 세부 라지 객체 사이즈만큼 반복
			{
				Rect rt = vRois_Large[i]; // 세부 라지객체 i번째 rt에 담음
				rt.x -= inflate; // 세부 라지객체 i번째의 x위치에서 7만큼 빼서 좌로 이동 (스몰객체 x 좌표에 -7만큼 왼쪽으로 x축 이동)
				rt.width += inflate * 2; // 가로사이즈 디폴트 5를 2배로 늘려 윤곽을 잡아준다.//가로 길이 7*2=14만큼 rt.width길이 설정하여 범위 설정
				rt.height += inflate + 4;// 세로사이즈 디폴트 5를 2배로 늘려 윤곽을 잡아준다.rt.height += inflate * 2(스크래치까지 잡아줘서  + 4로 크기 조정
				                         //     
				cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 1); //sub_img_draw에 rt객체의 정보만큼 사각형 그려줌

				Mat Pad = sub_img(rt).clone(); // ((세부 객체 사각형 정보를 따서 Pad에 복사))
				Mat Pad_Bin;
				cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY); //이진화

				contours.clear(); //clear를 써서 새롬게 초기화
				hierarchy.clear(); //clear를 써서 새롬게 초기화
				cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //Pad_Bin의 각 객체 외곽선 검출
				for (size_t p = 0; p < contours.size(); p++) //Pad_Bin의 각 객체 수만큼 for문을 돌린다. 
				{
					double area = contourArea(contours[p]); //검출된 객체 전체 면적 값 area에 저장
					if (area < 10) continue; //area가 10을 넘지 않으면 종료
					RotatedRect rrt = minAreaRect(contours[p]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 rrt에 담아줌
					Rect rt = rrt.boundingRect(); // 객체의 위치에 상관없이 정방향 사각형 생성하여 rt에 담아줌

					if (18 <= rt.width && rt.width <= 22)//large 사이즈 일정 조건 길이 추출  //rt 검출된 객체 길이 18 <= rt.width && rt.width <= 22
					{

					}
					else
					{
						Rect rtSubErrRgn = rt; //조건에  충족하지 않은 rt값 rtSubErrRgn에 저장
						rtSubErrRgn.x += vRois_Large[i].x - inflate; // x점 위치에 -7만큼 위치 조정
						rtSubErrRgn.y += vRois_Large[i].y; //y점 위치는 그대로
						cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 1); //위치값 따서 사각형 그려줌

						Rect rtDraw = rt;
						rtDraw.x += vRois_Large[i].x + vRois[k].x - inflate; //rtDraw.x에 vRois[k].x전체 크기 중간 객체 추출 좌표를 더하고 세부객체 x좌표 vRois_Small[i].x를 담아 투과적으로 해당 세부객체 x좌표를 직접적으로 가리키게 됨.
						rtDraw.y += vRois_Large[i].y + vRois[k].y; //rtDraw.y에 vRois[k].y전체 크기 중간 객체 추출 좌표를 더하고 세부객체 y좌표 vRois_Small[i].y를 담아 투과적으로 해당 세부객체 y좌표를 직접적으로 가리키게 됨.
						vRois_Large_Err.push_back(rtDraw); // 직접적으로 가리킨 좌표 
						cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 1); //
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

				cv::rectangle(sub_img_draw, rt, CV_RGB(255, 255, 0), 2); //src_openn에서 찾아낸 객체의 외곽선 부분의 wR2f.tl() 좌상 꼭지점과

				Mat Pad = sub_img(rt).clone();
				Mat Pad_Bin;
				cv::threshold(Pad, Pad_Bin, obj_PAD_threshold, 255, ThresholdTypes::THRESH_BINARY);

				contours.clear(); //
				hierarchy.clear(); //
				cv::findContours(Pad_Bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE); //객체 범위값을 구하기 위해 객체 외곽선 검출
				for (size_t p = 0; p < contours.size(); p++) //contourss.size();의 크기만큼 for문을 돌린다.(새롭게 정의된 객체 사이즈(수) 
				{
					double area = contourArea(contours[p]);
					if (area < 10) continue;
					RotatedRect rrt = minAreaRect(contours[p]);//객체 외곽선을 넘지 않을 만큼 의 사이즈를 rrt에 담아줌
					Rect rt = rrt.boundingRect(); // rt 객체의 위치에 상관없이 정방향 사각형 생성

					if (10 <= rt.width && rt.width <= 20)//small
					{
					}
					else {
						Rect rtSubErrRgn = rt;
						rtSubErrRgn.x += vRois_Small[i].x - inflate;
						rtSubErrRgn.y += vRois_Small[i].y;
						cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //


						cv::rectangle(sub_img_draw, rtSubErrRgn, CV_RGB(0, 255, 255), 2); //
						Rect rtDraw = rt;//chips
						rtDraw.x += vRois_Small[i].x + vRois[k].x - inflate; //rtDraw.x에 vRois[k].x전체 크기 중간 객체 추출 좌표를 더하고 세부객체 x좌표 vRois_Small[i].x를 담아 투과적으로 해당 세부객체 x좌표를 직접적으로 가리키게 됨.
						rtDraw.y += vRois_Small[i].y + vRois[k].y;
						vRois_Small_Err.push_back(rtDraw);
						cv::rectangle(sub_img_draw, rrt.boundingRect(), CV_RGB(255, 0, 0), 2); //
					}
				}
			}
		}



		//display result :: error rectangle
		for (size_t i = 0; i < vRois_Large_Err.size(); i++)
		{
			cv::rectangle(src_draw, vRois_Large_Err[i], CV_RGB(255, 0, 255), 2); //
		}
		for (size_t i = 0; i < vRois_Small_Err.size(); i++)
		{
			cv::rectangle(src_draw, vRois_Small_Err[i], CV_RGB(255, 0, 255), 2); //
		}
		int b = 0;
	}

	return;
}
