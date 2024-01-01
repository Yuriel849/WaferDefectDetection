
// OpenCVAppGUIDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "OpenCVAppGUI.h"
#include "OpenCVAppGUIDlg.h"
#include "afxdialogex.h"

#include "common.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <format>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define NAME_WINDOW_SRC "srcView"
#define NAME_WINDOW_DEBUG "srcDebug"
#define NAME_WINDOW_RESULT "srcResult"
#define WM_ADD_STRING	(WM_USER + 500)



Scalar eJudgeColor[] =
{
   CV_RGB(0,255,0),
   CV_RGB(255, 0, 0),
   CV_RGB(255, 200, 234),
   CV_RGB(250, 178, 8),
   CV_RGB(80,252,254),
   CV_RGB(250, 225, 0)
};
string eItemList[] =
{
   "OK",
   "Scratch",
   "Contam..",
   "Crack",
   "QR",
   "Color"
};


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// COpenCVAppGUIDlg 대화 상자



COpenCVAppGUIDlg::COpenCVAppGUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPENCVAPPGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COpenCVAppGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, _listBoxLog);
}

BEGIN_MESSAGE_MAP(COpenCVAppGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOAD, &COpenCVAppGUIDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, &COpenCVAppGUIDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &COpenCVAppGUIDlg::OnBnClickedBtnInspection)
	ON_BN_CLICKED(IDC_BTN_INSPECTION_CV, &COpenCVAppGUIDlg::OnBnClickedBtnInspectionCv)
	//	ON_BN_CLICKED(IDC_BTN_SAMPLE_CODE, &COpenCVAppGUIDlg::OnBnClickedBtnSampleCode)
	ON_MESSAGE(WM_ADD_STRING, &COpenCVAppGUIDlg::OnAddString)
END_MESSAGE_MAP()


// COpenCVAppGUIDlg 메시지 처리기

