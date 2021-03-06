
// ImageProcessingDoc.cpp : CImageProcessingDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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


// CImageProcessingDoc 构造/析构

CImageProcessingDoc::CImageProcessingDoc()
{
	// TODO: 在此添加一次性构造代码
	InitDocVariable();
}

CImageProcessingDoc::~CImageProcessingDoc()
{
	FreeDocVariable();

}

void CImageProcessingDoc::UpdateObject(HGLOBAL hDIB)
{
	if (m_pDibInit != NULL)               // 判断DIB是否为空
	{
		delete m_pDibInit;// 非空，则清除
		m_pDibInit = NULL;

	}
	//m_pDibInit = hDIB;                     // 替换成新的DIB对象	
}

BOOL CImageProcessingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CImageProcessingDoc 序列化

void CImageProcessingDoc::Serialize(CArchive& ar)
{
		m_pDibInit->Serialize(ar);

	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CImageProcessingDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
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

// 搜索处理程序的支持
void CImageProcessingDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
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

// CImageProcessingDoc 诊断

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


// CImageProcessingDoc 命令

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
	CString strSaveFileType = "位图文件 (*.bmp;*.dib)|*.bmp; *.dib|All Files (*.*)|*.*||";
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
//\函数名称：
//  OnFileReload()
//\输入参数:
//   无
//
//\返回值:
//   无
//
// \说明:
//  该函数响应ID_FILE_RELOAD点击的消息，并重载DIB
//---------------------------------------
void CImageProcessingDoc::OnFileReload() 
{
	// 判断当前图像是否已经被改动
	if (IsModified())
	{
		// 提示用户该操作将丢失所有当前的修改
		if (MessageBox(NULL, "重新打开图像将丢失所有改动！是否继续？", "系统提示", MB_ICONQUESTION | MB_YESNO) == IDNO)
		{
			// 用户取消操作，直接返回
			return;
		}
	}
	CFile file;

	CString strPathName;
	CFileException fe;

	strPathName = GetPathName();

	// 重新打开文件
	if( !file.Open(strPathName, CFile::modeRead | CFile::shareDenyWrite, &fe))
	{
		// 失败
		ReportSaveLoadException(strPathName, &fe, FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);

		// 返回
		return;
	}

	// 更改光标
	BeginWaitCursor();

	// 重新读取图象
	if(!m_pDibInit->Read(&file)){
		// 恢复光标形状
 		EndWaitCursor();
 				
 		 		
 		// 返回
 		return;
	}

	// 初始化胀标记为FALSE
 	SetModifiedFlag(FALSE);
 	
 	// 刷新
 	UpdateAllViews(NULL);
  	
 	// 恢复光标形状
 	EndWaitCursor();
	
 	// 返回
 	return;
}
