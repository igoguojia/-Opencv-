
// ImageProcessingView.cpp : CImageProcessingView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ImageProcessing.h"
#endif
#include "ImageProcessingDoc.h"
#include "ImageProcessingView.h"
#include  "GlobalApi.h"
#include  "CImageOperation.h"
//#include <complex>
#include "conio.h"
#include <cmath>
#include "CFunctions.h"
#include "COperation.h"
#include<iostream>
//#include "function.h"
//#define PI 3.1415926535
#define PI 4*atan(1)  //使用了function.h中的定义

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageProcessingView

IMPLEMENT_DYNCREATE(CImageProcessingView, CScrollView)

BEGIN_MESSAGE_MAP(CImageProcessingView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CImageProcessingView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_CAMERA, &CImageProcessingView::OnCamera)
	ON_COMMAND(ID_32795, &CImageProcessingView::OnAdjust)
	ON_COMMAND(ID_32796, &CImageProcessingView::OnOperation)
END_MESSAGE_MAP()
// CImageProcessingView 构造/析构
CImageProcessingView::CImageProcessingView()
{
	// TODO: 在此处添加构造代码
	// 为小波变换设置的参数
	// 临时存放小波变换系数内存
	m_pDbImage = NULL;

	// 设置当前层数
	m_nDWTCurDepth = 0;

	// 设置小波基紧支集长度
	m_nSupp = 1;


}
CFunctions *cf = NULL;
COperation *co = NULL;
CImageProcessingView::~CImageProcessingView()
{
	// 释放已分配内存
	if (m_pDbImage) {
		delete[]m_pDbImage;
		m_pDbImage = NULL;
	}
	if (cf)
	{
		delete cf;
		cf = NULL;
	}
	if (co)
	{
		delete co;
		co = NULL;
	}
	//FreeConsole();

}

BOOL CImageProcessingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
}

// CImageProcessingView 绘制

void CImageProcessingView::OnDraw(CDC* pDC)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CSize sizeDibDisplay;



	if (!pDoc->m_pDibInit->IsEmpty()) {
		sizeDibDisplay = pDoc->m_pDibInit->GetDimensions();
		pDoc->m_pDibInit->Draw(pDC, CPoint(0, 0), sizeDibDisplay);
	}
}

void CImageProcessingView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CSize sizeTotal = pDoc->m_pDibInit->GetDimensions();
	SetScrollSizes(MM_TEXT, sizeTotal);

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	//AllocConsole();
}
// CImageProcessingView 打印


void CImageProcessingView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CImageProcessingView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CImageProcessingView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CImageProcessingView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CImageProcessingView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CImageProcessingView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CImageProcessingView 诊断

#ifdef _DEBUG
void CImageProcessingView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CImageProcessingView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CImageProcessingDoc* CImageProcessingView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageProcessingDoc)));
	return (CImageProcessingDoc*)m_pDocument;
}
#endif //_DEBUG

void CImageProcessingView::OnCamera()
{
	// TODO: 在此添加命令处理程序代码
	pCapture = cvCreateCameraCapture(1);
	cvNamedWindow("video", 1);
	HWND hWnd = (HWND)cvGetWindowHandle("video");
	::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	while (pFrame = cvQueryFrame(pCapture))
	{
		cvShowImage("video", pFrame);
		if (cv::waitKey(15) == 'p')
		{	//if (face1->width > 100)
		//{
		//}
			time(&rawtime);
			info = localtime(&rawtime);
			strftime(buffer, 30, "%Y%m%d_%H%M%S", info);
			sprintf(tempjpg, "..\\%s.bmp", buffer);

			cvSaveImage(tempjpg, pFrame);
			cvShowImage("face", pFrame);
			cv::waitKey(500);
			cvDestroyWindow("face");
			theApp.OnCameraFileOpen(tempjpg);
			break;
		}
		if (cv::waitKey(30) == 27)
		{
			break;
		}
	}
	cvDestroyWindow("video");
	cvReleaseCapture(&pCapture);
	pCapture = NULL;
}


void CImageProcessingView::OnAdjust()
{
	// TODO: 在此添加命令处理程序代码
	/*IplImage * image = NULL; //原始图像
	image = cvLoadImage("..\\test.png", 1); //显示图片
	//cvShowImage("video", pFrame);
	cvSaveImage("..\\testbmp.bmp", image);*/
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小

	SizeDim = pDib->GetDimensions();
	lHeight = SizeDim.cy;
	lWidth = SizeDim.cx;
	SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数
	pixelByte = lpBMIH->biBitCount / 8;

	horiginImage = GlobalAlloc(GHND, lLineBytes *lHeight);
	htempImage = GlobalAlloc(GHND, lLineBytes *lHeight);
	hcancelImage = GlobalAlloc(GHND, lLineBytes *lHeight);

	if (horiginImage == NULL || htempImage == NULL || hcancelImage == NULL)
	{
		return;
	}
	lporiginImage = (LPBYTE)GlobalLock(horiginImage);
	lptempImage = (LPBYTE)GlobalLock(htempImage);
	lpcancelImage = (LPBYTE)GlobalLock(hcancelImage);

	// 初始化新分配的内存，设定初始值为0
	memset(lporiginImage, (BYTE)255, lLineBytes *lHeight);
	memset(lptempImage, (BYTE)255, lLineBytes *lHeight);
	memset(lpcancelImage, (BYTE)255, lLineBytes *lHeight);

	memcpy(lporiginImage, lpDIBBits, lLineBytes * lHeight);
	memcpy(lpcancelImage, lpDIBBits, lLineBytes * lHeight);

	imagesize = lLineBytes * lHeight;

	diffbc = new int[imagesize];
	diffsaturation = new int[imagesize];
	diffclarity = new int[imagesize];
	diffautolight = new int[imagesize];
	diffhighlight = new int[imagesize];
	diffshadow = new int[imagesize];
	diffRGB = new int[imagesize];

	memset(diffbc, 0, sizeof(int)*imagesize);
	memset(diffsaturation, 0, sizeof(int)*imagesize);
	memset(diffclarity, 0, sizeof(int)*imagesize);
	memset(diffautolight, 0, sizeof(int)*imagesize);
	memset(diffhighlight, 0, sizeof(int)*imagesize);
	memset(diffshadow, 0, sizeof(int)*imagesize);
	memset(diffRGB, 0, sizeof(int)*imagesize);

	cf = new CFunctions(this);
	cf->Create(IDD_DIALOG1);
	cf->ShowWindow(SW_SHOWNORMAL);
}

void CImageProcessingView::OnCancel()
{
	CImageProcessingDoc * pDoc = GetDocument();

	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	memcpy(lpDIBBits, lpcancelImage, imagesize);
	pDoc->UpdateAllViews(NULL);									// 更新视图	

}

void CImageProcessingView::DestoryWin()
{
	delete diffbc;
	delete diffsaturation;
	delete diffclarity;
	delete diffautolight;
	delete diffhighlight;
	delete diffshadow;
	delete diffRGB;
	diffbc = NULL;
	diffsaturation = NULL;
	diffclarity = NULL;
	diffautolight = NULL;
	diffhighlight = NULL;
	diffshadow = NULL;
	diffRGB = NULL;
	GlobalUnlock(horiginImage);
	GlobalFree(horiginImage);
	GlobalUnlock(htempImage);
	GlobalFree(htempImage);
	GlobalUnlock(hcancelImage);
	GlobalFree(hcancelImage);
	lporiginImage = NULL;
	lptempImage = NULL;
	lpcancelImage = NULL;
	delete cf;
	cf = NULL;

}

void CImageProcessingView::OnCompare(UINT un)
{
	/*CSingleLock singleLock(&Mutex);
	singleLock.Lock();*/
	CImageProcessingDoc * pDoc = GetDocument();

	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	if (un == WM_LBUTTONDOWN)
	{
		memset(lptempImage, 0, imagesize);
		memcpy(lptempImage, lpDIBBits, imagesize);
		memcpy(lpDIBBits, lporiginImage, imagesize);

		ButtonDown = FALSE;
	}
	else if (un == WM_LBUTTONUP)
	{
		memcpy(lpDIBBits, lptempImage, imagesize);
		ButtonDown = TRUE;
	}
	pDoc->UpdateAllViews(NULL);									// 更新视图	
	//singleLock.Unlock();//解锁

}

void CImageProcessingView::OnApply()
{
	CImageProcessingDoc * pDoc = GetDocument();

	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	memcpy(lporiginImage, lpDIBBits, imagesize);

	memset(diffbc, 0, sizeof(int)*imagesize);
	memset(diffsaturation, 0, sizeof(int)*imagesize);
	memset(diffclarity, 0, sizeof(int)*imagesize);
	memset(diffautolight, 0, sizeof(int)*imagesize);
	memset(diffhighlight, 0, sizeof(int)*imagesize);
	memset(diffshadow, 0, sizeof(int)*imagesize);
	cf->SeniorTool = FALSE;
	cf->BasisTool = FALSE;

	pDoc->UpdateAllViews(NULL);// 更新视图	

}

void CImageProcessingView::OnRecover()
{
	CImageProcessingDoc * pDoc = GetDocument();

	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	memcpy(lpDIBBits, lporiginImage, imagesize);

	memset(diffbc, 0, sizeof(int)*imagesize);
	memset(diffsaturation, 0, sizeof(int)*imagesize);
	memset(diffclarity, 0, sizeof(int)*imagesize);
	memset(diffautolight, 0, sizeof(int)*imagesize);
	memset(diffhighlight, 0, sizeof(int)*imagesize);
	memset(diffshadow, 0, sizeof(int)*imagesize);
	cf->SeniorTool = FALSE;
	cf->BasisTool = FALSE;

	pDoc->UpdateAllViews(NULL);// 更新视图	
}

