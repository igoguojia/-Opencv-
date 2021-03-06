
// ImageProcessingView.h : CImageProcessingView ��Ľӿ�
//

#pragma once
#include<opencv.hpp>
#include "ImageProcessingDoc.h"
//#include "CFunctions.h"
class CImageProcessingView : public CScrollView
{
protected: // �������л�����
	CImageProcessingView();
	DECLARE_DYNCREATE(CImageProcessingView)

	// ����
public:
	//BOOL DIBDWTStep(CDib* pDib, int nInv);
	CImageProcessingDoc * GetDocument() const;

	// ����
public:

	// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// ʵ��
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

	// ���ɵ���Ϣӳ�亯��
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
protected://ͼ����Ϣ
	HGLOBAL	horiginImage;
	LPBYTE	lporiginImage;

	HGLOBAL	hcancelImage;//���ͼ�񣬲������
	LPBYTE	lpcancelImage;

	HGLOBAL htempImage;//����ʱ�ָ���ͼ��
	LPBYTE lptempImage;
	LONG	lHeight;// ͼ��ĸ߶�
	LONG	lLineBytes;// ͼ��ÿ�е��ֽ���
	LONG	lWidth;
	int pixelByte;
	LPBITMAPINFOHEADER	lpcancelbmi;			// ָ����תͼ���ָ��
	LPBITMAPINFOHEADER	lpbmi;			// ָ����תͼ���ָ��
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
	CMutex Mutex;//���ڻ�����

	void OnApply();
	void OnRecover();
public:
	float BGR[3] = { 0 };
	float H, S, I, thTa, mi, mx;
	float t1, t2, t3;
	void RGB_HSI();
	void HSI_RGB();
	void OnBright(float bright);
	void OnContrast(float bright, float contrast);//�Աȶȣ���ǿ�Աȶ�, -1,1.0
	void OnSaturation(float saturation);//���Ͷ�
public:
	void OnClarity(int clarity);//������
	float sigma = 0;
	int r = 0;//rλ��˹ģ���뾶
	float gaussFunc1D(int x);
	void getKernal(float *weight);
	int edge(int x, int i, int w);//��Ե���
public:
	void UpdateImage();//�����༭��������ʱ����originImage��
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
	//	operation����
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

#ifndef _DEBUG  // ImageProcessingView.cpp �еĵ��԰汾
inline CImageProcessingDoc* CImageProcessingView::GetDocument() const
{
	return reinterpret_cast<CImageProcessingDoc*>(m_pDocument);
}
#endif

