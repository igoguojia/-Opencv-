
// ImageProcessingView.h : CImageProcessingView 类的接口
//

#pragma once
#include<opencv.hpp>
#include "ImageProcessingDoc.h"
//#include "CFunctions.h"
class CImageProcessingView : public CScrollView
{
protected: // 仅从序列化创建
	CImageProcessingView();
	DECLARE_DYNCREATE(CImageProcessingView)

	// 特性
public:
	//BOOL DIBDWTStep(CDib* pDib, int nInv);
	CImageProcessingDoc * GetDocument() const;

	// 操作
public:

	// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 实现
public:
	double* m_pDbImage;
	int m_nSupp;
	int m_nDWTCurDepth;
	virtual ~CImageProcessingView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
//	afx_msg void OnCodingShanfino();
//	afx_msg void OnCodingArith();
//	afx_msg void OnCodingBitplane();
//	afx_msg void OnCodingHuffman();
//	afx_msg void OnCodingLoadimg();
//	afx_msg void OnCodingWriteimg();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnLapplacian();
//	afx_msg void OnAddShow();
//	afx_msg void OnRegGrow();
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	int x, y;

	afx_msg void OnCamera();
	afx_msg void OnAdjust();
protected://图像信息
	HGLOBAL	horiginImage;
	LPBYTE	lporiginImage;

	HGLOBAL	hcancelImage;//最初图像，不会更改
	LPBYTE	lpcancelImage;

	HGLOBAL htempImage;//重置时恢复的图像
	LPBYTE lptempImage;
	LONG	lHeight;// 图象的高度
	LONG	lLineBytes;// 图象每行的字节数
	LONG	lWidth;
	int pixelByte;
	LPBITMAPINFOHEADER	lpcancelbmi;			// 指向旋转图像的指针
	LPBITMAPINFOHEADER	lpbmi;			// 指向旋转图像的指针
	DWORD palSize;
	LONG imagesize = 0;

	int *diffbc = NULL, *diffsaturation = NULL, *diffclarity = NULL;
protected:
	CvCapture* pCapture = NULL;
	IplImage* pFrame = NULL;
	char tempjpg[50] = "";
	time_t rawtime = 0;
	struct tm *info = NULL;
	char buffer[30] = "";
public:
	void OnCancel();
	void DestoryWin();
	void OnCompare(UINT un);
	BOOL ButtonDown = TRUE;
	CMutex Mutex;//用于互斥锁

	void OnApply();
	void OnRecover();
public:
	float BGR[3] = { 0 };
	float H, S, I, thTa, mi, mx;
	float t1, t2, t3;
	void RGB_HSI();
	void HSI_RGB();
	void OnBright(float bright);
	void OnContrast(float bright, float contrast);//对比度，增强对比度, -1,1.0
	void OnSaturation(float saturation);//饱和度
public:
	void OnClarity(int clarity);//清晰度
	float sigma = 0;
	int r = 0;//r位高斯模糊半径
	float gaussFunc1D(int x);
	void getKernal(float *weight);
	int edge(int x, int i, int w);//边缘检测
public:
	void UpdateImage();//基础编辑操作更换时更新originImage；
	void RecoverImage_1();

public:
	int *diffautolight = NULL, *diffhighlight = NULL, *diffshadow = NULL;
	void OnAutoLight(double alight);
	void OnHighLight(float hlight);
	void OnShadow(float shadow);
	void RecoverImage_2();

	void OnHue(int hue);
	void RecoverImage_3();
	int *diffRGB = NULL;
	//	operation窗口
public:
	afx_msg void OnOperation();

	void OnCancel_2();
	void OnDestroyWin_2();
	void OnApply_2();
	void OnRecover_2();
	void OnUDMirror();
	void OnRotate(int rotation);

	void OnAddnoise();
	void OnRemovenoise();
	void OnScale(float scale);
	void OnSharpen();
	void OnDivide();
	void OnRelief();
	BOOL OnConnectImage(float conncet);

	CDib *connect_Dib=NULL;
	void OnHighlightAverage();
	void OnCut(float up,float down,float left,float right);
	void OnApplyCut(float up, float down, float left, float right);

//	afx_msg void On32797();
};
//extern CImageProcessingView view;

#ifndef _DEBUG  // ImageProcessingView.cpp 中的调试版本
inline CImageProcessingDoc* CImageProcessingView::GetDocument() const
{
	return reinterpret_cast<CImageProcessingDoc*>(m_pDocument);
}
#endif