void CImageProcessingView::RGB_HSI()
{
	//没有对HSI归一化，但是对RGB归一化了
	//HSI S 和I的取值范围为0-1，下面放缩的时候要注意不要超出范围,H 的范围为0-2*PI；
	for (int i = 0; i < 3; i++)
		BGR[i] /= 255;
	if (BGR[2] > BGR[1] && BGR[2] > BGR[0])
	{
		mx = BGR[2];
		mi = min(BGR[1], BGR[0]);
	}
	else
	{
		if (BGR[1] > BGR[0])
		{
			mx = BGR[1];
			mi = min(BGR[2], BGR[0]);
		}
		else
		{
			mx = BGR[0];
			mi = min(BGR[2], BGR[1]);
		}
	}

	I = (BGR[2] + BGR[1] + BGR[0]) / 3.0;
	if (I < 0.0)
		I = 0.0;
	else if (I > 1.0)
		I = 1.0;

	if (I == 0 || mx == mi)
	{
		S = 0;
		H = 0;
	}
	else
	{
		S = 1.0 - mi / I;
		thTa = (BGR[2] - BGR[1]) * (BGR[2] - BGR[1]) + (BGR[2] - BGR[0]) * (BGR[1] - BGR[0]);
		thTa = sqrt(thTa) + 1e-5;
		thTa = acos(((BGR[2] - BGR[1] + BGR[2] - BGR[0])*0.5) / thTa);

		if (BGR[1] >= BGR[0])
			H = thTa;
		else
			H = 2 * PI - thTa;
	}
	//H = H / (2 * PI);

	// 最终输出的 H S I值即为计算后的值     

	/*for (int i = 0; i < 3; i++)
		BGR[i] /= 255;
	I = (BGR[0] + BGR[1] + BGR[2]) / 3;
	if (I == 0)
	{
		thTa = 0;
		S = 0;
	}
	else
	{
		thTa = acos((2 * BGR[2] - BGR[1] - BGR[0]) / (sqrt(BGR[2] * BGR[2] - BGR[1] * BGR[2] - BGR[2] * BGR[0] - BGR[1] * BGR[0]+BGR[1]*BGR[1]+BGR[0]*BGR[0])) / 2);
		S = 1 - 3 * min(min(BGR[0], BGR[1]), BGR[2]) / (BGR[0] + BGR[1] + BGR[2]);
	}

	if (BGR[1] >= BGR[0])
		H = 0;
	else
		H = 2 * PI - thTa;*/
}
void CImageProcessingView::HSI_RGB()
{
	//没有对HSI归一化，但是对RGB归一化了
	/*t1 = (1.0 - S) / 3.0;

	if (H >= 0 && H < (PI * 2 / 3))
	{
		BGR[0] = t1;
		t2 = S * cos(H);
		t3 = cos(PI / 3 - H);
		BGR[2] = (1 + t2 / t3) / 3;
		BGR[2] = 3 * I * BGR[2];
		BGR[0] = 3 * I * BGR[0];
		BGR[1] = 3 * I - (BGR[2] + BGR[0]);
	}
	else if (H >= (PI * 2 / 3) && H < (PI * 4 / 3))
	{
		BGR[2] = t1;
		t2 = S * cos(H - 2 * PI / 3);
		t3 = cos(PI - H);
		BGR[1] = (1 + t2 / t3) / 3;
		BGR[2] = 3 * I * BGR[2];
		BGR[1] = 3 * BGR[1] * I;
		BGR[0] = 3 * I - (BGR[2] + BGR[1]);
	}
	else if (H >= (PI * 4 / 3) && H <= (PI * 2))
	{
		BGR[1] = t1;
		t2 = S * cos(H - 4 * PI / 3);
		t3 = cos(PI * 5 / 3 - H);
		BGR[0] = (1 + t2 / t3) / 3;
		BGR[1] = 3 * BGR[1] * I;
		BGR[0] = 3 * I * BGR[0];
		BGR[2] = 3 * I - (BGR[1] + BGR[0]);
	}*/
	if (H >= 0 && H < PI * 2 / 3)
	{
		BGR[0] = I * (1 - S);
		BGR[2] = I * (1 + S * cos(H) / cos(PI / 3 - H));
		BGR[1] = 3 * I - (BGR[2] + BGR[0]);
	}
	else if (H >= PI * 2 / 3 && H < PI * 4 / 3)
	{
		H = H - PI * 2 / 3;
		BGR[2] = I * (1 - S);
		BGR[1] = I * (1 + S * cos(H) / cos(PI / 3 - H));
		BGR[0] = 3 * I - (BGR[2] + BGR[1]);
	}
	else
	{
		H = H - PI * 4 / 3;
		BGR[1] = I * (1 - S);
		BGR[0] = I * (1 + S * cos(H) / cos(PI / 3 - H));
		BGR[2] = 3 * I - (BGR[1] + BGR[0]);
	}
}
void CImageProcessingView::OnBright(float bright)
{

	LONG i, j, k;
	LPBYTE lpSrc;//指向原始数据
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);

	if (pixelByte >= 3)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				int temp = 0;
				int outvalue = 0;
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
					BGR[k] = *lpSrc;
				}

				RGB_HSI();
				I *= bright;
				if (I > 1)I = 1.0;
				//S /= 2;
				HSI_RGB();
				for (k = 0; k < pixelByte; k++)
				{
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = BGR[k] * 255.0 + 0.5;//增量
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpDst = (BYTE)temp;
				}
			}
		}
	}
	else
	{
		int temp = 0;
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{

				lpSrc = lporiginImage + lLineBytes * i + j;
				temp = (*lpSrc)*bright;
				lpDst = lpNewDIBBits + lLineBytes * i + j;
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				*lpDst = (BYTE)temp;

			}
		}
	}
	//pDoc->UpdateAllViews(NULL);									// 更新视图	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
}

void CImageProcessingView::OnContrast(float bright, float contrast)
{
	LONG i, j, k;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);

	//float v = 255 / (((float)contrast / 100)*log(256) - ((float)contrast / 100)*log(1));
	float v = tan((45 + 44 * contrast) / 180 * PI);
	//int v = contrast * 255.0;	
	//float threshold = 127.5;
	if (pixelByte >= 3)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				float temp = 0;
				for (k = 0; k < pixelByte; k++)
				{
					/*	lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
						temp =(*lpSrc) + 1;
						temp = ((float)contrast/100)*log(temp);
						temp = 0+v * (temp - log(1))+0.5;
						if (temp > 255)
							temp = 255;
						lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
						*lpDst = temp;*/

						/**
						Algorithm of Brightness Contrast transformation
						The formula is:
						y = [x - 127.5 * (1 - B)] * k + 127.5 * (1 + B);
						x is the input pixel value   0-255
						y is the output pixel value  0-255
						B is brightness, value range is [-1,1]
						k is used to adjust contrast
							k = tan( (45 + 44 * c) / 180 * PI );
							c is contrast, value range is [-1,1]
							**/
							//亮度对比度调节 转换成 I
					lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
					BGR[k] = *lpSrc;

					/*lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = (*lpSrc);
					float addContrst = temp - v;
					if (addContrst == -255)
						temp =v;
					else if (addContrst == 255)
					{
						temp = temp;
					}
					else if (addContrst < 0 && addContrst >- 255)
					{
						temp = temp + (temp - v)*addContrst / 255.0;
					}
					else if (addContrst >= 0 && addContrst < 255)
					{
						addContrst = 255 * 255 / (255 - addContrst) - 255;
						temp = temp + (temp - v)*addContrst / 255.0;

					}
					if (temp > 255)
						temp = 255;
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					*lpDst = temp;
					*/
					/*lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = (*lpSrc);
					if (contrast == 1)
						temp > threshold? temp = temp : temp = threshold ;
					else if (contrast >=0)
					{
						contrast = 1 - contrast;
						contrast = 1 / contrast - 1;
						temp = temp + (temp - threshold)*contrast;

					}
					else
					{
						temp = temp + (temp - threshold)*contrast;
					}
					if (temp > 255)
						temp = 255;
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					*lpDst = (BYTE)temp;*/
				}

				RGB_HSI();
				I *= 255.0;
				I = (I - 127.5 * (1 - bright))* v + 127.5 * (1 + bright);
				I /= 255.0;
				if (I > 1.0)I = 1.0;
				if (I < 0.0)I = 0;
				HSI_RGB();
				for (k = 0; k < pixelByte; k++)
				{
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = BGR[k] * 255.0 + 0.5;
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpDst = (BYTE)temp;
				}

			}
		}
	}
	else
	{
		int temp = 0;
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{

				lpSrc = lporiginImage + lLineBytes * i + j;
				temp = *lpSrc;
				temp = (temp - 127.5 * (1 - bright))* v + 127.5 * (1 + bright);
				lpDst = lpNewDIBBits + lLineBytes * i + j;
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				*lpDst = (BYTE)temp;
			}

		}
	}
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				diffbc[lLineBytes * i + j * pixelByte + k] = *lpDst - *lpSrc;
			}
		}
	}
	//pDoc->UpdateAllViews(NULL);									// 更新视图	
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
}

void CImageProcessingView::OnSaturation(float saturation)
{
	if (pixelByte < 3)
		return;
	LONG i, j, k;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);

	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			int temp = 0;
			int outvalue = 0;
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				BGR[k] = *lpSrc;
			}
			RGB_HSI();
			S *= saturation;
			if (S > 1)S = 1.0;
			if (S < 0)S = 0;
			HSI_RGB();
			for (k = 0; k < pixelByte; k++)
			{
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				temp = BGR[k] * 255.0 + 0.5;
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				*lpDst = (BYTE)temp;
			}
		}
	}
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				diffsaturation[lLineBytes * i + j * pixelByte + k] = *lpDst - *lpSrc;
			}
		}
	}
	//pDoc->UpdateAllViews(NULL);									// 更新视图
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
}