BOOL COpenCVAppGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//ShowWindow(SW_MAXIMIZE);
	//ShowWindow(SW_MINIMIZE);

	//opencv -> window handle
	namedWindow(NAME_WINDOW_SRC, WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle(NAME_WINDOW_SRC);
	HWND hParent = (HWND)FindWindow(NULL, NAME_WINDOW_SRC);
	HWND hOrgParent = ::SetParent(hWnd, GetDlgItem(IDC_PICTURE)->m_hWnd);
	::ShowWindow(hOrgParent, SW_HIDE);


	namedWindow(NAME_WINDOW_DEBUG, WINDOW_AUTOSIZE);
	hWnd = (HWND)cvGetWindowHandle(NAME_WINDOW_DEBUG);
	hParent = (HWND)FindWindow(NULL, NAME_WINDOW_DEBUG);
	hOrgParent = ::SetParent(hWnd, GetDlgItem(IDC_PICTURE_DEBUG)->m_hWnd);
	::ShowWindow(hOrgParent, SW_HIDE);

	namedWindow(NAME_WINDOW_RESULT, WINDOW_AUTOSIZE);
	hWnd = (HWND)cvGetWindowHandle(NAME_WINDOW_RESULT);
	hParent = (HWND)FindWindow(NULL, NAME_WINDOW_RESULT);
	hOrgParent = ::SetParent(hWnd, GetDlgItem(IDC_PICTURE_RESULT)->m_hWnd);
	::ShowWindow(hOrgParent, SW_HIDE);

	//GetDlgItem(IDC_PICTURE)->GetWindowRect(_rtImageView);
	//ScreenToClient(_rtImageView);
	_mWndImageView.clear();
	_mWndImageView.insert(pair<int, CRect>(IDC_PICTURE, CRect(0, 0, 0, 0)));
	_mWndImageView.insert(pair<int, CRect>(IDC_PICTURE_DEBUG, CRect(0, 0, 0, 0)));
	_mWndImageView.insert(pair<int, CRect>(IDC_PICTURE_RESULT, CRect(0, 0, 0, 0)));

	for (auto iter = _mWndImageView.begin(); iter != _mWndImageView.end(); iter++) {
		int resID = (iter->first);
		CRect rtImgView;
		GetDlgItem(resID)->GetWindowRect(rtImgView);
		ScreenToClient(rtImgView);
		iter->second = rtImgView;
	}

	_mMatBuff.clear();
	_mInsps.clear();
	//add update inspection list
	UpdateInspList();
	AddString("System Log :: Start()");
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void COpenCVAppGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void COpenCVAppGUIDlg::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	if (IsIconic())
	{
		//CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBrush brush(HS_DIAGCROSS, RGB(200, 200, 200));
		//dc.FillRect(CRect(rect.left + 1, rect.top + 1, rect.right, rect.bottom), &brush);
		for (auto iter = _mWndImageView.begin(); iter != _mWndImageView.end(); iter++) {
			CRect rt = iter->second;
			dc.FillRect(rt, &brush);
		}


		if (!_mMatBuff[eImgSrcColor].empty())
		{
			Mat Img = _mMatBuff[eImgSrcColor];
			Mat DrawResize;
			Size size(int(_dNewScale * Img.cols), int(_dNewScale * Img.rows));
			resize(Img, DrawResize, size);

			resizeWindow(NAME_WINDOW_SRC, size.width, size.height);
			imshow(NAME_WINDOW_SRC, DrawResize);
		}

		if (_bShowResult)
		{
			if (!_mMatBuff[eImgDrawColor].empty())
			{
				Mat Img = _mMatBuff[eImgDrawColor];
				Mat DrawResize;
				Size size(int(_dNewScale * Img.cols), int(_dNewScale * Img.rows));
				resize(Img, DrawResize, size);

				resizeWindow(NAME_WINDOW_RESULT, size.width, size.height);
				imshow(NAME_WINDOW_RESULT, DrawResize);
			}
		}

		if (_bShowDebug)
		{
			if (!_mMatBuff[eImgDebugColor].empty())
			{
				Mat Img = _mMatBuff[eImgDebugColor];
				Mat DrawResize;
				Size size(int(_dNewScale * Img.cols), int(_dNewScale * Img.rows));
				resize(Img, DrawResize, size);

				resizeWindow(NAME_WINDOW_DEBUG, size.width, size.height);
				imshow(NAME_WINDOW_DEBUG, DrawResize);
			}
		}




		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COpenCVAppGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void COpenCVAppGUIDlg::UpdateDispSrc() //이미지 올릴 때(load) 사용
{
	_bShowResult = false;
	CRect rectSrc;
	::GetWindowRect(GetDlgItem(IDC_PICTURE)->m_hWnd, rectSrc);
	double dScaleX = rectSrc.Width() / (double)_SourceImage.cols;
	double dScaleY = rectSrc.Height() / (double)_SourceImage.rows;
	_dSrcScale = min(dScaleX, dScaleY);
	_dNewScale = _dSrcScale;
	//_bShowResult = FALSE;
	CWnd* pWnd = GetDlgItem(IDC_PICTURE);
	pWnd->GetClientRect(&rectSrc);//제어 클라이언트 영역의 좌표 얻기
	pWnd->ClientToScreen(rectSrc);//제어 클라이언트 영역에서 대화 영역으로 영역 좌표 변환
	this->ScreenToClient(rectSrc); //다이얼로그 영역의 영역 좌표를 다이얼로그의 클라이언트 영역 좌표로 변환
	InvalidateRect(rectSrc);
	//비율이 다른 사진을 표시할 때 DC 잔여물 방지


	// Display imag
	//imshow("Contours", drawing);
	_bShowDebug = true;
	_bShowResult = true;
	Invalidate(FALSE);
	/*namedWindow("Wafer Defect Detection", WINDOW_NORMAL);
	cv::resizeWindow("Wafer Defect Detection", 1200, 800);
	imshow("Wafer Defect Detection", drawing);
	waitKey();*/

}

void COpenCVAppGUIDlg::OnBnClickedBtnLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_READONLY, _T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||"));
	if (fileDlg.DoModal() == IDOK)
	{
		CString path = fileDlg.GetPathName();

		string fileName = path.GetString();
		_SourceImage = cv::imread(fileName, IMREAD_ANYCOLOR);

		OnAllocateBuffer(_SourceImage.cols, _SourceImage.rows);
		UpdateDispSrc();

		InvalidateRect(_rtImageView, FALSE);
	}
}

void COpenCVAppGUIDlg::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//string fileName = "jetson_clone.bmp";
	//_imageView.Save(fileName.c_str());

	CFileDialog dlgFileSave(FALSE, "*.*", NULL,//화일명 없음
		OFN_FILEMUSTEXIST,
		_T("image file(*.jpg;*.bmp;*.png;)|*.jpg;*.bmp;*.png;|All Files(*.*)|*.*||"), NULL);

	//옵션 선택 부분.
	if (dlgFileSave.DoModal() == IDOK)
	{
		string str = dlgFileSave.GetPathName().GetString();
		imwrite(str, _ResultImage);
	}

	//AfxMessageBox("Image Saved");
}

