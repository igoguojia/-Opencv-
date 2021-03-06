#pragma once

#include "stdafx.h"
#include"Cdib.h"
#include "ImageProcessingView.h"
// CFunctions 对话框
//#define PI acos(-1)
class CFunctions : public CDialogEx
{
	DECLARE_DYNAMIC(CFunctions)

public:
	CFunctions(CImageProcessingView* pParent);   // 标准构造函数
	virtual ~CFunctions();
	CImageProcessingView *pView;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
public:
	void upEdit();

public:
	//BasisTool
	CSliderCtrl SLI_bright;
	CSliderCtrl SLI_contrast;
	CSliderCtrl SLI_saturation;
	CSliderCtrl SLI_clarity;
	CString EDIT_bright;
	CString EDIT_contrast;
	CString EDIT_saturation;
	CString EDIT_clarity;
	void recoverSlider_1();
	afx_msg void OnBnRecovervalue_1();
	BOOL BasisTool = FALSE;

public:
	//SeniorTool
	CSliderCtrl SLI_autolight;
	CString EDIT_autolight;
	CSliderCtrl SLI_highlight;
	CString EDIT_highlight;
	CSliderCtrl SLI_shadow;
	CString EDIT_shadow;
	void recoverSlider_2();
	afx_msg void OnBnRecovervalue_2();
	BOOL SeniorTool = FALSE;
public:
	//operation
	afx_msg void OnBnRecover();
//	afx_msg void OnBnClickedMirror();

public:
	//底部功能
	afx_msg void OnBnClickedApply();
//	afx_msg void OnBnSave();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnCancel();
public:
	CSliderCtrl SLI_red;
	CSliderCtrl SLI_green;
	CSliderCtrl SLI_blue;
	BOOL HueTool = FALSE;

	void recoverSlider_3();
	afx_msg void OnBnClickedRecovervalue3();

};