/////////////////////////////////////////////////////////////////////////////
//采用高斯模糊
float CImageProcessingView::gaussFunc1D(int x)
{
	float A = 1 / (sigma*sqrt(2 * 3.141592653));
	float index = -1.0*(x*x) / (2 * sigma*sigma);

	return A * exp(index);
}
void CImageProcessingView::getKernal(float * weight)
{
	float sum = 0;
	// 获取权值空间weight[]
	for (int i = 0; i < 2 * r + 1; i++)
	{
		weight[i] = gaussFunc1D(i - r);
		sum += weight[i];
	}
	//归一化
	for (int i = 0; i < 2 * r + 1; i++)
	{
		weight[i] /= sum;
	}
}
int CImageProcessingView::edge(int x, int i, int w)
{
	// x为中心位置, i为偏移量,w为宽度
	int inx = x + i;
	if (inx < 0 || inx >= w)
		return x - i;
}

void CImageProcessingView::OnClarity(int clarity)
{
	if (clarity == 0)
	{
		memset(diffclarity, 0, sizeof(int)*imagesize);
		return;
	}
	float *weight = NULL;
	r = clarity;
	weight = new float[2 * r + 1];

	sigma = (float)r / 3;
	getKernal(weight);

	// TODO: 在此添加命令处理程序代码
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;

	LONG i, j, k;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);

	//一维高斯函数，横向纵向两个方向，快速算法。
	//https://blog.csdn.net/mahabharata_/article/details/69066942
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			int temp;
			int sum[3] = { 0 };
			int outvalue = 0;
			for (int m = -r; m <= r; m++)
			{
				int inx = edge(j, m, lWidth);
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lporiginImage + lLineBytes * i + inx * pixelByte + k;
					temp = (*lpSrc)*weight[r + m] + 0.5;
					//if (temp > 255)temp = 255;
					sum[k] += temp;

				}
			}
			for (k = 0; k < pixelByte; k++)
			{
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				if (sum[k] > 255)sum[k] = 255;
				if (sum[k] < 0)sum[k] = 0;
				*lpDst = (BYTE)sum[k];
			}
		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, imagesize);
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			int temp = 0;
			int sum[3] = { 0 };

			for (int m = -r; m <= r; m++)
			{
				int iny = edge(i, m, lHeight);
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lpDIBBits + lLineBytes * iny + j * pixelByte + k;
					temp = (*lpSrc)*weight[r + m] + 0.5;
					//if (temp > 255)temp = 255;
					sum[k] += temp;
				}
			}
			for (k = 0; k < pixelByte; k++)
			{
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				if (sum[k] > 255)sum[k] = 255;
				if (sum[k] < 0)sum[k] = 0;
				*lpDst = (BYTE)sum[k];
			}
		}
	}
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				diffclarity[lLineBytes * i + j * pixelByte + k] = *lpDst - *lpSrc;
			}
		}
	}
	//memcpy(lpDIBBits, lpNewDIBBits, imagesize);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	//pDoc->UpdateAllViews(NULL);									// 更新视图	
}
/////////////////////////////////////////////////////////////////////////////

void CImageProcessingView::UpdateImage()
{
	CImageProcessingDoc * pDoc = GetDocument();

	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	// 头文件信息
	LONG i, j, k;
	LPBYTE lpSrc;
	memcpy(lpDIBBits, lporiginImage, imagesize);
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			int temp = 0;
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
				temp = *lpSrc + diffclarity[lLineBytes * i + j * pixelByte + k]
					+ diffsaturation[lLineBytes * i + j * pixelByte + k]
					+ diffbc[lLineBytes * i + j * pixelByte + k]
					+ diffautolight[lLineBytes * i + j * pixelByte + k]
					+ diffhighlight[lLineBytes * i + j * pixelByte + k]
					+ diffshadow[lLineBytes * i + j * pixelByte + k]
					+ diffRGB[lLineBytes * i + j * pixelByte + k];
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				*lpSrc = (BYTE)temp;
			}
		}
	}
	pDoc->UpdateAllViews(NULL);									// 更新视图	

}
void CImageProcessingView::RecoverImage_1()
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	LONG i, j, k;
	LPBYTE lpSrc;
	//memcpy(lpDIBBits, lporiginImage, lLineBytes * lHeight);
	//memcpy(horiginImage, hrecoverImage, lLineBytes * lHeight);
	memcpy(lpDIBBits, lporiginImage, imagesize);
	if (cf->SeniorTool || cf->HueTool)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				int temp = 0;
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = *lpSrc
						+ diffautolight[lLineBytes * i + j * pixelByte + k]
						+ diffhighlight[lLineBytes * i + j * pixelByte + k]
						+ diffshadow[lLineBytes * i + j * pixelByte + k]
						+ diffRGB[lLineBytes * i + j * pixelByte + k];
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpSrc = (BYTE)temp;
				}
			}
		}
	}

	memset(diffbc, 0, sizeof(int)*imagesize);
	memset(diffsaturation, 0, sizeof(int)*imagesize);
	memset(diffclarity, 0, sizeof(int)*imagesize);

	cf->BasisTool = FALSE;
	pDoc->UpdateAllViews(NULL);									// 更新视图	

}

////////////////////////////////////////////////////////////////////
void CImageProcessingView::OnAutoLight(double alight)
{
	LONG i, j, k;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);
	float v = tan((45 + 44 * alight) / 180 * PI);
	if (pixelByte >= 3)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				float temp = 0;
				for (k = 0; k < pixelByte; k++)
				{


					/**
					Algorithm of Brightness Contrast transformation
					The formula is:
					y = [x - 127.5 * (1 - B)] * k + 127.5 * (1 + B);
					x is the input pixel value   0-255
					y is the output pixel value  0-255
					B is brightness, value range is [-1,1]
					k is used to adjust contrast
						k = tan( (45 + 44 * c) / 180 * PI );
						c is contrast, value range is [-1,1]
						**/
						//亮度对比度调节 转换成 I
					lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
					BGR[k] = *lpSrc;



				}

				RGB_HSI();
				I *= 255.0;
				I = (I - 127.5 * (1 - alight))* v + 127.5 * (1 + alight);
				I /= 255.0;
				if (I > 1.0)I = 1.0;
				if (I < 0.0)I = 0;
				HSI_RGB();
				for (k = 0; k < pixelByte; k++)
				{
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = BGR[k] * 255.0 + 0.5;
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpDst = (BYTE)temp;
				}

			}
		}
	}
	else
	{
		int temp = 0;
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{

				lpSrc = lporiginImage + lLineBytes * i + j;
				temp = *lpSrc;
				temp = (temp - 127.5 * (1 - alight))* v + 127.5 * (1 + alight);
				lpDst = lpNewDIBBits + lLineBytes * i + j;
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				*lpDst = (BYTE)temp;
			}

		}
	}
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				diffautolight[lLineBytes * i + j * pixelByte + k] = *lpDst - *lpSrc;
			}
		}
	}
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

}
void CImageProcessingView::OnHighLight(float hlight)
{
	LONG i, j, k;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);
	//float v = tan((45 + 44 * alight) / 180 * PI);
	if (pixelByte >= 3)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				float temp = 0;
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
					BGR[k] = *lpSrc;
				}

				RGB_HSI();
				I *= 255.0;
				if (I > 127.5)I *= (1.0 + 0.15*hlight);
				I /= 255.0;
				if (I > 1.0)I = 1.0;
				if (I < 0.0)I = 0;
				HSI_RGB();
				for (k = 0; k < pixelByte; k++)
				{
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = BGR[k] * 255.0 + 0.5;
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpDst = (BYTE)temp;
				}

			}
		}
	}
	else
	{
		int temp = 0;
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{

				lpSrc = lporiginImage + lLineBytes * i + j;
				temp = *lpSrc;
				if (temp > 128)temp *= (1.0 + 0.15*hlight);
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				lpDst = lpNewDIBBits + lLineBytes * i + j;
				*lpDst = (BYTE)temp;
			}

		}
	}
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				diffhighlight[lLineBytes * i + j * pixelByte + k] = *lpDst - *lpSrc;
			}
		}
	}
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

}
void CImageProcessingView::OnShadow(float shadow)
{
	LONG i, j, k;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);
	//float v = tan((45 + 44 * alight) / 180 * PI);
	if (pixelByte >= 3)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				float temp = 0;
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
					BGR[k] = *lpSrc;
				}

				RGB_HSI();
				I *= 255.0;
				if (I < 127.5)I *= (1.0 - 0.15*shadow);
				I /= 255.0;
				if (I > 1.0)I = 1.0;
				if (I < 0.0)I = 0;
				HSI_RGB();
				for (k = 0; k < pixelByte; k++)
				{
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = BGR[k] * 255.0 + 0.5;
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpDst = (BYTE)temp;
				}

			}
		}
	}
	else
	{
		int temp = 0;
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{

				lpSrc = lporiginImage + lLineBytes * i + j;
				temp = *lpSrc;
				if (temp < 128)temp *= (1.0 - 0.15*shadow);
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				lpDst = lpNewDIBBits + lLineBytes * i + j;
				*lpDst = (BYTE)temp;
			}

		}
	}
	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				diffshadow[lLineBytes * i + j * pixelByte + k] = *lpDst - *lpSrc;
			}
		}
	}
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

}
void CImageProcessingView::RecoverImage_2()
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	LONG i, j, k;
	LPBYTE lpSrc;
	memcpy(lpDIBBits, lporiginImage, imagesize);

	if (cf->BasisTool || cf->HueTool)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				int temp = 0;
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = *lpSrc + diffclarity[lLineBytes * i + j * pixelByte + k]
						+ diffsaturation[lLineBytes * i + j * pixelByte + k]
						+ diffbc[lLineBytes * i + j * pixelByte + k]
						+ diffRGB[lLineBytes * i + j * pixelByte + k];
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpSrc = (BYTE)temp;
				}
			}
		}
	}


	memset(diffautolight, 0, sizeof(int)*imagesize);
	memset(diffhighlight, 0, sizeof(int)*imagesize);
	memset(diffshadow, 0, sizeof(int)*imagesize);
	cf->SeniorTool = FALSE;

	pDoc->UpdateAllViews(NULL);									// 更新视图	
}

