// COperation.cpp: 实现文件
//

#include "stdafx.h"
#include "ImageProcessing.h"
#include "COperation.h"
#include "afxdialogex.h"


// COperation 对话框

IMPLEMENT_DYNAMIC(COperation, CDialogEx)

COperation::COperation(CImageProcessingView* pParent/*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, EDIT_rotate(_T(""))
	, EDIT_scale(_T(""))
	, EDIT_connect(_T(""))
{
	pView = pParent;

}

COperation::~COperation()
{
	pView = NULL;
}

void COperation::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIRotate, SLI_rotate);
	DDX_Text(pDX, IDC_EDITRotate, EDIT_rotate);
	DDX_Control(pDX, IDC_SLIScale, SLI_scale);
	DDX_Text(pDX, IDC_EDITScale, EDIT_scale);
	DDX_Control(pDX, IDC_SLIConnect, SLI_conncet);
	DDX_Text(pDX, IDC_EDITConnect, EDIT_connect);

	DDX_Control(pDX, IDC_SLIUP, SLI_up);
	DDX_Control(pDX, IDC_SLIDOWN, SLI_down);
	DDX_Control(pDX, IDC_SLILEFT, SLI_left);
	DDX_Control(pDX, IDC_SLIRIGHT, SLI_right);
	DDX_Text(pDX, IDC_EDITUp, EDIT_up);
	DDX_Text(pDX, IDC_EDITDown, EDIT_down);
	DDX_Text(pDX, IDC_EDITLeft, EDIT_left);
	DDX_Text(pDX, IDC_EDITRight, EDIT_right);

}


BEGIN_MESSAGE_MAP(COperation, CDialogEx)
	ON_BN_CLICKED(IDC_RECOVER, &COperation::OnBnClickedRecover)
	ON_BN_CLICKED(IDC_UDMIRROR, &COperation::OnBnClickedUdmirror)
	//ON_BN_CLICKED(IDC_ROTATE, &COperation::OnBnClickedRotate)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_ADDNOISE, &COperation::OnBnClickedAddnoise)
	ON_BN_CLICKED(IDC_REMOVENOISE, &COperation::OnBnClickedRemovenoise)
	ON_BN_CLICKED(IDC_SHARPEN, &COperation::OnBnClickedSharpen)
	ON_BN_CLICKED(IDC_DIVIDE, &COperation::OnBnClickedDivide)
	ON_BN_CLICKED(IDC_RELIEF, &COperation::OnBnClickedRelief)
	ON_BN_CLICKED(IDC_CONNECTIMAGE, &COperation::OnBnClickedConnectimage)
	ON_BN_CLICKED(IDC_HIGHLIGHTAVERAGE, &COperation::OnBnClickedHighlightaverage)
	ON_BN_CLICKED(IDC_APPLY, &COperation::OnBnClickedApply)
	ON_BN_CLICKED(IDC_CUT, &COperation::OnBnClickedCut)
END_MESSAGE_MAP()


// COperation 消息处理程序

BOOL COperation::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SLI_rotate.SetRange(0, 360);
	//SLI_rotate.SetPos(0);
	SLI_scale.SetRange(50, 300);
	SLI_scale.SetPos(100);

	SLI_conncet.SetRange(0, 200);
	SLI_conncet.SetPos(0);

	SLI_up.SetRange(0, 100);
	SLI_up.SetPos(0);

	SLI_down.SetRange(0, 100);
	SLI_down.SetPos(0);

	SLI_left.SetRange(0, 100);
	SLI_left.SetPos(0);

	SLI_right.SetRange(0, 100);
	SLI_right.SetPos(0);


	upEdit();



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
void COperation::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	pView->OnCancel_2();
	CDialogEx::OnCancel();
	pView->OnDestroyWin_2();
}
void COperation::OnOK()
{

	// TODO: 在此添加专用代码和/或调用基类
	CDialogEx::OnOK();
	pView->OnDestroyWin_2();

}
void COperation::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
	UpdateData(true);
	if ((&SLI_rotate) == (CSliderCtrl *)pScrollBar)         // mSlider 为你的slider控件
	{
		pView->OnRotate(SLI_rotate.GetPos());
		SLI_scale.SetPos(100);
		SLI_conncet.SetPos(0);
		if (pView->connect_Dib)
		{
			delete  pView->connect_Dib;
			pView->connect_Dib = NULL;
		}
		SLI_conncet.EnableWindow(false);
		GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(TRUE);
		SLI_up.SetPos(0);
		SLI_down.SetPos(0);
		SLI_left.SetPos(0);
		SLI_right.SetPos(0);
	}
	else if ((&SLI_scale) == (CSliderCtrl *)pScrollBar)         // mSlider 为你的slider控件
	{
		pView->OnScale((float)SLI_scale.GetPos()/100);
		SLI_rotate.SetPos(0);
		SLI_conncet.SetPos(0);
		if (pView->connect_Dib)
		{
			delete  pView->connect_Dib;
			pView->connect_Dib = NULL;
		}
		SLI_conncet.EnableWindow(false);
		GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(TRUE);

		SLI_up.SetPos(0);
		SLI_down.SetPos(0);
		SLI_left.SetPos(0);
		SLI_right.SetPos(0);
	}
	else if ((&SLI_conncet) == (CSliderCtrl *)pScrollBar)         // mSlider 为你的slider控件
	{
		pView->OnConnectImage((float)SLI_conncet.GetPos() / 100);
		SLI_rotate.SetPos(0);
		SLI_scale.SetPos(100);
	/*	if (pView->connect_Dib)
		{
			delete  pView->connect_Dib;
			pView->connect_Dib = NULL;
		}
		SLI_conncet.EnableWindow(false);
		GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(TRUE);
*/
		SLI_up.SetPos(0);
		SLI_down.SetPos(0);
		SLI_left.SetPos(0);
		SLI_right.SetPos(0);
	}
	else if ((&SLI_up) == (CSliderCtrl *)pScrollBar)         // mSlider 为你的slider控件
	{
		if ((SLI_up.GetPos() + SLI_down.GetPos()) > 100)
			SLI_up.SetPos(100 - SLI_down.GetPos());

		pView->OnCut((float)SLI_up.GetPos() / 100, 
			(float)SLI_down.GetPos() / 100, 
			(float)SLI_left.GetPos() / 100,
			(float)SLI_right.GetPos() / 100);
		SLI_rotate.SetPos(0);
		SLI_scale.SetPos(100);
		SLI_conncet.SetPos(0);

		if (pView->connect_Dib)
		{
			delete  pView->connect_Dib;
			pView->connect_Dib = NULL;
		}
		SLI_conncet.EnableWindow(false);
		GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(TRUE);
	}
	else if ((&SLI_down) == (CSliderCtrl *)pScrollBar)         // mSlider 为你的slider控件
	{
		if ((SLI_up.GetPos() + SLI_down.GetPos()) > 100)
			SLI_down.SetPos(100 - SLI_up.GetPos());
		pView->OnCut((float)SLI_up.GetPos() / 100,
			(float)SLI_down.GetPos() / 100,
			(float)SLI_left.GetPos() / 100,
			(float)SLI_right.GetPos() / 100);
		SLI_rotate.SetPos(0);
		SLI_scale.SetPos(100);
		SLI_conncet.SetPos(0);

		if (pView->connect_Dib)
		{
			delete  pView->connect_Dib;
			pView->connect_Dib = NULL;
		}
		SLI_conncet.EnableWindow(false);
		GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(TRUE);
	}
	else if ((&SLI_left) == (CSliderCtrl *)pScrollBar)         // mSlider 为你的slider控件
	{
		if ((SLI_left.GetPos() + SLI_right.GetPos()) > 100)
			SLI_left.SetPos(100 - SLI_right.GetPos());

		pView->OnCut((float)SLI_up.GetPos() / 100,
			(float)SLI_down.GetPos() / 100,
			(float)SLI_left.GetPos() / 100,
			(float)SLI_right.GetPos() / 100);
		SLI_rotate.SetPos(0);
		SLI_scale.SetPos(100);
		SLI_conncet.SetPos(0);

		if (pView->connect_Dib)
		{
			delete  pView->connect_Dib;
			pView->connect_Dib = NULL;
		}
		SLI_conncet.EnableWindow(false);
		GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(TRUE);
	}
	else if ((&SLI_right) == (CSliderCtrl *)pScrollBar)         // mSlider 为你的slider控件
	{
	if ((SLI_left.GetPos() + SLI_right.GetPos()) > 100)
		SLI_right.SetPos(100 - SLI_left.GetPos());
		pView->OnCut((float)SLI_up.GetPos() / 100,
			(float)SLI_down.GetPos() / 100,
			(float)SLI_left.GetPos() / 100,
			(float)SLI_right.GetPos() / 100);
		SLI_rotate.SetPos(0);
		SLI_scale.SetPos(100);
		SLI_conncet.SetPos(0);

		if (pView->connect_Dib)
		{
			delete  pView->connect_Dib;
			pView->connect_Dib = NULL;
		}
		SLI_conncet.EnableWindow(false);
		GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(TRUE);
	}
	upEdit();
	UpdateData(false);

}

