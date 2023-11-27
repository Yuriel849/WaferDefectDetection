#include "InspectContamination.h"


InspectContamination::InspectContamination()
{

}

InspectContamination::~InspectContamination()
{

}

int InspectContamination::OnTestProcess(const Mat& src, const Mat& drawColor, void* pUserData)
{
	cv::Mat src_gray_search = src;
	cv::Mat src_draw = drawColor;
	
	cv::Mat src_gray_templt = cv::imread(TEMPLATE, cv::ImreadModes::IMREAD_GRAYSCALE);

	// Template matching to find individual microcircuits inside the wafer
	vector<Rect> finds;
	GetObjectRegionsTemplatMatching(src_gray_search, src_gray_templt, finds);

	// Inspect each microcircuit for contamination defects
	for (size_t k = 0; k < finds.size(); k++)
	{
		// Mark microcircuit as defectless (will remark if defects detected)
		//cv::rectangle(src_draw, finds[k], CV_RGB(255, 0, 0), 1);
		//cv::drawMarker(src_draw, finds[k].tl(), CV_RGB(255, 0, 0), MarkerTypes::MARKER_CROSS);
		//string msg = to_string(k + 1);
		//putText(src_draw, msg, finds[k].tl(), FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(0, 255, 255), 1, 8);
		cv::rectangle(src_draw, finds[k], CV_RGB(0, 205, 255), CV_FILLED);

		//---- Check this wafer for chip invasion defects ----//
		cv::Mat src_templt = src_gray_templt.clone();
		cv::Mat src_wafer = src_gray_search(finds[k]).clone();
		cv::Mat src_wMasked = mask(28, 112, src_wMasked, src_wafer); // For wafer : Mask y-axis 28 - 112
		cv::Mat src_tMasked = mask(24, 106, src_tMasked, src_templt); // For template : Mask y-axis 24 - 106
		// Binarization
		cv::Mat src_tBin = cv::Mat::zeros(src_templt.size(), CV_8UC1);
		cv::Mat src_wBin = cv::Mat::zeros(src_wafer.size(), CV_8UC1);
		double thres = 125;
		cv::threshold(src_wMasked, src_wBin, thres, 255, ThresholdTypes::THRESH_BINARY);
		cv::threshold(src_tMasked, src_tBin, thres, 255, ThresholdTypes::THRESH_BINARY);
		//Morphology
		int kernelSz = 1;
		int shape = MorphShapes::MORPH_ELLIPSE;
		cv::Size sz = Size(2 * kernelSz + 1, 2 * kernelSz + 1);
		Mat SE = cv::getStructuringElement(shape, sz);
		cv::Mat src_wErode, src_tErode;
		cv::erode(src_wBin, src_wErode, SE);
		cv::erode(src_tBin, src_tErode, SE);
		// Find contours
		vector<vector<Point>> wContours, tContours;
		vector<Vec4i> wHierarchy, tHierarchy;
		findContours(src_wErode, wContours, wHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		findContours(src_tErode, tContours, tHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		Mat drawing_wafer = src_wafer.clone();
		Mat drawing_templt = src_templt.clone();
		// Check area of bounding rectangles
		if (wContours.size() == tContours.size())
		{
			for (size_t i = 0; i < wContours.size(); i++)
			{
				Rect2f wR2f = minAreaRect(wContours[i]).boundingRect2f();
				Rect2f tR2f = minAreaRect(tContours[i]).boundingRect2f();
				// Identify squares with minimum area bounding rectangle
				cv::rectangle(drawing_wafer, wR2f.tl(), wR2f.br(), 0, 1, LINE_8);
				cv::rectangle(drawing_templt, tR2f.tl(), tR2f.br(), 0, 1, LINE_8);
				// If difference in area greater than 30
				if (abs(tR2f.area() - wR2f.area()) > 30) { defectFlag = true; break; }
			}
		}

		if (defectFlag)
		{
			cv::rectangle(src_draw, finds[k], CV_RGB(255, 0, 100), CV_FILLED);
			defectFlag = false;
		}
		int b = 0;
		cv::drawMarker(src_draw, finds[k].tl(), CV_RGB(255, 0, 0), MarkerTypes::MARKER_CROSS);
		string msg = to_string(k + 1);
		putText(src_draw, msg, finds[k].tl(), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 255), 4, 8);
	}
	int a = 0;

	return 1;
}

int InspectContamination::GetObjectRegionsTemplatMatching(const Mat& src, const Mat& ptrn, std::vector<cv::Rect>& vRegions)
{
	double thres = 0.99;
	Mat result = Mat::zeros(Size(src.cols - ptrn.cols + 1, src.rows - ptrn.rows + 1), CV_32FC1);
	int match_method = TM_CCORR_NORMED;
	matchTemplate(src, ptrn, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	double minVal; double maxVal; Point minLoc; Point maxLoc;
	Point matchLoc;

	bool multiobjects = true;
	if (!multiobjects)
	{
		minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
		if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
		{
			matchLoc = minLoc;
		}
		else
		{
			matchLoc = maxLoc;
		}

		vRegions.push_back(Rect(matchLoc.x, matchLoc.y, ptrn.cols, ptrn.rows));
	}
	else
	{
		Mat tm_bin;
		double min_thr = thres;
		double max_thr = 1.0;
		cv::threshold(result, tm_bin, min_thr, max_thr, ThresholdTypes::THRESH_BINARY);
		tm_bin *= 255;

		tm_bin.convertTo(tm_bin, CV_8UC1);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(tm_bin, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

		for (size_t i = 0; i < contours.size(); i++)
		{
			RotatedRect rrt = minAreaRect(contours[i]);
			//vRegions.push_back(rrt.boundingRect());
			vRegions.push_back(Rect(rrt.boundingRect().tl().x, rrt.boundingRect().tl().y, ptrn.cols, ptrn.rows));
		}
	}

	return 1;
}

Mat& InspectContamination::mask(int boundaryLow, int boundaryHigh, Mat& masked, Mat& target)
{
	cv::Mat mask = cv::Mat::zeros(target.size(), CV_8UC1);
	uchar* mData = mask.data;
	for (size_t row = 0; row < target.rows; row++)
	{
		for (size_t col = 0; col < target.cols; col++)
		{
			int index = row * target.cols + col;
			if (!(boundaryLow < row && row < boundaryHigh)) { mData[index] = 255; }
		}
	}
	masked = target & mask;
	return masked;
}