void CImageProcessingView::OnHue(int hue)
{
	if (pixelByte < 3)return;
	hue %= 360;//取余，归一到0-360；
	float hueAngle = (float)hue*PI / 180.0;// 将旋转角度从度转换到弧度
	LONG i, j, k;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);

	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			float temp = 0;
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				BGR[k] = *lpSrc;
			}

			RGB_HSI();

			H += hueAngle;
			H /= 2;
			if (H > 2 * PI)cprintf("%d %d% %f", i, j, H);
			HSI_RGB();
			for (k = 0; k < pixelByte; k++)
			{
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				temp = BGR[k] * 255.0 + 0.5;
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				*lpDst = (BYTE)temp;
			}

		}
	}

	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				diffRGB[lLineBytes * i + j * pixelByte + k] = *lpDst - *lpSrc;
			}
		}
	}
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

}

void CImageProcessingView::RecoverImage_3()
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	LONG i, j, k;
	LPBYTE lpSrc;
	memcpy(lpDIBBits, lporiginImage, lLineBytes * lHeight);
	if (cf->BasisTool || cf->SeniorTool)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				int temp = 0;
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = *lpSrc + diffclarity[lLineBytes * i + j * pixelByte + k]
						+ diffsaturation[lLineBytes * i + j * pixelByte + k]
						+ diffbc[lLineBytes * i + j * pixelByte + k]
						+ diffautolight[lLineBytes * i + j * pixelByte + k]
						+ diffhighlight[lLineBytes * i + j * pixelByte + k]
						+ diffshadow[lLineBytes * i + j * pixelByte + k];
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpSrc = (BYTE)temp;
				}
			}
		}
	}

	memset(diffRGB, 0, sizeof(int)*imagesize);

	cf->HueTool = FALSE;

	pDoc->UpdateAllViews(NULL);									// 更新视图	
}

///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void CImageProcessingView::OnOperation()
{
	CImageProcessingDoc * pDoc = GetDocument();

	/*LONG	lHeight;// 图象的高度
	LONG	lWidth;
	LONG	lLineBytes;// 图象每行的字节数*/
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	SizeDim = pDib->GetDimensions();
	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;
	SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数
	pixelByte = lpBMIH->biBitCount / 8;
	imagesize = lLineBytes * lHeight;
	palSize = pDib->GetPalSize();

	LPBYTE	lpcancelNewDIB;			// 指向旋转图像的指针
	LPBYTE  lpNewDIB;
	hcancelImage = GlobalAlloc(GHND, lLineBytes *lHeight + lpBMIH->biSize + palSize);
	horiginImage = GlobalAlloc(GHND, lLineBytes *lHeight + lpBMIH->biSize + palSize);

	lpcancelNewDIB = (LPBYTE)GlobalLock(hcancelImage);
	lpNewDIB = (LPBYTE)GlobalLock(horiginImage);

	memcpy(lpcancelNewDIB, pDib->m_lpBMIH, lpBMIH->biSize + palSize);
	memcpy(lpNewDIB, pDib->m_lpBMIH, lpBMIH->biSize + palSize);

	lpcancelImage = lpcancelNewDIB + *(LPDWORD)lpcancelNewDIB + palSize;;//FindDIBBits(lpNewDIB);
	lporiginImage = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);

	lpcancelbmi = (LPBITMAPINFOHEADER)lpcancelNewDIB;
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;

	// 初始化新分配的内存，设定初始值为0
	memcpy(lpcancelImage, lpDIBBits, lLineBytes * lHeight);
	memcpy(lporiginImage, lpDIBBits, lLineBytes * lHeight);

	// TODO: 在此添加命令处理程序代码
	co = new COperation(this);
	co->Create(IDD_DIALOG2);
	co->ShowWindow(SW_SHOWNORMAL);
}


void CImageProcessingView::OnCancel_2()
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//LPBYTE  lpDIBBits = pDib->m_lpImage;
	long	lDstWidth;      //临时图像的宽度和高度
	long	lDstHeight;
	long	lDstLineBytes;
	lDstWidth = (LONG)lpcancelbmi->biWidth; //计算缩放后的图像实际宽度, 加0.5是由于强制类型转换时不四舍五入，而是直接截去小数部分
	lDstHeight = (LONG)lpcancelbmi->biHeight;
	lDstLineBytes = (lDstWidth * pixelByte * 8 + 31) / 32 * 4;;
	LPBYTE lpSrc;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	HGLOBAL	hNewDIBBits;
	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	LPBITMAPINFOHEADER	lpDstbmi;			// 指向旋转图像的指针

	hNewDIBBits = GlobalAlloc(GHND, lDstLineBytes *lDstHeight + lpcancelbmi->biSize + palSize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIB = (LPBYTE)GlobalLock(hNewDIBBits);

	memcpy(lpNewDIB, lpcancelbmi, lpcancelbmi->biSize + palSize);

	// 初始化新分配的内存，设定初始值为0
	lpNewDIBBits = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);

	lpDstbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpDst = (LPBYTE)lpNewDIBBits;
	//memset(lpDst, (BYTE)255, lDstLineBytes *lDstHeight);
	memcpy(lpNewDIBBits, lpcancelImage, lDstLineBytes*lDstHeight);

	pDib->AttachMemory(lpDstbmi, 0, hNewDIBBits);
	pDoc->UpdateAllViews(NULL);									// 更新视图	

	//CImageProcessingDoc * pDoc = GetDocument();
	////  获得图象CDib类的指针
	//CDib * pDib = pDoc->m_pDibInit;
	//LPBYTE  lpDIBBits = pDib->m_lpImage;
	//LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	//lWidth = lpcancelbmi->biWidth;
	//lHeight = lpcancelbmi->biHeight;
	//lLineBytes = ((lpcancelbmi->biWidth) * pixelByte * 8 + 31) / 32 * 4;
	//imagesize = lLineBytes * lHeight;
	//memcpy(lpBMIH, lpcancelbmi, lpcancelbmi->biSize + palSize);
	//memcpy(lpDIBBits, lpcancelImage, imagesize);
	//pDib->AttachMemory(lpcancelbmi, 0, hcancelImage);
	//GlobalUnlock(horiginImage);
	//GlobalFree(horiginImage);
	//LPBYTE  lpNewDIB;
	//horiginImage = GlobalAlloc(GHND, lLineBytes *lHeight + lpcancelbmi->biSize + palSize);
	//lpNewDIB = (LPBYTE)GlobalLock(horiginImage);
	//memcpy(lpNewDIB, lpcancelbmi, lpcancelbmi->biSize + palSize);
	//lporiginImage = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);
	//lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	//memcpy(lporiginImage, lpcancelImage, lLineBytes*lHeight);


	pDoc->UpdateAllViews(NULL);									// 更新视图
}
//void CImageProcessingView::OnCancel_2()
//{
//	CImageProcessingDoc * pDoc = GetDocument();
//	//  获得图象CDib类的指针
//	CDib * pDib = pDoc->m_pDibInit;
//
//	LPBYTE  lpDIBBits = pDib->m_lpImage;
//	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
//	lWidth = lpcancelbmi->biWidth;
//	lHeight = lpcancelbmi->biHeight;
//	lLineBytes = ((lpcancelbmi->biWidth) * pixelByte * 8 + 31) / 32 * 4;
//	imagesize = lLineBytes * lHeight;
//
//	memcpy(lpBMIH, lpcancelbmi, lpcancelbmi->biSize + palSize);
//	memcpy(lpDIBBits, lpcancelImage, imagesize);
//
//	//pDib->AttachMemory(lpcancelbmi, 0, hcancelImage);
//
//	GlobalUnlock(horiginImage);
//	GlobalFree(horiginImage);
//
//	LPBYTE  lpNewDIB;
//	horiginImage = GlobalAlloc(GHND, lLineBytes *lHeight + lpcancelbmi->biSize + palSize);
//	lpNewDIB = (LPBYTE)GlobalLock(horiginImage);
//
//	memcpy(lpNewDIB, lpcancelbmi, lpcancelbmi->biSize + palSize);
//	lporiginImage = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);
//	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
//
//	memcpy(lporiginImage, lpcancelImage, lLineBytes*lHeight);
//
//
//	pDoc->UpdateAllViews(NULL);									// 更新视图
//}

void CImageProcessingView::OnDestroyWin_2()
{
	GlobalUnlock(hcancelImage);
	GlobalFree(hcancelImage);
	GlobalUnlock(horiginImage);
	GlobalFree(horiginImage);
	lporiginImage = NULL;
	lpcancelImage = NULL;
	if (connect_Dib)
	{
		delete connect_Dib;
		connect_Dib = NULL;
	}
	delete co;
	co = NULL;
}