void COperation::upEdit()
{
	EDIT_rotate.Format("%d°", SLI_rotate.GetPos());
	EDIT_scale.Format("%d%%", SLI_scale.GetPos());
	EDIT_connect.Format("%d%%", SLI_conncet.GetPos());

	EDIT_up.Format("%d%%", SLI_up.GetPos());
	EDIT_down.Format("%d%%", SLI_down.GetPos());
	EDIT_left.Format("%d%%", SLI_left.GetPos());
	EDIT_right.Format("%d%%", SLI_right.GetPos());

	UpdateData(false);

}

void COperation::recoverSlider()
{
	SLI_rotate.SetPos(0);
	SLI_scale.SetPos(100);
	SLI_conncet.SetPos(0);

	if (pView->connect_Dib)
	{
		delete  pView->connect_Dib;
		pView->connect_Dib = NULL;
	}
	SLI_conncet.EnableWindow(false);
	GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(TRUE);

	SLI_up.SetPos(0);
	SLI_down.SetPos(0);
	SLI_left.SetPos(0);
	SLI_right.SetPos(0);
	UpdateData(false);
}

void COperation::OnBnClickedRecover()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox("该操作将放弃所有更改！但不会退出", "提示", MB_OKCANCEL) == IDOK)
	{
		pView->OnRecover_2();
		recoverSlider();
		upEdit();
	}
}