void COpenCVAppGUIDlg::OnBnClickedBtnInspection()
{
	_bShowResult = _bShowDebug = false;
	Mat drawResult = _mMatBuff[eImgDrawColor];

	//Legend
	int length = sizeof(eItemList) / sizeof(eItemList[0]);
	for (size_t i = 0; i < length; i++)
	{
		cv::Rect rtInfo = cv::Rect(10, 10 + i * 100, 100, 100);
		cv::rectangle(drawResult, rtInfo, eJudgeColor[i], FILLED);

		string msg = eItemList[i];
		putText(drawResult, msg, Point(rtInfo.x + rtInfo.width + 30, rtInfo.y + rtInfo.height / 2 + 10), FONT_HERSHEY_SIMPLEX, 1.8, eJudgeColor[i], 7);
	}

	if (_mMatBuff[eImgSrcGray].empty())
	{
		AfxMessageBox("NO IMAGE");
	}

	int flaw_num = 0;

	Mat src = _mMatBuff[eImgSrcGray];
	Mat drawing = _mMatBuff[eImgDebugColor] = _mMatBuff[eImgSrcColor].clone();

	//InspectScratch* pins = &insp_scratch;
	//InspGeneric* pins = &insp_scratch;
	//InspGeneric* ptr[3] = { &insp_scratch,&insp_contRe,&insp_cont };

	double mean_H = 0.0;
	cv::Mat bin;
	{//Detect Color: juhee
		cv::threshold(src, bin, 230, 250, cv::THRESH_BINARY_INV);
		// close
		int kernelSz = 4;
		int shape = cv::MorphShapes::MORPH_RECT;
		cv::Size sz = cv::Size(2 * kernelSz + 3, 2 * kernelSz + 2);
		cv::Mat SE = cv::getStructuringElement(shape, sz);
		cv::Mat close;
		int type = cv::MorphTypes::MORPH_CLOSE;
		cv::morphologyEx(bin, close, type, SE);

		cv::RNG rng(12345);
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(close, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

		// Find contour maximum area
		size_t maxContourIndex = 0;
		double maxContourArea = 0;

		for (size_t i = 0; i < contours.size(); i++)
		{
			double area = cv::contourArea(contours[i]);
			if (area > maxContourArea)
			{
				maxContourArea = area;
				maxContourIndex = i;
			}
		}

		cv::Mat contourMask = cv::Mat::zeros(close.size(), CV_8UC1);	//mask with only the selected contour filled
		cv::drawContours(contourMask, contours, static_cast<int>(maxContourIndex), cv::Scalar(255), cv::FILLED);

		// Apply the mask to the original image
		cv::Mat circleImage;
		drawResult.copyTo(circleImage, contourMask);

		cv::Mat hsv_image;
		cv::cvtColor(circleImage, hsv_image, cv::COLOR_BGR2HSV);	//HSV 변환

		std::vector<cv::Mat> channels;
		cv::split(hsv_image, channels);
		cv::Mat hue_channel = channels[0];	// Hue 채널 가져오기

		mean_H = cv::mean(hue_channel)[0];	//평균 Hue(색조)

		//cv::putText(drawResult, "Mean H : " + std::to_string(mean_H), cv::Point(50, 1600), cv::FONT_HERSHEY_SIMPLEX, 2, CV_RGB(250, 225, 0), 3);
	}

	vector<cv::Rect> vRois_Small, vRois_Large;
	vInsps[0]->OnPreprocess(src);
	vector<DefectInfo> vErrInfo(vInsps[0]->GetChipRegions().size());

	string QR_val = "WAFER_DEFAULT";
	string Yield_val = "YIELD_0";

	for (size_t i = 0; i < vInsps.size(); i++)
	{
		InspGeneric* pins = vInsps[i];
		string inspName = pins->GetInspName();

		pins->OnPreprocess(src);
		std::vector<cv::Rect> vChips = pins->GetChipRegions();
		//std::vector<cv::Rect> vLargePadInChips = pins->GetLargePadInChipRegions();

		//std::vector<cv::Rect> vSmallPadInChips = pins->GetSmallPadInChipRegions();
		pins->SetDrawBuffer(drawing);
		pins->SetErrRegions(vErrInfo);
		//pins->SetLargePadInChipRegions(vLargePadInChips);
		//pins->SetSmallPadInChipRegions(vSmallPadInChips);
		pins->OnTestProcess(src, drawing, vChips, flaw_num);
		vErrInfo = pins->GetErrRegions();

		//display
		int scratch_cnt = 0;
		Scalar color;
		string msg;
		for (size_t k = 0; k < vChips.size(); k++)
		{
			Rect rt = vErrInfo[k].rt;
			switch (vErrInfo[k].Judge)
			{
			case eJudge_OK:
				color = eJudgeColor[eJudge_OK];
				break;
			case eJudge_Scratch:
				color = eJudgeColor[eJudge_Scratch];
				msg = to_string(scratch_cnt + 1);
				scratch_cnt++;
				cv::putText(drawing, msg, vChips[i].br(), FONT_HERSHEY_SIMPLEX, 1, color, 2, 8);      //0, 255, 255
				cv::putText(drawing, pins->GetInspName(), vChips[i].tl(), FONT_HERSHEY_SIMPLEX, 0.5, color, 1.5, 8);
				break;
			case eJudge_Contamination:
				color = eJudgeColor[eJudge_Contamination];
				break;
			case eJudge_Crack:
				color = eJudgeColor[eJudge_Crack];
				break;
			default:
				break;
			}

			cv::rectangle(drawResult, vChips[k], color, FILLED);
		}

		if (pins->GetQRCode() != "")
		{
			cv::Rect rtQR = pins->GetQRArea();
			cv::rectangle(drawResult, rtQR, color, 2);
			rtQR.y -= 50;
			putText(drawResult, pins->GetQRCode(), Point(rtQR.x, rtQR.y), FONT_HERSHEY_SIMPLEX, 2.7, eJudgeColor[eJudge_QR], 7);
			QR_val = pins->GetQRCode();
		}
		if (mean_H > 0.0)
		{
			//display color 
			cv::putText(drawResult, "Color value : " + std::to_string(mean_H), cv::Point(50, 1660), cv::FONT_HERSHEY_SIMPLEX, 2, eJudgeColor[5], 3);
		}
	}

	// cout << flaw_num << endl;	// 이 칩의 총 결함 개수가 제대로 저장되었는가 확인 
	vInsps[0]->OnPostprocess(src);

	// binary 결과 사진 저장할 변수
	_ResultImage = drawResult;

	_bShowResult = _bShowDebug = true;
	Invalidate(FALSE);
	string QR_out = "Wafer QR Code is : " + QR_val;
	//string Yield_out = "Wafer Yield is : " + Yield_val + "%";
	string Color_out = "Wafer color value is : " + std::to_string(mean_H);
	AddString(QR_out.c_str());
	//AddString(Yield_out.c_str());
	AddString(Color_out.c_str());
}

int COpenCVAppGUIDlg::OnAllocateBuffer(int cols, int rows) //이미지 Load 시 사용
{
	_mMatBuff.clear();

	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgSrcColor, Mat(rows, cols, CV_8UC3)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgSrcGray, Mat(rows, cols, CV_8UC1)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgDebugGray, Mat(rows, cols, CV_8UC1)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgDebugColor, Mat(rows, cols, CV_8UC3)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgDrawColor, Mat(rows, cols, CV_8UC3)));
	_mMatBuff.insert(make_pair<int, cv::Mat>(eImgResultColor, Mat(rows, cols, CV_8UC3)));

	//source to source of map
	//_SourceImage.copyTo(_mMatBuff[eImgSrc]);

	//source to draw of map
	if (_SourceImage.type() == CV_8UC1)
	{
		cvtColor(_SourceImage, _mMatBuff[eImgSrcColor], COLOR_GRAY2BGR);
		_SourceImage.copyTo(_mMatBuff[eImgSrcGray]);
	}
	else
	{
		_SourceImage.copyTo(_mMatBuff[eImgSrcColor]);
		cvtColor(_SourceImage, _mMatBuff[eImgSrcGray], COLOR_BGR2GRAY);
	}

	Mat vi = _mMatBuff[eImgSrcColor];

	_mMatBuff[eImgSrcColor].copyTo(_mMatBuff[eImgDrawColor]);

	//debug of map ... just create buffer as initilized value
	_mMatBuff[eImgDebugGray] = 0;

	//result of map ... just create buffer as initilized value
	_mMatBuff[eImgResultColor] = 255;

	return 0;
}

