#pragma once
#include "InspGeneric.h"


class InspectContamination : public InspGeneric
{
public:
	InspectContamination();
	~InspectContamination();
	int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num) override;

private:
	int con_num = 0;
	string contamination = "Contamination";

	double obj_PAD_threshold = 130;

	vector<DefectInfo> vRois_Small_Err, vRois_Large_Err; // 스몰 객체, 라지 객체 검출 저장을 위한 백터 설정
};
#pragma once