void CImageProcessingView::OnApply_2()
{	// 更改光标形状
	BeginWaitCursor();

	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小

	//LONG lWidth, lHeight, lLineBytes;
	SizeDim = pDib->GetDimensions();

	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;

	SizeRealDim = pDib->GetDibSaveDim();//单位为像素

	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数
	imagesize = lLineBytes * lHeight;

	GlobalUnlock(horiginImage);
	GlobalFree(horiginImage);
	lporiginImage = NULL;

	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	palSize = pDib->GetPalSize();
	horiginImage = GlobalAlloc(GHND, lLineBytes *lHeight + pDib->m_lpBMIH->biSize + palSize);
	lpNewDIB = (LPBYTE)GlobalLock(horiginImage);
	memcpy(lpNewDIB, pDib->m_lpBMIH, pDib->m_lpBMIH->biSize + palSize);
	lporiginImage = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	memcpy(lporiginImage, lpDIBBits, lLineBytes * lHeight);

	if (connect_Dib)
	{
		delete connect_Dib;
		connect_Dib = NULL;
	}

	// 恢复光标
	EndWaitCursor();

}

void CImageProcessingView::OnRecover_2()
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	long	lDstWidth;      //临时图像的宽度和高度
	long	lDstHeight;
	long	lDstLineBytes;
	lDstWidth = (LONG)lWidth; //计算缩放后的图像实际宽度, 加0.5是由于强制类型转换时不四舍五入，而是直接截去小数部分
	lDstHeight = (LONG)lHeight;
	lDstLineBytes = lLineBytes;
	LPBYTE lpSrc;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	HGLOBAL	hNewDIBBits;
	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	LPBITMAPINFOHEADER	lpDstbmi;			// 指向旋转图像的指针

	hNewDIBBits = GlobalAlloc(GHND, lDstLineBytes *lDstHeight + lpbmi->biSize + palSize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIB = (LPBYTE)GlobalLock(hNewDIBBits);

	memcpy(lpNewDIB, lpbmi, lpbmi->biSize + palSize);

	// 初始化新分配的内存，设定初始值为0
	lpNewDIBBits = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);

	lpDstbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpDst = (LPBYTE)lpNewDIBBits;
	//memset(lpDst, (BYTE)255, lDstLineBytes *lDstHeight);
	memcpy(lpNewDIBBits, lporiginImage, lDstLineBytes*lDstHeight);

	pDib->AttachMemory(lpDstbmi, 0, hNewDIBBits);
	if (connect_Dib)
	{
		delete connect_Dib;
		connect_Dib = NULL;
	}
	pDoc->UpdateAllViews(NULL);									// 更新视图	

	//CImageProcessingDoc * pDoc = GetDocument();
	////  获得图象CDib类的指针
	//CDib * pDib = pDoc->m_pDibInit;

	//LPBYTE  lpDIBBits = pDib->m_lpImage;
	//LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	//lWidth = lpbmi->biWidth;
	//lHeight = lpbmi->biHeight;
	//lLineBytes = ((lpbmi->biWidth) * pixelByte * 8 + 31) / 32 * 4;
	//imagesize = lLineBytes * lHeight;

	//memcpy(lpBMIH, lpbmi, lpbmi->biSize + palSize);
	//memcpy(lpDIBBits, lpcancelImage, imagesize);
	//if (connect_Dib)
	//{
	//	delete connect_Dib;
	//	connect_Dib = NULL;
	//}
	//pDoc->UpdateAllViews(NULL);									// 更新视图

}
//void CImageProcessingView::OnRecover_2()
//{
//	CImageProcessingDoc * pDoc = GetDocument();
//	//  获得图象CDib类的指针
//	CDib * pDib = pDoc->m_pDibInit;
//
//	LPBYTE  lpDIBBits = pDib->m_lpImage;
//	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
//	lWidth = lpbmi->biWidth;
//	lHeight = lpbmi->biHeight;
//	lLineBytes = ((lpbmi->biWidth) * pixelByte * 8 + 31) / 32 * 4;
//	imagesize = lLineBytes * lHeight;
//
//	memcpy(lpBMIH, lpbmi, lpbmi->biSize + palSize);
//	memcpy(lpDIBBits, lpcancelImage, imagesize);
//	if (connect_Dib)
//	{
//		delete connect_Dib;
//		connect_Dib = NULL;
//	}
//	pDoc->UpdateAllViews(NULL);									// 更新视图
//
//}


void CImageProcessingView::OnUDMirror()
{
	//GlobalUnlock(horiginImage);
	//GlobalFree(horiginImage);
	//lporiginImage = NULL;

	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	LONG i;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpBits;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	LONG	lHeight;// 图象的高度
	LONG	lWidth;
	LONG	lLineBytes;// 图象每行的字节数
	LONG  imagesize = 0;

	//LONG lWidth, lHeight, lLineBytes;
	SizeDim = pDib->GetDimensions();

	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;

	SizeRealDim = pDib->GetDibSaveDim();//单位为像素

	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数
	imagesize = lLineBytes * lHeight;

	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);

	for (i = 0; i < lHeight / 2; i++)// 垂直镜像，针对图像每行进行操作
	{
		lpSrc = (LPBYTE)lpDIBBits + lLineBytes * i;
		lpBits = (LPBYTE)lpDIBBits + lLineBytes * (lHeight - i - 1);

		memcpy(lpNewDIBBits, lpBits, lLineBytes);
		memcpy(lpBits, lpSrc, lLineBytes);
		memcpy(lpSrc, lpNewDIBBits, lLineBytes);
	}
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	/*
	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	palSize = pDib->GetPalSize();
	horiginImage = GlobalAlloc(GHND, lLineBytes *lHeight + pDib->m_lpBMIH->biSize + palSize);

	lpNewDIB = (LPBYTE)GlobalLock(horiginImage);

	memcpy(lpNewDIB, pDib->m_lpBMIH, pDib->m_lpBMIH->biSize + palSize);

	lporiginImage = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);
	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;

	memcpy(lporiginImage, lpDIBBits, lLineBytes * lHeight);
	*/
	pDoc->UpdateAllViews(NULL);									// 更新视图	
}

