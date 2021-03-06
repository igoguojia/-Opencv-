
// ImageProcessingDoc.cpp : CImageProcessingDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "ImageProcessing.h"
#endif

#include "ImageProcessingDoc.h"
#include "GlobalApi.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageProcessingDoc

IMPLEMENT_DYNCREATE(CImageProcessingDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessingDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_RELOAD, OnFileReload)
END_MESSAGE_MAP()


// CImageProcessingDoc ����/����

CImageProcessingDoc::CImageProcessingDoc()
{
	// TODO: �ڴ����һ���Թ������
	InitDocVariable();
}

CImageProcessingDoc::~CImageProcessingDoc()
{
	FreeDocVariable();

}

void CImageProcessingDoc::UpdateObject(HGLOBAL hDIB)
{
	if (m_pDibInit != NULL)               // �ж�DIB�Ƿ�Ϊ��
	{
		delete m_pDibInit;// �ǿգ������
		m_pDibInit = NULL;

	}
	//m_pDibInit = hDIB;                     // �滻���µ�DIB����	
}

BOOL CImageProcessingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CImageProcessingDoc ���л�

void CImageProcessingDoc::Serialize(CArchive& ar)
{
		m_pDibInit->Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CImageProcessingDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CImageProcessingDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CImageProcessingDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageProcessingDoc ���

#ifdef _DEBUG
void CImageProcessingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageProcessingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageProcessingDoc ����

int CImageProcessingDoc::InitDocVariable()
{
	m_pDibInit = new CDib;

	// added by tanhc in 2002-7-30, used to test some function
	m_pDibTest = new CDib;

	m_nColorIndex = 0;

	//--------------------------------------------

	return 0;
}

int CImageProcessingDoc::FreeDocVariable()
{
	delete m_pDibInit;
	m_pDibInit = NULL;
	delete m_pDibTest ;
	m_pDibTest = NULL ;

	//--------------------------------------------

	return 0;
}

void CImageProcessingDoc::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
	CString strSaveFileType = "λͼ�ļ� (*.bmp;*.dib)|*.bmp; *.dib|All Files (*.*)|*.*||";
	CFileDialog FileDlg(FALSE, "*.bmp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, strSaveFileType);

	CFile fileOpen ;
	if( FileDlg.DoModal() == IDOK ) {
		if(!fileOpen.Open( FileDlg.GetPathName() , CFile::modeCreate|CFile::modeWrite )){
			AfxMessageBox("cannot create the file to save");
			return;
		}
		if( !m_pDibInit->Write( &fileOpen ) ){
			return;
		}
		fileOpen.Close();
	}
	
}
//---------------------------------------
//\�������ƣ�
//  OnFileReload()
//\�������:
//   ��
//
//\����ֵ:
//   ��
//
// \˵��:
//  �ú�����ӦID_FILE_RELOAD�������Ϣ��������DIB
//---------------------------------------
void CImageProcessingDoc::OnFileReload() 
{
	// �жϵ�ǰͼ���Ƿ��Ѿ����Ķ�
	if (IsModified())
	{
		// ��ʾ�û��ò�������ʧ���е�ǰ���޸�
		if (MessageBox(NULL, "���´�ͼ�񽫶�ʧ���иĶ����Ƿ������", "ϵͳ��ʾ", MB_ICONQUESTION | MB_YESNO) == IDNO)
		{
			// �û�ȡ��������ֱ�ӷ���
			return;
		}
	}
	CFile file;

	CString strPathName;
	CFileException fe;

	strPathName = GetPathName();

	// ���´��ļ�
	if( !file.Open(strPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		// ʧ��
		ReportSaveLoadException(strPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);

		// ����
		return;
	}

	// ���Ĺ��
	BeginWaitCursor();

	// ���¶�ȡͼ��
	if(!m_pDibInit->Read(&file)){
		// �ָ������״
 		EndWaitCursor();
 				
 		 		
 		// ����
 		return;
	}

	// ��ʼ���ͱ��ΪFALSE
 	SetModifiedFlag(FALSE);
 	
 	// ˢ��
 	UpdateAllViews(NULL);
  	
 	// �ָ������״
 	EndWaitCursor();
	
 	// ����
 	return;
}
