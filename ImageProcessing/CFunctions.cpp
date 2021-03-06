// CFunctions.cpp: 实现文件
//

#include "stdafx.h"
#include "ImageProcessing.h"
#include "CFunctions.h"
#include "ImageProcessingView.h"
#include <cmath>

//#include "ImageProcessingView.h"
// CFunctions 对话框

IMPLEMENT_DYNAMIC(CFunctions, CDialogEx)

CFunctions::CFunctions(CImageProcessingView* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, EDIT_bright(_T(""))
	, EDIT_contrast(_T(""))
	, EDIT_saturation(_T(""))
	, EDIT_clarity(_T(""))
	, EDIT_autolight(_T(""))
	, EDIT_highlight(_T(""))
	, EDIT_shadow(_T(""))
{
	pView = pParent;

}

CFunctions::~CFunctions()
{
	pView = NULL;

}

void CFunctions::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIBright, SLI_bright);
	DDX_Control(pDX, IDC_SLIContrast, SLI_contrast);
	DDX_Control(pDX, IDC_SLISaturation, SLI_saturation);
	DDX_Control(pDX, IDC_SLIClarity, SLI_clarity);
	DDX_Text(pDX, IDC_EDITBright, EDIT_bright);
	DDX_Text(pDX, IDC_EDITContrast, EDIT_contrast);
	DDX_Text(pDX, IDC_EDITSaturation, EDIT_saturation);
	DDX_Text(pDX, IDC_EDITClarity, EDIT_clarity);
	DDX_Control(pDX, IDC_SLIAutoLight, SLI_autolight);
	DDX_Text(pDX, IDC_EDITAutoLight, EDIT_autolight);
	DDX_Control(pDX, IDC_SLIHighlight, SLI_highlight);
	DDX_Text(pDX, IDC_EDITHighlight, EDIT_highlight);
	DDX_Control(pDX, IDC_SLIShadow, SLI_shadow);
	DDX_Text(pDX, IDC_EDITShadow, EDIT_shadow);
	DDX_Control(pDX, IDC_SLIRed, SLI_red);
	DDX_Control(pDX, IDC_SLIGreen, SLI_green);
	DDX_Control(pDX, IDC_SLIBlue, SLI_blue);
}


BEGIN_MESSAGE_MAP(CFunctions, CDialogEx)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_RECOVERVALUE1, &CFunctions::OnBnRecovervalue_1)
	ON_BN_CLICKED(IDC_RECOVERVALUE2, &CFunctions::OnBnRecovervalue_2)
	//ON_BN_CLICKED(IDC_SAVE, &CFunctions::OnBnSave)
	ON_BN_CLICKED(IDC_RECOVER, &CFunctions::OnBnRecover)
	//ON_BN_CLICKED(IDC_MIRROR, &CFunctions::OnBnClickedMirror)
	ON_BN_CLICKED(IDC_APPLY, &CFunctions::OnBnClickedApply)
	ON_BN_CLICKED(IDC_RECOVERVALUE3, &CFunctions::OnBnClickedRecovervalue3)
END_MESSAGE_MAP()


// CFunctions 消息处理程序


BOOL CFunctions::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	UpdateData(true);
	SLI_bright.SetRange(-100, 100);//设置范围
	SLI_bright.SetLineSize(1);

	SLI_bright.SetPos(0);//设滑块值	

	SLI_contrast.SetRange(-100, 100);//设置范围
	SLI_contrast.SetPos(0);

	SLI_saturation.SetRange(0, 400);//设置范围
	SLI_saturation.SetPos(100);//设滑块值

	SLI_clarity.SetRange(0, 400);//设置范围
	SLI_clarity.SetPos(0);//设滑块值

	SLI_autolight.SetRange(0, 100);
	SLI_autolight.SetPos(0);

	SLI_highlight.SetRange(0, 200);
	SLI_highlight.SetPos(0);

	SLI_shadow.SetRange(0, 200);
	SLI_shadow.SetPos(0);

	SLI_red.SetRange(180, 360);
	SLI_red.SetPos(180);

	SLI_green.SetRange(300, 480);
	SLI_green.SetPos(300);

	SLI_blue.SetRange(420, 600);
	SLI_blue.SetPos(420);


	upEdit();


	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CFunctions::upEdit()
{
	EDIT_bright.Format("%d%%", SLI_bright.GetPos());
	EDIT_contrast.Format("%d%%", SLI_contrast.GetPos());
	EDIT_saturation.Format("%d%%", SLI_saturation.GetPos());
	EDIT_clarity.Format("%d%%", SLI_clarity.GetPos());
	EDIT_autolight.Format("%d%%", SLI_autolight.GetPos());
	EDIT_highlight.Format("%d%%", SLI_highlight.GetPos());
	EDIT_shadow.Format("%d%%", SLI_shadow.GetPos());
	UpdateData(false);

}


