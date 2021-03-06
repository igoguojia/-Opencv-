#pragma once

#include "stdafx.h"
#include"Cdib.h"
#include "ImageProcessingView.h"
// COperation 对话框

class COperation : public CDialogEx
{
	DECLARE_DYNAMIC(COperation)

public:
	COperation(CImageProcessingView* pParent);   // 标准构造函数
	virtual ~COperation();
	CImageProcessingView *pView;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	void upEdit();
	void recoverSlider();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRecover();
	afx_msg void OnBnClickedUdmirror();
//	afx_msg void OnBnClickedRotate();
	CSliderCtrl SLI_rotate;
	CString EDIT_rotate;
	int temp = 0;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedAddnoise();
	CSliderCtrl SLI_scale;
	CString EDIT_scale;
	afx_msg void OnBnClickedRemovenoise();
	afx_msg void OnBnClickedSharpen();
	afx_msg void OnBnClickedDivide();
	afx_msg void OnBnClickedRelief();
	afx_msg void OnBnClickedConnectimage();
	CSliderCtrl SLI_conncet;
	CString EDIT_connect;
	afx_msg void OnBnClickedHighlightaverage();
	afx_msg void OnBnClickedApply();

	CSliderCtrl SLI_up;
	CSliderCtrl SLI_down;
	CSliderCtrl SLI_left;
	CSliderCtrl SLI_right;
	afx_msg void OnBnClickedCut();

	CString EDIT_up;
	CString EDIT_down;
	CString EDIT_left;
	CString EDIT_right;
};
