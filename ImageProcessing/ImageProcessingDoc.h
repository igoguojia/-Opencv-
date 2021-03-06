
// ImageProcessingDoc.h : CImageProcessingDoc 类的接口
//


#pragma once

#include "CDib.h"


class CImageProcessingDoc : public CDocument
{
protected: // 仅从序列化创建
	CImageProcessingDoc();
	DECLARE_DYNCREATE(CImageProcessingDoc)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	int m_nColorIndex;		// 伪彩色编码编号
	CDib* m_pDibTest;
	int InitDocVariable();
	int FreeDocVariable();
	CDib* m_pDibInit;
	virtual ~CImageProcessingDoc();
	void UpdateObject(HGLOBAL hDIB);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileReload();
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