int COpenCVAppGUIDlg::UpdateInspList()
{
	//_mInsps.clear();
	//_mInsps.insert(make_pair("OnInspFindcontourSample", COpenCVAppGUIDlg::CallInspFindcontourSample));
	//_mInsps.insert(make_pair("OnInspFindShape", COpenCVAppGUIDlg::CallInspFindShape));
	//_mInsps.insert(make_pair("OnInspFindMultiShape", COpenCVAppGUIDlg::CallInspFindMultiShape));
	//_mInsps.insert(make_pair("OnInspFindColor", COpenCVAppGUIDlg::CallInspFindColor));
	//_mInsps.insert(make_pair("OnInspSegmentColor", COpenCVAppGUIDlg::CallInspSegmentColor));
	//_mInsps.insert(make_pair("OnInspMorpology", COpenCVAppGUIDlg::CallInspMorpology));
	//_mInsps.insert(make_pair("OnInspMorpologyMultiObjects", COpenCVAppGUIDlg::CallInspMorpologyMultiObjects));
	//_mInsps.insert(make_pair("OnInspHistoEqulization", COpenCVAppGUIDlg::CallInspHistoEqulization));
	//_mInsps.insert(make_pair("OnInspCorrection", COpenCVAppGUIDlg::CallInspCorrection));
	//_mInsps.insert(make_pair("OnInspSearchingContour", COpenCVAppGUIDlg::CallInspSearchingContour));
	//_mInsps.insert(make_pair("OnInspMatching", COpenCVAppGUIDlg::CallInspMatching));
	//_mInsps.insert(make_pair("OnMeanShiftTracking", COpenCVAppGUIDlg::CallMeanShiftTracking));
	//_mInsps.insert(make_pair("OnCannyEdgeDetector", COpenCVAppGUIDlg::CallCannyEdgeDetector));


	/// main 함수 내용을 넣을 곳. //////////////////////////////////////////////////////////////



	vInsps.push_back(&insp_scratch);
	vInsps.push_back(&insp_crack);
	vInsps.push_back(&insp_cont);
	vInsps.push_back(&insp_QR);
	vInsps.push_back(&insp_Y);



	return 1;
}

