
// OpenCVAppGUIDlg.h: 헤더 파일
//
//ctrl + k + c 전체 주석
//ctrl + k + u 전체 주석 해제

#pragma once
#include "resource.h"

#include "common.h"
#include "InspectScratch.h"
#include "InspectCrack.h"
#include "InspectContamination.h"
#include "InspectQR.h"
#include "InspectYield.h"

//std::string FLAWLESS = "./res/img/final_real_wafer.png";
//std::string DONUT = "./res/img/donut_black.png";
//std::string EDGE_LOCATION = "./res/img/edge_location_black.png";
//std::string LOCATION = "./res/img/location_black.png";
//std::string SCRATCH = "./res/img/scratch_black.png";
//
//std::string SCRATCH_GREEN = "./res/img/Scratch_green.bmp";
//std::string LOCATION_GREEN = "./res/img/Location_green.bmp";
//std::string EDGE_LOCATION_NAVY = "./res/img/Edge_location_navy.bmp";
//std::string DONUT_NAVY = "./res/img/Donut_navy.bmp";


enum { 
	eImgSrcColor = 0, 
	eImgSrcGray, 
	eImgDebugGray, 
	eImgDebugColor, 
	eImgDrawColor, 
	eImgResultColor, 
	eImgBufferMax 
};



typedef int(*InspMethod)(void*);



// COpenCVAppGUIDlg 대화 상자
class COpenCVAppGUIDlg : public CDialogEx
{
	// 생성입니다.
public:
	COpenCVAppGUIDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENCVAPPGUI_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:
	InspectScratch insp_scratch;
	InspectCrack insp_crack;
	InspectContamination insp_cont;
	InspectQR insp_QR;
	InspectYield insp_Y;

	std::vector<InspGeneric*> vInsps;

	std::map<string, InspMethod> _mInsps;// { {"src", 10}, { "GPU", 15 }, { "RAM", 20 } };
	std::map<int, cv::Mat> _mMatBuff;// { {"src", 10}, { "GPU", 15 }, { "RAM", 20 } };
	std::map<int, CRect> _mWndImageView;// { {1016, (10,10,300,400)}, { 2, (301,10,300,400) }, { "RAM", 20 } };

	CRect _rtImageView;//image view area
	cv::Mat _SourceImage, _ResultImage, _DrawImage; // 이미지 정보를 담고 있는 객체.
	//_Draw ... display ... original+draw rect/line/circle ...
	//_Frame/_SourceBuffer  ... original
	//agorithm...function (input, params)...input-constant, params-variables
	//_ProcessImage ... 중간/결과 이미지 buffer

	bool _bShowResult = false;
	bool _bShowDebug = false;

	double _dSrcScale = 1.0, _dNewScale = 1.0;

	vector<cv::Point> _vLinePoints_Left;
	vector<cv::Point> _vLinePoints_Right;
	vector<cv::Point> _vLinePoints_Top;
	vector<cv::Point> _vLinePoints_Btm;
	cv::Point _pt1, _pt2;
public:
	void UpdateDispSrc();

private:
	int OnAllocateBuffer(int cols, int rows);
	int UpdateInspList();

	int OnInspection(InputArray src, OutputArray dst);
	int OnInspection(Mat src, Mat dst);
	int OnInspection(uchar* pSrc, size_t cols, size_t rows, uchar* pDst);

	static int CallInspFindcontourSample(void* lpUserData);
	static int CallInspFindShape(void* lpUserData);
	static int CallInspFindMultiShape(void* lpUserData);
	static int CallInspFindColor(void* lpUserData);
	static int CallInspSegmentColor(void* lpUserData);
	static int CallInspMorpology(void* lpUserData);
	static int CallInspMorpologyChar(void* lpUserData);
	static int CallInspMorpologyMultiObjects(void* lpUserData);
	static int CallInspHistoEqulization(void* lpUserData);
	static int CallInspCorrection(void* lpUserData);
	static int CallInspSearchingContour(void* lpUserData);
	static int CallInspMatching(void* lpUserData);
	static int CallMeanShiftTracking(void* lpUserData);
	static int CallCannyEdgeDetector(void* lpUserData);
	
	int OnInspFindcontourSample();
	int OnInspFindShapes();
	int OnInspFindMultiShape();
	int OnInspFindColor();
	int OnInspSegmentColor();
	int OnInspMorpology();
	int OnInspMorpologyChar();
	int OnInspMorpologyMultiObjects();
	int OnInspHistoEqulization();
	int OnInspCorrection();
	int OnInspSearchingContour();

	int OnInspMatching();

	int OnMeanShiftTracking();

	int OnDrawHistogram(const Mat& src, Mat& draw);

	int OnCannyEdgeDetector();

	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnBnClickedBtnInspectionCv();
	afx_msg void OnBnClickedBtnSampleCode();

	afx_msg LRESULT OnAddString(WPARAM wParam, LPARAM lParam);


	/// <summary>
	/// 함수 구현 
	/// </summary>

	CListBox _listBoxLog;
	void AddString(LPCTSTR lpszLog);
	CButton CBtn_donut;
	CButton CBtn_scratch;
	CButton CBtn_location;
	CButton CBtn_edge;
};