void COperation::OnBnClickedUdmirror()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->OnUDMirror();
}



void COperation::OnBnClickedAddnoise()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->OnAddnoise();
}


void COperation::OnBnClickedRemovenoise()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->OnRemovenoise();
}


void COperation::OnBnClickedSharpen()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->OnSharpen();
}


void COperation::OnBnClickedDivide()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->OnDivide();
}


void COperation::OnBnClickedRelief()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->OnRelief();
}


void COperation::OnBnClickedConnectimage()
{
	// TODO: 在此添加控件通知处理程序代码
	if (pView->OnConnectImage((float)SLI_conncet.GetPos() / 100))
	{
		SLI_conncet.EnableWindow(true);
		GetDlgItem(IDC_CONNECTIMAGE)->EnableWindow(FALSE); //禁用按钮
	}
}


void COperation::OnBnClickedHighlightaverage()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->OnHighlightAverage();
}


void COperation::OnBnClickedApply()
{
	// TODO: 在此添加控件通知处理程序代码
	if (MessageBox("该操作将应用更改！'还原'操作将无效！但退出会还原图像", "提示", MB_OKCANCEL) == IDOK)
	{
		recoverSlider();
		upEdit();
		pView->OnApply_2();

	}
}


void COperation::OnBnClickedCut()
{
	// TODO: 在此添加控件通知处理程序代码
	pView->OnApplyCut((float)SLI_up.GetPos() / 100,
		(float)SLI_down.GetPos() / 100,
		(float)SLI_left.GetPos() / 100,
		(float)SLI_right.GetPos() / 100);
	recoverSlider();
	upEdit();

}
