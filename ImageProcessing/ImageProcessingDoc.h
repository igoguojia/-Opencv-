
// ImageProcessingDoc.h : CImageProcessingDoc ��Ľӿ�
//


#pragma once

#include "CDib.h"


class CImageProcessingDoc : public CDocument
{
protected: // �������л�����
	CImageProcessingDoc();
	DECLARE_DYNCREATE(CImageProcessingDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	int m_nColorIndex;		// α��ɫ������
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

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileReload();
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