int COpenCVAppGUIDlg::OnInspection(InputArray src, OutputArray dst)
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspection(Mat src, Mat dst)
{
	//우리가 만들었던 함수를 끼워넣을 만한 함수 



	return 0;
}

int COpenCVAppGUIDlg::OnInspection(uchar* pSrc, size_t cols, size_t rows, uchar* pDst)
{
	return 0;
}

//static_cast
//double a = (double)(int -> double 1);
//a 1.000000
//const_cast
//dynamic_cast ...상속관계
//reinterpret_cast 

int COpenCVAppGUIDlg::CallInspFindcontourSample(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFindcontourSample();
}

int COpenCVAppGUIDlg::CallInspFindShape(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFindShapes();
}

int COpenCVAppGUIDlg::CallInspFindMultiShape(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFindMultiShape();
}

int COpenCVAppGUIDlg::CallInspFindColor(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspFindColor();
}

int COpenCVAppGUIDlg::CallInspSegmentColor(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspSegmentColor();
}

int COpenCVAppGUIDlg::CallInspMorpology(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspMorpology();
}
int COpenCVAppGUIDlg::CallInspMorpologyChar(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspMorpologyChar();
}
int COpenCVAppGUIDlg::CallInspMorpologyMultiObjects(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspMorpologyMultiObjects();
}
int COpenCVAppGUIDlg::CallInspHistoEqulization(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspHistoEqulization();
}
int COpenCVAppGUIDlg::CallInspCorrection(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspCorrection();
}
int COpenCVAppGUIDlg::CallInspSearchingContour(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspSearchingContour();
}
int COpenCVAppGUIDlg::CallInspMatching(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnInspMatching();
}
int COpenCVAppGUIDlg::CallMeanShiftTracking(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnMeanShiftTracking();
}
int COpenCVAppGUIDlg::CallCannyEdgeDetector(void* lpUserData)
{
	COpenCVAppGUIDlg* pDlg = reinterpret_cast<COpenCVAppGUIDlg*>(lpUserData);
	return pDlg->OnCannyEdgeDetector();
}
int COpenCVAppGUIDlg::OnInspFindcontourSample()
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspFindShapes()
{


	return 0;
}