void CFunctions::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateData(true);
	if ((&SLI_bright) == (CSliderCtrl *)pScrollBar)         // mSlider 为你的slider控件
	{
		BasisTool = TRUE;
		pView->OnContrast((float)SLI_bright.GetPos() / 100, (float)SLI_contrast.GetPos() / 100);
		EDIT_bright.Format("%d%%", SLI_bright.GetPos());

	}
	else if ((&SLI_contrast) == (CSliderCtrl *)pScrollBar)
	{
		BasisTool = TRUE;
		pView->OnContrast((float)SLI_bright.GetPos() / 100, (float)SLI_contrast.GetPos() / 100);
		EDIT_contrast.Format("%d%%", SLI_contrast.GetPos());

	}
	else if ((&SLI_saturation) == (CSliderCtrl *)pScrollBar)
	{
		BasisTool = TRUE;
		pView->OnSaturation((float)SLI_saturation.GetPos() / 100);
		EDIT_saturation.Format("%d%%", SLI_saturation.GetPos());

	}
	else if ((&SLI_clarity) == (CSliderCtrl *)pScrollBar)
	{
		BasisTool = TRUE;
		pView->OnClarity((float)SLI_clarity.GetPos() / 100 + 0.5);
		EDIT_clarity.Format("%d%%", SLI_clarity.GetPos());

	}
	else if ((&SLI_autolight) == (CSliderCtrl *)pScrollBar)
	{
		SeniorTool = TRUE;
		pView->OnAutoLight((double)SLI_autolight.GetPos() / 1000);
		EDIT_autolight.Format("%d%%", SLI_autolight.GetPos());

	}
	else if ((&SLI_highlight) == (CSliderCtrl *)pScrollBar)
	{
		SeniorTool = TRUE;
		pView->OnHighLight((float)SLI_highlight.GetPos() / 100);
		EDIT_highlight.Format("%d%%", SLI_highlight.GetPos());

	}
	else if ((&SLI_shadow) == (CSliderCtrl *)pScrollBar)
	{
		SeniorTool = TRUE;
		pView->OnShadow((float)SLI_shadow.GetPos() / 100);
		EDIT_shadow.Format("%d%%", SLI_shadow.GetPos());

	}
	else if ((&SLI_red) == (CSliderCtrl *)pScrollBar)
	{
		HueTool = TRUE;
		pView->OnHue(SLI_red.GetPos());
		SLI_green.SetPos(300);
		SLI_blue.SetPos(420);
	}
	else if ((&SLI_green) == (CSliderCtrl *)pScrollBar)
	{
		HueTool = TRUE;
		pView->OnHue(SLI_green.GetPos());
		SLI_red.SetPos(180);
		SLI_blue.SetPos(420);

	}
	else if ((&SLI_blue) == (CSliderCtrl *)pScrollBar)
	{
		HueTool = TRUE;
		pView->OnHue(SLI_blue.GetPos());
		SLI_red.SetPos(180);
		SLI_green.SetPos(300);

	}
	pView->UpdateImage();
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpdateData(false);


}
void CFunctions::recoverSlider_1()
{

	SLI_bright.SetPos(0);//设滑块值	

	SLI_contrast.SetPos(0);

	SLI_saturation.SetPos(100);//设滑块值

	SLI_clarity.SetPos(0);//设滑块值

}

void CFunctions::OnBnRecovervalue_1()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->RecoverImage_1();
	recoverSlider_1();
	upEdit();
}


void CFunctions::recoverSlider_2()
{
	SLI_autolight.SetPos(0);
	SLI_highlight.SetPos(0);
	SLI_shadow.SetPos(0);
}

void CFunctions::OnBnRecovervalue_2()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->RecoverImage_2();
	recoverSlider_2();
	upEdit();
}


void CFunctions::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	pView->OnCancel();
	CDialogEx::OnCancel();
	pView->DestoryWin();
}


void CFunctions::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	CDialogEx::OnOK();
	pView->DestoryWin();
}


BOOL CFunctions::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//对比
	if (pMsg->message == WM_LBUTTONDOWN)//鼠标在按钮上按下
	{
		if (pMsg->hwnd == GetDlgItem(IDC_COMPARE)->m_hWnd)//判断被按下的按钮，参数是控件ID
		{
			pView->OnCompare(pMsg->message);
		}

	}
	else if (pMsg->message == WM_LBUTTONUP)//鼠标在按钮上抬起
	{
		if (pMsg->hwnd == GetDlgItem(IDC_COMPARE)->m_hWnd)
		{
			pView->OnCompare(pMsg->message);
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


//void CFunctions::OnBnSave()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}


void CFunctions::OnBnRecover()//还原
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox("该操作将放弃所有更改！但不会退出", "提示", MB_OKCANCEL) == IDOK)
	{
		recoverSlider_1();
		recoverSlider_2();
		upEdit();
		pView->OnRecover();
	}

}


//void CFunctions::OnBnClickedMirror()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	pView->OnMirror();
//}


void CFunctions::OnBnClickedApply()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox("该操作将应用更改！'重置''还原'操作将无效！！但退出会还原图像", "提示", MB_OKCANCEL) == IDOK)
	{
		recoverSlider_1();
		recoverSlider_2();
		upEdit();
		pView->OnApply();

	}

}

void CFunctions::recoverSlider_3()
{
	SLI_red.SetPos(180);
	SLI_green.SetPos(300);
	SLI_blue.SetPos(420);
}

void CFunctions::OnBnClickedRecovervalue3()
{
	// TODO: 在此添加控件通知处理程序代码.
	pView->RecoverImage_3();
	recoverSlider_3();
	UpdateData(false);

}