void CImageProcessingView::OnRotate(int rotation)
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	LPBYTE  lpDIBBits = pDib->m_lpImage;

	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	LONG i, j, j1, i1;
	LPBYTE lpSrc;
	float fRotateAngle = (float)rotation*PI / 180.0;// 将旋转角度从度转换到弧度
	if (fRotateAngle > 2 * PI)
		fRotateAngle -= 2 * PI;
	float fSina = (float)sin((double)fRotateAngle);// 计算旋转角度的正余弦	
	float fCosa = (float)cos((double)fRotateAngle);
	float	fSrcX1, fSrcY1, fSrcX2, fSrcY2, fSrcX3, fSrcY3, fSrcX4, fSrcY4;// 旋转前四个角的坐标（以图像中心为坐标系原点）		
	float	fDstX1, fDstY1, fDstX2, fDstY2, fDstX3, fDstY3, fDstX4, fDstY4;// 旋转后四个角的坐标（以图像中心为坐标系原点）	

	fSrcX1 = (float)(-(lWidth - 1) / 2);// 计算原图的四个角的坐标
	fSrcY1 = (float)((lHeight - 1) / 2);
	fSrcX2 = (float)((lWidth - 1) / 2);
	fSrcY2 = (float)((lHeight - 1) / 2);
	fSrcX3 = (float)(-(lWidth - 1) / 2);
	fSrcY3 = (float)(-(lHeight - 1) / 2);
	fSrcX4 = (float)((lWidth - 1) / 2);
	fSrcY4 = (float)(-(lHeight - 1) / 2);

	fDstX1 = fCosa * fSrcX1 + fSina * fSrcY1;// 计算新图四个角的坐标
	fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	fDstX2 = fCosa * fSrcX2 + fSina * fSrcY2;
	fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	fDstX3 = fCosa * fSrcX3 + fSina * fSrcY3;
	fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	fDstX4 = fCosa * fSrcX4 + fSina * fSrcY4;
	fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;

	long	lDstWidth;      //临时图像的宽度和高度
	long	lDstHeight;
	long	lDstLineBytes;
	lDstWidth = (long)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);// 计算旋转后的图像实际宽度  像素
	lDstHeight = (long)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);// 计算旋转后的图像高度

	lDstLineBytes = (lDstWidth * pixelByte * 8 + 31) / 32 * 4;

	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	HGLOBAL	hNewDIBBits;
	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	LPBITMAPINFOHEADER	lpDstbmi;			// 指向旋转图像的指针

	hNewDIBBits = GlobalAlloc(GHND, lDstLineBytes *lDstHeight + lpbmi->biSize + palSize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIB = (LPBYTE)GlobalLock(hNewDIBBits);

	memcpy(lpNewDIB, lpbmi, lpbmi->biSize + palSize);

	// 初始化新分配的内存，设定初始值为0
	lpNewDIBBits = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);

	lpDstbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpDstbmi->biWidth = lDstWidth;
	lpDstbmi->biHeight = lDstHeight;

	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lDstLineBytes *lDstHeight);

	float varFloat1;        //浮点参数变量1
	float varFloat2;        //浮点参数变量2
	varFloat1 = (float)(-0.5 * (lDstWidth) * fCosa - 0.5 * (lDstHeight ) * fSina// 将经常用到的两个常数事先求出，以便作为常数使用
		+ 0.5 * (lWidth ));//dx
	varFloat2 = (float)(0.5 * (lDstWidth) * fSina - 0.5 * (lDstHeight) * fCosa
		+ 0.5 * (lHeight ));//dy

	//逐个扫面图像中的像素点,进行灰度线性变换
	for (i = 0; i < lDstHeight; i++)
	{
		for (j = 0; j < lDstWidth; j++)
		{
			lpDst = (LPBYTE)lpNewDIBBits + lDstLineBytes * i + j * pixelByte;
			i1 = (long)(-((float)j) * fSina + ((float)i) * fCosa + varFloat2 + 0.5);// y 计算该像素在源DIB中的坐标
			j1 = (long)(((float)j) * fCosa + ((float)i) * fSina + varFloat1 + 0.5);//x

			if ((j1 >= 0) && (j1 < lWidth) && (i1 >= 0) && (i1 < lHeight))
			{
				// 指向原图像倒数第j行，第i个像素的指针			
				lpSrc = (LPBYTE)lporiginImage + lLineBytes * i1 + j1 * pixelByte;
				for (int k = 0; k < pixelByte; k++)
					*(lpDst + k) = *(lpSrc + k);
			}
			else
			{
				*lpDst = 255;// 源图中不存在的像素，赋为255
			}
		}
	}
	pDib->AttachMemory(lpDstbmi, 0, hNewDIBBits);
	pDoc->UpdateAllViews(NULL);									// 更新视图	
}
double uniform_rand(double lowerBndr, double upperBndr)
{
	return lowerBndr + ((double)std::rand() / (RAND_MAX + 1.0)) * (upperBndr - lowerBndr);
}
double gauss_rand(double mean, double sigma)
{
	double x, y, r2;
	do {
		x = -1.0 + 2.0 * uniform_rand(0.0, 1.0);
		y = -1.0 + 2.0 * uniform_rand(0.0, 1.0);
		r2 = x * x + y * y;
	} while (r2 > 1.0 || r2 == 0.0);
	return mean + sigma * y * sqrt(-2.0 * log(r2) / r2);
}
void CImageProcessingView::OnAddnoise()
{
	int count = 1000;
	int size = 1;
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	LPBYTE lpSrc;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;

	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	long  i, j, k;
	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	LONG	lHeight;// 图象的高度
	LONG	lWidth;
	LONG	lLineBytes;// 图象每行的字节数
	SizeDim = pDib->GetDimensions();
	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;
	SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数

	//int xPosition;
	//int yPosition;
	//int psNoise;
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)0, lWidth * lHeight);

	//memcpy(lpNewDIBBits, lpDIBBits, lWidth * lHeight);
	////逐个扫面图像中的像素点,进行灰度线性变换
	//srand((unsigned)time(NULL));
	//while (count)
	//{
	//	xPosition = rand() % (lWidth - size + 1);
	//	yPosition = rand() % (lHeight - size + 1);
	//	psNoise = rand() % 2;
	//	if (psNoise)                                     // 产生盐点
	//		for (int i = 0; i < size; i++)
	//		{
	//			for (int j = 0; j < size; j++)
	//			{
	//				lpDst = (LPBYTE)lpNewDIBBits + lWidth * (yPosition + j) + xPosition + i;
	//				*lpDst = (BYTE)255;
	//			}
	//		}
	//	else                                            // 产生椒点
	//		for (int i = 0; i < size; i++)
	//		{
	//			for (int j = 0; j < size; j++)
	//			{
	//				lpDst = (LPBYTE)lpNewDIBBits + lWidth * (yPosition + j) + xPosition + i;
	//				*lpDst = (BYTE)0;
	//			}
	//		}
	//	count = count - 1;
	//}

	int int_pixel;
	BYTE pixel;	//像素值

	for (j = 0; j < lHeight; j++)
	{
		for (i = 0; i < lLineBytes; i++)
		{
			// 指向原图像倒数第j行，第i个像素的指针			
			lpSrc = (LPBYTE)lpDIBBits + lLineBytes * j + i;
			// 指向目标图像倒数第j行，第i个像素的指针			
			lpDst = (LPBYTE)lpNewDIBBits + lLineBytes * j + i;
			//取得当前指针处的像素值，注意要转换为BYTE类型
			pixel = (BYTE)*lpSrc;

			int_pixel = pixel + gauss_rand(2, 0.8) * 16;

			if (int_pixel > 255)
				int_pixel = 255;
			else if (int_pixel < 0)
				int_pixel = 0;
			//根据公式(5-2)求出目标图像中与当前点对应的像素点的灰度值

			*lpDst = (BYTE)(int_pixel);

		}
	}
	// 复制变换后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	pDoc->UpdateAllViews(NULL);									// 更新视图	*/
	//逐个扫面图像中的像素点,进行灰度线性变换


}

void CImageProcessingView::OnRemovenoise()
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	LPBYTE  lpDIBBits = pDib->m_lpImage;

	LPBYTE	lpSrc;
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	long i, j, k;			//循环变量
	BYTE average;	//领域均值变量

	int int_average;
	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	LONG	lHeight;// 图象的高度
	LONG	lWidth;
	LONG	lLineBytes;// 图象每行的字节数
	SizeDim = pDib->GetDimensions();
	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;
	SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数


	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);

	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	for (j = 0; j < lHeight; j++)
	{
		for (i = 0; i < lLineBytes; i++)
		{

			// 指向原图像倒数第j行，第i个像素的指针			
			lpSrc = (LPBYTE)lpDIBBits + lLineBytes * j + i;
			// 指向目标图像倒数第j行，第i个像素的指针			
			lpDst = (LPBYTE)lpNewDIBBits + lLineBytes * j + i;
			//取得当前指针处的像素值，注意要转换为BYTE类型
			if (i < pixelByte || i >= (lLineBytes - pixelByte) || j < 1 || j >= (lHeight - 1))
			{
				average = (BYTE)*lpSrc;
				*lpDst = (BYTE)average;
			}
			else
			{
				average = 0;
				int_average = 0;
				for (int n = -1; n <= 1; n++)
					for (int m = -pixelByte; m <= pixelByte; m += pixelByte)
					{
						int_average += (BYTE)(*(lpSrc + n * lLineBytes + m));
					}
				average = (BYTE)(int_average / 9);
				*lpDst = (BYTE)average;


			}
		}
	}
	// 复制变换后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	pDoc->UpdateAllViews(false);

}

void CImageProcessingView::OnScale(float scale)
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	LPBYTE  lpDIBBits = pDib->m_lpImage;

	LONG i, j, j1, i1;
	LPBYTE lpSrc;

	long	lDstWidth;      //临时图像的宽度和高度
	long	lDstHeight;
	long	lDstLineBytes;
	lDstWidth = (LONG)(lWidth *scale + 0.5); //计算缩放后的图像实际宽度, 加0.5是由于强制类型转换时不四舍五入，而是直接截去小数部分
	lDstHeight = (LONG)(lHeight*scale + 0.5);

	lDstLineBytes = (lDstWidth * pixelByte * 8 + 31) / 32 * 4;

	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	HGLOBAL	hNewDIBBits;
	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	LPBITMAPINFOHEADER	lpDstbmi;			// 指向旋转图像的指针

	hNewDIBBits = GlobalAlloc(GHND, lDstLineBytes *lDstHeight + lpbmi->biSize + palSize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIB = (LPBYTE)GlobalLock(hNewDIBBits);

	memcpy(lpNewDIB, lpbmi, lpbmi->biSize + palSize);

	// 初始化新分配的内存，设定初始值为0
	lpNewDIBBits = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);

	lpDstbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpDstbmi->biWidth = lDstWidth;
	lpDstbmi->biHeight = lDstHeight;

	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lDstLineBytes *lDstHeight);

	for (i = 0; i < lDstHeight; i++)
	{
		for (j = 0; j < lDstWidth; j++)
		{
			// 指向目标图像第j行，第i个像素的指针			
			lpDst = (LPBYTE)lpNewDIBBits + lDstLineBytes * i + j * pixelByte;
			i1 = (long)(i / scale + 0.5);
			j1 = (long)(j / scale + 0.5);

			if ((j1 >= 0) && (j1 < lWidth) && (i1 >= 0) && (i1 < lHeight))
			{
				// 指向原图像倒数第j行，第i个像素的指针			
				lpSrc = (LPBYTE)lporiginImage + lLineBytes * i1 + j1 * pixelByte;
				for (int k = 0; k < pixelByte; k++)
					*(lpDst + k) = *(lpSrc + k);
			}
			else
			{
				*lpDst = 255;// 源图中不存在的像素，赋为255
			}

		}
	}
	pDib->AttachMemory(lpDstbmi, 0, hNewDIBBits);
	pDoc->UpdateAllViews(NULL);									// 更新视图	

}

void CImageProcessingView::OnSharpen()
{
	// 获取文档
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;

	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;

	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	LONG	lHeight;// 图象的高度
	LONG	lWidth;
	LONG	lLineBytes;// 图象每行的字节数
	SizeDim = pDib->GetDimensions();
	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;
	SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数

	// 更改光标形状
	BeginWaitCursor();

	HLOCAL hNewDIBBits;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	LPBYTE lpSrc;
	// 循环变量
	LONG	i, j, k;
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes * lHeight);

	int x, y;
	BYTE t;
	// 计算各个灰度值的计数（对于非256色位图，此处给数组dProba赋值方法将不同）
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				if (i == 0 || i == lHeight - 1 || j == 0 || j == lWidth - 1)
				{	// 指向图象指针
					lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					*lpDst = (BYTE)*lpSrc;
				}
				else
				{
					x = *(lpDIBBits + (i - 1)*lLineBytes + (j + 1)*pixelByte + k)
						+ 2 * *(lpDIBBits + i * lLineBytes + (j + 1)*pixelByte + k)
						+ *(lpDIBBits + (i + 1)*lLineBytes + (j + 1)*pixelByte + k)
						- *(lpDIBBits + (i - 1)*lLineBytes + (j - 1)*pixelByte + k)
						- 2 * *(lpDIBBits + i * lLineBytes + (j - 1)*pixelByte + k)
						- *(lpDIBBits + (i + 1)*lLineBytes + (j - 1)*pixelByte + k);
					//y方向梯
					y = *(lpDIBBits + (i - 1)*lLineBytes + (j - 1)*pixelByte + k)
						+ 2 * *(lpDIBBits + (i - 1)*lLineBytes + j * pixelByte + k)
						+ *(lpDIBBits + (i - 1)*lLineBytes + (j + 1)*pixelByte + k)
						- *(lpDIBBits + (i + 1)*lLineBytes + (j - 1)*pixelByte + k)
						- 2 * *(lpDIBBits + (i + 1)*lLineBytes + j * pixelByte + k)
						- *(lpDIBBits + (i + 1)*lLineBytes + (j + 1)*pixelByte + k);
					x /= 4;
					y /= 4;
					t = (BYTE)(sqrt(x*x + y * y) + 0.5);
					if (t > 255)
						t = 255;
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;

					*lpDst = t;

				}
			}
		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	pDoc->UpdateAllViews(NULL);									// 更新视图		
		// 恢复光标
	EndWaitCursor();
}

