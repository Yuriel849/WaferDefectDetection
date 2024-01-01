#pragma once

#include "Common.h"

struct DefectInfo
{
	cv::Rect rt;
	int Judge;//0-Ok, 1-Scratch, 2-Contamination, 3-..etc;
};


enum eJudgeResult
{
	eJudge_OK,
	eJudge_Scratch,
	eJudge_Contamination,
	eJudge_Crack,
	eJudge_QR,
	eJudge_Max
};

class InspGeneric
{
public:
	InspGeneric();
	virtual ~InspGeneric();
	
	virtual int OnPreprocess(const Mat& src);
	virtual int OnTestProcess(const Mat& src, const Mat& drawColor, std::vector<cv::Rect>& vChips, int& flaw_num) = 0;
	virtual int OnPostprocess(const Mat& src);
	void SetDrawBuffer(const Mat& srcColor) { drawImg = srcColor; }
	void SetInspName(const string& name) { _InspectName = name; }
	string GetInspName() { return _InspectName; }

	void SetQRCode(string QRCode) { _QRCode = QRCode; }
	string GetQRCode() { return _QRCode; }
	void SetQRArea(Rect QRArea) { _rtQRCode = QRArea; }
	cv::Rect GetQRArea() {return _rtQRCode;}

	void SetErrRegions(const vector<DefectInfo>& ErrRegions) { vErrRegions = ErrRegions; }
	vector<DefectInfo> GetErrRegions() { return vErrRegions; }

	vector<Rect> GetChipRegions() { return vChips; }
	vector<Rect> GetLargePadInChipRegions() { return vLargePadsInChip; }
	vector<Rect> GetSmallPadInChipRegions() { return vSmallPadsInChip; }
	void SetLargePadInChipRegions(const vector<Rect>& LargePadsInChip) { vLargePadsInChip = LargePadsInChip; }
	void SetSmallPadInChipRegions(const vector<Rect>& SmallPadsInChip) { vSmallPadsInChip = SmallPadsInChip; }

protected:
	const string PATTERN = "./res/img/wafer_template.png";
	string _InspectName = "Crack";
	string _QRCode = "";
	cv::Rect _rtQRCode;

	float avg_Width_Large=0, avg_Height_Large = 0, avg_Width_Small = 0, avg_Height_Small = 0;
	vector<DefectInfo> vErrRegions;
	std::vector<cv::Rect> vChips, vLargePadsInChip, vSmallPadsInChip;
	cv::Mat drawImg;
};

