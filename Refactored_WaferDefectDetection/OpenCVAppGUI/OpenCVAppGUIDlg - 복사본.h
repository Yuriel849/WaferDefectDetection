
// OpenCVAppGUIDlg.h: 헤더 파일
//

#pragma once

#include <iostream>

#define OPENCV_470

#include <opencv2/opencv.hpp>
#ifdef OPENCV_470
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>
#endif // OPENCV_470


#ifdef _DEBUG
#pragma comment(lib,"opencv_world470d.lib")
#else	//RELEASE
#pragma comment(lib,"opencv_world470.lib")
#endif
using namespace std;
using namespace cv;


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

	CRect _rtImageView;//image view area
	CImage _imageView;//image control object

	cv::Mat _DrawImage; // 이미지 정보를 담고 있는 객체.
	BITMAPINFO* _pBitmapInfo=nullptr; // Bitmap 정보를 담고 있는 구조체.
	bool _bShowResult = false;
	double _dSrcScale = 1.0, _dPtrnScale = 1.0, _dNewScale = 1.0;
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
	void UpdateDispSrc();
	void DisplayImage(int resDisplayID, Mat srcMat);
	void DrawImage();
	void CreateBitmapInfo(int w, int h, int bpp);
};