void CImageProcessingView::OnDivide()
{
	// TODO: 在此添加命令处理程序代码
	CImageProcessingDoc * pDoc = GetDocument();

	LONG	lHeight;// 图象的高度
	LONG	lWidth;
	LONG	lLineBytes;// 图象每行的字节数
	LONG	lCountSum;// 图象象素总数
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	int		nColorNum; // 获取当前DIB颜色数目
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	int pixelByte;
	double *dProba;
	CImageOperation cio;
	LONG i, j, k;
	LPBYTE lpSrc;

	SizeDim = pDib->GetDimensions();
	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;
	lCountSum = lHeight * lWidth;// 计算图象象素总数
	SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数
	pixelByte = lpBMIH->biBitCount / 8;

	if (lpBMIH->biBitCount == 24)
		nColorNum = (int)pow(2.0f, lpBMIH->biBitCount / 3);
	else
		//  由头文件信息得到图象的比特数，从而得到颜色信息
		nColorNum = (int)pow(2.0f, lpBMIH->biBitCount);

	dProba = new double[nColorNum];
	memset(dProba, 0, sizeof(double)*nColorNum);
	cio.StaColor(lHeight, lWidth, lLineBytes, pixelByte, dProba, lpDIBBits);
	double T = 200;//阈值
	double T2 = 0;
	double G1 = 0, G2 = 0;
	//double g1sum = 0, g2sum = 0;
	int g1 = 0, g2 = 0;
	double u1 = 0, u2 = 0;
	while (abs(T2 - T) > 0.5)
	{
		if (T2 != 0)
			T = T2;
		for (i = 0; i < nColorNum; i++)
		{
			if (i < T)
			{
				G1 += dProba[i] * i;
				g1 += dProba[i];
			}
			else
			{
				G2 += dProba[i] * i;
				g2 += dProba[i];
			}
		}
		u1 = G1 / g1;
		u2 = G2 / g2;
		//计算新的阈值
		T2 = (u1 + u2) / 2;
	}

	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	hNewDIBBits = LocalAlloc(LHND, lLineBytes *lHeight);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes *lHeight);

	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lWidth; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{

				lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
				if (*lpSrc >= T2)
				{
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					*lpDst = *lpSrc;
				}

			}
		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	pDoc->UpdateAllViews(NULL);									// 更新视图	
}

void CImageProcessingView::OnRelief()
{
	// TODO: 在此添加命令处理程序代码
	CImageProcessingDoc * pDoc = GetDocument();

	LONG	lHeight;// 图象的高度
	LONG	lWidth;
	LONG	lLineBytes;// 图象每行的字节数
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	int		nColorNum; // 获取当前DIB颜色数目
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	int pixelByte;
	LONG i, j, k;
	LPBYTE lpSrc;

	SizeDim = pDib->GetDimensions();
	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;
	SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数
	pixelByte = lpBMIH->biBitCount / 8;

	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	hNewDIBBits = LocalAlloc(LHND, lLineBytes *lHeight);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lLineBytes *lHeight);

	for (i = 0; i < lHeight - 1; i++)
	{
		for (j = 0; j < lWidth - 1; j++)
		{
			int temp;
			for (k = 0; k < pixelByte; k++)
			{

				lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
				temp = *(lpDIBBits + lLineBytes * (i + 1) + (j + 1) * pixelByte + k);
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				*lpDst = *lpSrc - temp + 128;


			}
		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	//释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	pDoc->UpdateAllViews(NULL);									// 更新视图	
}

BOOL CImageProcessingView::OnConnectImage(float conncet)
{
	BOOL Updown = TRUE;
	CFileDialog dlg(TRUE, _T("BMP"), _T("*.BMP"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("位图文件(*.BMP)|*.BMP|"));
	CString  filename;
	CFile file;
	if (connect_Dib == NULL)
	{
		if (IDOK == dlg.DoModal())
		{
			filename.Format("%s", dlg.GetPathName());
			file.Open(filename, CFile::modeRead | CFile::shareDenyWrite);
			connect_Dib = new CDib;
			connect_Dib->Read(&file);
		}
		else
		{
			AfxMessageBox("未能打开文件");//通知提示框
			return FALSE;
		}

	}
	//图象数据的指针
	LPBYTE  lpCIDIBBits = connect_Dib->m_lpImage;
	// 头文件信息
	LPBITMAPINFOHEADER lpCIBMIH = connect_Dib->m_lpBMIH;

	CSize   CISizeDim;
	CSize   CISizeRealDim;	//得到实际的Dib图象存储大小
	int CIpixelByte;
	LONG	lCIHeight;// 图象的高度
	LONG	lCIWidth;
	LONG	lCILineBytes;// 图象每行的字节数
	CISizeDim = connect_Dib->GetDimensions();
	lCIWidth = CISizeDim.cx;
	lCIHeight = CISizeDim.cy;
	CISizeRealDim = connect_Dib->GetDibSaveDim();//单位为像素
	lCILineBytes = CISizeRealDim.cx;	// 计算图象每行的字节数
	CIpixelByte = lpCIBMIH->biBitCount / 8;

	// TODO: 在此添加命令处理程序代码
	CImageProcessingDoc * pDoc = GetDocument();
	CDib * pDib = pDoc->m_pDibInit;

	//LONG	lHeight;// 图象的高度
	//LONG	lWidth;
	//LONG	lLineBytes;// 图象每行的字节数
	////  获得图象CDib类的指针
	////图象数据的指针
	//LPBYTE  lpDIBBits = pDib->m_lpImage;
	//// 头文件信息
	//LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	//CSize   SizeDim;
	//CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	//int pixelByte;
	//SizeDim = pDib->GetDimensions();
	//lWidth = SizeDim.cx;
	//lHeight = SizeDim.cy;
	//SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	//lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数
	//pixelByte = lpBMIH->biBitCount / 8;

	if (pixelByte != CIpixelByte)
	{
		AfxMessageBox("色彩位数不同");//通知提示框
		return FALSE;
	}

	if (conncet > 1.0)
	{
		conncet -= 1.0;
		Updown = FALSE;
	}
	long	lDstWidth;      //临时图像的宽度和高度
	long	lDstHeight;
	long	lDstLineBytes;
	long    lDerTaHeight;
	if (Updown)
		lDerTaHeight = lCIHeight * conncet;
	else
	{
		lDerTaHeight = lHeight * conncet;
	}
	//if (lDerTaHeight > lCIHeight)
	//	lDerTaHeight = lCIHeight;
	(lWidth > lCIWidth) ? lDstWidth = lWidth : lDstWidth = lCIWidth;
	//lDstHeight = lHeight + lCIHeight - lDerTaHeight;
	lDstHeight = lHeight + lCIHeight;
	/*if (lDstHeight < lHeight || lDstHeight < lCIHeight)
		(lHeight > lCIHeight) ? lDstHeight = lHeight : lDstHeight = lCIHeight;*/
	lDstLineBytes = (lDstWidth * pixelByte * 8 + 31) / 32 * 4;

	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	HGLOBAL	hNewDIBBits;
	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	LPBITMAPINFOHEADER	lpDstbmi;			// 指向旋转图像的指针

	hNewDIBBits = GlobalAlloc(GHND, lDstLineBytes *lDstHeight + lpbmi->biSize + palSize);
	if (hNewDIBBits == NULL)
	{
		return FALSE;
	}

	lpNewDIB = (LPBYTE)GlobalLock(hNewDIBBits);

	memcpy(lpNewDIB, lpbmi, lpbmi->biSize + palSize);

	// 初始化新分配的内存，设定初始值为0
	lpNewDIBBits = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);

	lpDstbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpDstbmi->biWidth = lDstWidth;
	lpDstbmi->biHeight = lDstHeight;

	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lDstLineBytes *lDstHeight);

	LPBYTE	CI_lpSrc;
	LPBYTE templp;
	LPBYTE lpSrc;
	LONG i, j, k;


	templp = new BYTE[lDstLineBytes];

	if (Updown)
	{
		for (i = lDerTaHeight; i < lDerTaHeight + lHeight; i++)
		{
			lpSrc = (LPBYTE)lporiginImage + lLineBytes * (i - lDerTaHeight);
			lpDst = (LPBYTE)lpNewDIBBits + lDstLineBytes * i;
			//if (lLineBytes < lDstLineBytes)
			//{
			//	memset(templp, (BYTE)255, lDstLineBytes);
			//	memcpy(templp, lpSrc, lLineBytes);
			//	memcpy(lpDst, templp, lDstLineBytes);
			//}
			//else
			//{
			memcpy(lpDst, lpSrc, lLineBytes);

			//}

		}
		//for (i = lHeight - lDerTaHeight; i < lHeight - lDerTaHeight + lCIHeight; i++)
		for (i = lHeight; i < lDstHeight; i++)
		{
			CI_lpSrc = (LPBYTE)lpCIDIBBits + lCILineBytes * (i - lHeight);
			lpDst = (LPBYTE)lpNewDIBBits + lDstLineBytes * i;
			/*	if (lCILineBytes < lDstLineBytes)
				{
					memset(templp, (BYTE)255, lDstLineBytes);
					memcpy(templp, CI_lpSrc, lCILineBytes);
					memcpy(lpDst, templp, lDstLineBytes);
				}
				else
				{*/
			memcpy(lpDst, CI_lpSrc, lCILineBytes);

			//}

		}
	}
	else
	{
		for (i = lDstHeight - lHeight; i < lDstHeight; i++)
		{
			lpSrc = (LPBYTE)lporiginImage + lLineBytes * (i - lDstHeight + lHeight);
			lpDst = (LPBYTE)lpNewDIBBits + lDstLineBytes * i;
			memcpy(lpDst, lpSrc, lLineBytes);
		}
		for (i = lDstHeight - lDerTaHeight - lCIHeight; i < lDstHeight - lDerTaHeight; i++)
		{
			CI_lpSrc = (LPBYTE)lpCIDIBBits + lCILineBytes * (i - (lDstHeight - lDerTaHeight - lCIHeight));
			lpDst = (LPBYTE)lpNewDIBBits + lDstLineBytes * i;
			memcpy(lpDst, CI_lpSrc, lCILineBytes);

		}
	}
	pDib->AttachMemory(lpDstbmi, 0, hNewDIBBits);

	delete[] templp;
	templp = NULL;

	pDoc->UpdateAllViews(NULL);

}


