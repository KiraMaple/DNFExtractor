// PngRenderDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "DNFExtractor.h"
#include "PngRenderDialog.h"
#include "afxdialogex.h"


// CPngRenderDialog 对话框

IMPLEMENT_DYNAMIC(CPngRenderDialog, CDialogEx)

CPngRenderDialog::CPngRenderDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPngRenderDialog::IDD, pParent)
{
	m_bIsLoadPng = false;
	m_nIsRealPos = false;
	m_pstPngIndex = NULL;
}

CPngRenderDialog::~CPngRenderDialog()
{
}

void CPngRenderDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDNFExtractorDlg)
	DDX_Control(pDX, IDC_PNG_RENDER, m_wndPngRender);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPngRenderDialog, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CPngRenderDialog 消息处理程序

BOOL CPngRenderDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}

void CPngRenderDialog::OnPaint()
{
	CDialogEx::OnPaint();

	CPaintDC dc(this); // device context for painting

	if (m_bIsLoadPng && m_pstPngIndex)
	{
		UINT dwBufferSize = CExtractor::s_dwBufferSize;
		BYTE* pBuff = new BYTE[dwBufferSize];

		int ret = CExtractor::NpkToPng(pBuff, dwBufferSize, m_pstPngIndex->m_pData, m_pstPngIndex->m_dwBufferSize, m_pstPngIndex->m_stHeader.width, m_pstPngIndex->m_stHeader.height, m_pstPngIndex->m_stType.dwType);

		if (ret)
		{
			delete[] pBuff;
			Clear();
			MessageBox("图像转换失败！", "Error", MB_OK);
			return;
		}

		CImage oImage;
		if (FAILED(CExtractor::LoadImageFromBuffer(pBuff, dwBufferSize, oImage)))
		{
			delete[] pBuff;
			Clear();
			oImage.Destroy();
			return;
		}

		CDC * pDc = m_wndPngRender.GetWindowDC();    //获得显示控件的DC
		pDc->SetStretchBltMode(STRETCH_DELETESCANS);	 //保持图片不失真

		CRect imageRect;
		if (m_nIsRealPos)
		{
			imageRect.left = m_pstPngIndex->m_stHeader.key_x;
			imageRect.right = imageRect.left + oImage.GetWidth();
			imageRect.top = m_pstPngIndex->m_stHeader.key_y;
			imageRect.bottom = imageRect.top + oImage.GetHeight();
		}
		else
		{
			imageRect.left = 0;
			imageRect.right = oImage.GetWidth();
			imageRect.top = 0;
			imageRect.bottom = oImage.GetHeight();
		}
		oImage.Draw(pDc->m_hDC, imageRect);
		ReleaseDC(pDc);
		oImage.Destroy();
		delete[] pBuff;
	}
}

void CPngRenderDialog::ResizeRender()
{
	CRect rect;
	GetClientRect(rect);
	m_wndPngRender.MoveWindow(rect);
}

void CPngRenderDialog::Clear()
{
	m_bIsLoadPng = FALSE;
	m_pstPngIndex = NULL;
	Redraw();
}

void CPngRenderDialog::SetPngInfo(CExtractor::NImgF_Index* pstIndex)
{
	m_pstPngIndex = pstIndex;
	m_bIsLoadPng = TRUE;
	Redraw();
}

void CPngRenderDialog::Redraw()
{
	Invalidate();
}