int COpenCVAppGUIDlg::OnInspFindMultiShape()
{

	return 0;
}






void COpenCVAppGUIDlg::OnBnClickedBtnInspectionCv()
{

}


void COpenCVAppGUIDlg::OnBnClickedBtnSampleCode()
{

}
void COpenCVAppGUIDlg::AddString(LPCTSTR lpszLog)
{
	SendMessage(WM_ADD_STRING, 0, (LPARAM)lpszLog);
}
LRESULT COpenCVAppGUIDlg::OnAddString(WPARAM wParam, LPARAM lParam)
{
	if (!GetSafeHwnd()) return 0;

	if (_listBoxLog.GetCount() > 500)
	{
		_listBoxLog.ResetContent();
	}

	LPCTSTR lpszLog = reinterpret_cast <LPCTSTR> (lParam);
	SYSTEMTIME t;
	GetLocalTime(&t);
	CString nStrMsg = _T("");
	nStrMsg += lpszLog;
	int nIdx = _listBoxLog.AddString(nStrMsg);
	_listBoxLog.SetTopIndex(_listBoxLog.GetCount() - 1);

	//stringstream ssTime;
	//time_t const now_c = time(NULL);
	////ssTime << put_time(localtime(&now_c), "%a %d %b %Y - %I_%M_%S%p");
	//ssTime << put_time(localtime(&now_c), "%a %d %b %Y-%I_%M");
	//string time_c = ssTime.str();

	//ofstream file;
	//string fileName;
	//fileName += "log";
	//fileName += ssTime.str();
	//fileName += ".txt";

	//file.open(fileName, ios::out | ios::app);
	//file << nStrMsg << endl;
	//file.close();

	return 0;
}

int COpenCVAppGUIDlg::OnInspFindColor()
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspSegmentColor()
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspMorpology()
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspMorpologyChar()
{
	return 0;
}

int COpenCVAppGUIDlg::OnInspMorpologyMultiObjects()
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspHistoEqulization()
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspCorrection()
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspSearchingContour()
{

	return 0;
}

int COpenCVAppGUIDlg::OnInspMatching()
{

	return 0;
}

cv::Rect track_window(0, 0, 50, 50);
bool track_window_activated = false;
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{


}
int COpenCVAppGUIDlg::OnMeanShiftTracking()
{

	return 0;
}

int COpenCVAppGUIDlg::OnDrawHistogram(const Mat& src_gray, Mat& draw_color)
{

	return 1;
}

#include "canny.h"
int COpenCVAppGUIDlg::OnCannyEdgeDetector()
{

	return 1;
}