void CImageProcessingView::OnHighlightAverage()
{
	// TODO: 在此添加命令处理程序代码
	CImageProcessingDoc * pDoc = GetDocument();

	LONG	lHeight;// 图象的高度
	LONG	lWidth;
	LONG	lLineBytes;// 图象每行的字节数
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	// 头文件信息
	LPBITMAPINFOHEADER lpBMIH = pDib->m_lpBMIH;
	int		nColorNum; // 获取当前DIB颜色数目
	CSize   SizeDim;
	CSize   SizeRealDim;	//得到实际的Dib图象存储大小
	int pixelByte;
	SizeDim = pDib->GetDimensions();
	lWidth = SizeDim.cx;
	lHeight = SizeDim.cy;
	SizeRealDim = pDib->GetDibSaveDim();//单位为像素
	lLineBytes = SizeRealDim.cx;	// 计算图象每行的字节数
	pixelByte = lpBMIH->biBitCount / 8;

	LONG i, j, k;
	LPBYTE lpSrc;
	LPBYTE	lpDst;
	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	HLOCAL	hNewDIBBits;
	hNewDIBBits = LocalAlloc(LHND, imagesize);
	if (hNewDIBBits == NULL)
	{
		return;
	}
	lpNewDIBBits = (LPBYTE)LocalLock(hNewDIBBits);
	// 初始化新分配的内存，设定初始值为0
	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, imagesize);
	//float v = tan((45 + 44 * alight) / 180 * PI);
	if (pixelByte >= 3)
	{
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{
				float temp = 0;
				for (k = 0; k < pixelByte; k++)
				{
					lpSrc = lpDIBBits + lLineBytes * i + j * pixelByte + k;
					BGR[k] = *lpSrc;
				}

				RGB_HSI();
				I *= 255.0;
				if (I > 150)I *= 0.9;
				I /= 255.0;
				if (I > 1.0)I = 1.0;
				if (I < 0.0)I = 0;
				HSI_RGB();
				for (k = 0; k < pixelByte; k++)
				{
					lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
					temp = BGR[k] * 255.0 + 0.5;
					if (temp > 255)temp = 255;
					if (temp < 0)temp = 0;
					*lpDst = (BYTE)temp;
				}

			}
		}
	}
	else
	{
		int temp = 0;
		for (i = 0; i < lHeight - 1; i++)
		{
			for (j = 0; j < lWidth - 1; j++)
			{

				lpSrc = lpDIBBits + lLineBytes * i + j;
				temp = *lpSrc;
				if (temp > 150)temp *= 0.9;
				if (temp > 255)temp = 255;
				if (temp < 0)temp = 0;
				lpDst = lpNewDIBBits + lLineBytes * i + j;
				*lpDst = (BYTE)temp;
			}

		}
	}
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes*lHeight);
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);

	pDoc->UpdateAllViews(false);
}

void CImageProcessingView::OnCut(float up, float down, float left, float right)
{
	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	//图象数据的指针
	LPBYTE  lpDIBBits = pDib->m_lpImage;
	LONG i, j, k;
	LPBYTE lpSrc;

	long	lDstWidth;      //临时图像的宽度和高度
	long	lDstHeight;
	long	lDstLineBytes;
	lDstWidth = (LONG)lWidth; //计算缩放后的图像实际宽度, 加0.5是由于强制类型转换时不四舍五入，而是直接截去小数部分
	lDstHeight = (LONG)lHeight;

	lDstLineBytes = lLineBytes;

	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	HGLOBAL	hNewDIBBits;
	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	LPBITMAPINFOHEADER	lpDstbmi;			// 指向旋转图像的指针

	hNewDIBBits = GlobalAlloc(GHND, lDstLineBytes *lDstHeight + lpbmi->biSize + palSize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIB = (LPBYTE)GlobalLock(hNewDIBBits);

	memcpy(lpNewDIB, lpbmi, lpbmi->biSize + palSize);

	// 初始化新分配的内存，设定初始值为0
	lpNewDIBBits = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);

	lpDstbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpDstbmi->biWidth = lDstWidth;
	lpDstbmi->biHeight = lDstHeight;

	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)175, lDstLineBytes *lDstHeight);

	LONG leftpixel = lWidth * left,
		rightpixel = lWidth * (1 - right),
		uppixel = lHeight * up,
		downpixel = lHeight * (1 - down);
	for (i = uppixel; i < downpixel; i++)
	{
		for (j = leftpixel; j < rightpixel; j++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i + j * pixelByte + k;
				lpDst = lpNewDIBBits + lLineBytes * i + j * pixelByte + k;
				*lpDst = *lpSrc;
			}

		}
	}
	pDib->AttachMemory(lpDstbmi, 0, hNewDIBBits);
	pDoc->UpdateAllViews(NULL);									// 更新视图	

	//memcpy(lpDIBBits, lpNewDIBBits, imagesize);
	//LocalUnlock(hNewDIBBits);
	//LocalFree(hNewDIBBits);
	//pDoc->UpdateAllViews(false);
}

void CImageProcessingView::OnApplyCut(float up, float down, float left, float right)
{
	LONG leftpixel = lWidth * left,
		rightpixel = lWidth * (1 - right),
		uppixel = lHeight * up,
		downpixel = lHeight * (1 - down);

	CImageProcessingDoc * pDoc = GetDocument();
	//  获得图象CDib类的指针
	CDib * pDib = pDoc->m_pDibInit;
	LPBYTE  lpDIBBits = pDib->m_lpImage;

	LONG i, j, j1, i1, k;
	LPBYTE lpSrc;

	long	lDstWidth;      //临时图像的宽度和高度
	long	lDstHeight;
	long	lDstLineBytes;
	lDstWidth = (LONG)(rightpixel - leftpixel); //计算缩放后的图像实际宽度, 加0.5是由于强制类型转换时不四舍五入，而是直接截去小数部分
	lDstHeight = (LONG)(downpixel - uppixel);

	lDstLineBytes = (lDstWidth * pixelByte * 8 + 31) / 32 * 4;

	LPBYTE	lpNewDIBBits;	// 指向缓存DIB图像的指针
	LPBYTE	lpDst;			// 指向缓存图像的指针	
	HGLOBAL	hNewDIBBits;
	LPBYTE	lpNewDIB;			// 指向旋转图像的指针
	LPBITMAPINFOHEADER	lpDstbmi;			// 指向旋转图像的指针

	hNewDIBBits = GlobalAlloc(GHND, lDstLineBytes *lDstHeight + lpbmi->biSize + palSize);
	if (hNewDIBBits == NULL)
	{
		return;
	}

	lpNewDIB = (LPBYTE)GlobalLock(hNewDIBBits);

	memcpy(lpNewDIB, lpbmi, lpbmi->biSize + palSize);

	// 初始化新分配的内存，设定初始值为0
	lpNewDIBBits = lpNewDIB + *(LPDWORD)lpNewDIB + palSize;;//FindDIBBits(lpNewDIB);

	lpDstbmi = (LPBITMAPINFOHEADER)lpNewDIB;
	lpDstbmi->biWidth = lDstWidth;
	lpDstbmi->biHeight = lDstHeight;

	lpDst = (LPBYTE)lpNewDIBBits;
	memset(lpDst, (BYTE)255, lDstLineBytes *lDstHeight);

	for (i = 0, i1 = uppixel; i < lDstHeight; i++, i1++)
	{
		for (j = 0, j1 = leftpixel; j < lDstWidth; j++, j1++)
		{
			for (k = 0; k < pixelByte; k++)
			{
				lpSrc = lporiginImage + lLineBytes * i1 + j1 * pixelByte + k;
				lpDst = lpNewDIBBits + lDstLineBytes * i + j * pixelByte + k;
				*lpDst = *lpSrc;
			}
		}
	}
	pDib->AttachMemory(lpDstbmi, 0, hNewDIBBits);
	pDoc->UpdateAllViews(NULL);									// 更新视图	



}


