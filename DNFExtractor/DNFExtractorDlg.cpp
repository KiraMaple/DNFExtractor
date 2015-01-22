
// DNFExtractorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DNFExtractor.h"
#include "DNFExtractorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDNFExtractorDlg 对话框



CDNFExtractorDlg::CDNFExtractorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDNFExtractorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDNFExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDNFExtractorDlg)
	DDX_Text(pDX, IDC_FILENAME_EDIT, m_szFilename);
	DDX_Control(pDX, IDC_IMG_LIST, m_wndImgList);
	DDX_Control(pDX, IDC_PNG_LIST, m_wndPngList);
	DDX_Control(pDX, IDC_OPEN_FILE, m_wndOpenBtn);
	DDX_Control(pDX, IDC_CLOSE_FILE, m_wndCloseBtn);
	DDX_Control(pDX, IDC_SEARCH_CLEAR, m_wndSearchClrBtn);
	DDX_Check(pDX, IDC_REALPOS_CHECK, m_wndPngRender.m_nIsRealPos);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDNFExtractorDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_FILENAME_EDIT, &CDNFExtractorDlg::OnEnChangeFilenameEdit)
	ON_EN_CHANGE(IDC_SEARCH_EDIT, &CDNFExtractorDlg::OnEnChangeSearchEdit)
	ON_BN_CLICKED(IDC_SEARCH_CLEAR, &CDNFExtractorDlg::OnBnClickedSearchClear)
	ON_BN_CLICKED(IDC_OPEN_FILE, &CDNFExtractorDlg::OnBnClickedOpenFile)
	ON_BN_CLICKED(IDC_CLOSE_FILE, &CDNFExtractorDlg::OnBnClickedCloseFile)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IMG_LIST, &CDNFExtractorDlg::OnLvnItemchangedImgList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PNG_LIST, &CDNFExtractorDlg::OnLvnItemchangedPngList)
	ON_BN_CLICKED(IDC_REALPOS_CHECK, &CDNFExtractorDlg::OnBnClickedRealposCheck)
END_MESSAGE_MAP()


// CDNFExtractorDlg 消息处理程序

BOOL CDNFExtractorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CRect rect;
	CStatic* pRender = (CStatic*)GetDlgItem(IDC_PNG_RECT);
	pRender->GetWindowRect(&rect);
	ScreenToClient(rect);

	m_wndPngRender.Create(IDD_PNG_DIALOG, this);
	m_wndPngRender.MoveWindow(rect);
	m_wndPngRender.ResizeRender();
	m_wndPngRender.ShowWindow(SW_SHOWNORMAL);
	pRender->ShowWindow(SW_HIDE);

	m_poExtractor = NULL;
	m_szFilename = "";
	m_szSearchTxt = "";

	m_wndCloseBtn.EnableWindow(FALSE);

	m_wndImgList.InsertColumn(0, _T("文件名"), LVCFMT_CENTER, 185);

	m_wndPngList.InsertColumn(0, _T("ID"), LVCFMT_CENTER, 50);
	m_wndPngList.InsertColumn(1, _T("模式"), LVCFMT_CENTER, 150);
	m_wndPngList.InsertColumn(2, _T("宽高"), LVCFMT_CENTER, 80);
	m_wndPngList.InsertColumn(3, _T("坐标"), LVCFMT_CENTER, 80);
	m_wndPngList.InsertColumn(4, _T("帧域宽高"), LVCFMT_CENTER, 100);

	DWORD dwStyle = 0;
	dwStyle = m_wndImgList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_wndImgList.SetExtendedStyle(dwStyle);

	dwStyle = m_wndPngList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_wndPngList.SetExtendedStyle(dwStyle);

	UpdateData(FALSE);

	SetActiveWindow();
	ShowWindow(SW_SHOWNORMAL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDNFExtractorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDNFExtractorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDNFExtractorDlg::OnEnChangeFilenameEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CDNFExtractorDlg::OnBnClickedSearchClear()
{
	// TODO:  在此添加控件通知处理程序代码
}


void CDNFExtractorDlg::OnEnChangeSearchEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CDNFExtractorDlg::OnBnClickedOpenFile()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_poExtractor != NULL)
	{
		return;
	}

	TCHAR szFilters[] = _T("Neople Package Files (*.npk)|*.npk||");

	CFileDialog dlg(TRUE, _T("npk"), _T("*.npk"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters);

	if (dlg.DoModal() != IDOK)
		return;

	m_poExtractor = new CExtractor(dlg.GetPathName());
	if ( FALSE == m_poExtractor->IsLoad() )
	{
		delete m_poExtractor;
		m_poExtractor = NULL;
		return;
	}

	m_szFilename = dlg.GetPathName();
	m_szFilename = m_szFilename.Mid(m_szFilename.ReverseFind('\\') + 1);
	UpdateData(FALSE);
	m_wndOpenBtn.EnableWindow(FALSE);
	m_wndCloseBtn.EnableWindow(TRUE);

	int nImgCount = m_poExtractor->GetImgCount();
	for (int i = 0; i < nImgCount; i++)
	{
		CExtractor::NPK_Index* poIndex = m_poExtractor->GetImgByPos(i);
		if (poIndex != NULL)
		{
			CString strFileName = poIndex->name;
			strFileName = strFileName.Mid(strFileName.ReverseFind('/') + 1);
			m_wndImgList.InsertItem(i, strFileName);
		}
	}

	if (nImgCount > 0)
	{
		m_wndImgList.EnsureVisible(0, FALSE);
		m_wndImgList.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_wndImgList.SetFocus();
	}
}


void CDNFExtractorDlg::OnBnClickedCloseFile()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_poExtractor != NULL)
	{
		delete m_poExtractor;
		m_poExtractor = NULL;
	}

	m_szFilename = "";
	UpdateData(FALSE);
	m_wndOpenBtn.EnableWindow(TRUE);
	m_wndCloseBtn.EnableWindow(FALSE);

	m_wndPngRender.Clear();

	m_wndImgList.DeleteAllItems();
	m_wndPngList.DeleteAllItems();
}

void CDNFExtractorDlg::OnLvnItemchangedImgList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (pNMLV->uNewState == (LVIS_FOCUSED | LVIS_SELECTED))
	{
		if (m_poExtractor == NULL)
		{
			return;
		}

		int nPos = pNMLV->iItem;
		if (nPos == -1)
		{
			return;
		}

		m_poExtractor->SetActiveImg(nPos);
		UpdatePngList();
	}

}

void CDNFExtractorDlg::OnLvnItemchangedPngList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (pNMLV->uNewState == (LVIS_FOCUSED | LVIS_SELECTED))
	{
		if (m_poExtractor == NULL)
		{
			return;
		}

		int nPos = pNMLV->iItem;
		if (nPos == -1)
		{
			return;
		}

		//m_poExtractor->SetActivePng(nPos);
		UpdatePngRender(nPos);
	}

}

void CDNFExtractorDlg::UpdatePngList()
{
	m_wndPngList.DeleteAllItems();

	int nCount = m_poExtractor->GetPngCount();

	for (int i = 0; i < nCount; i++)
	{
		CExtractor::NImgF_Index* pstHeader = m_poExtractor->GetPngByPos(i);
		if (pstHeader != NULL)
		{
			CString szText = _T("");
			szText.Format(_T("%d"), i);
			m_wndPngList.InsertItem(i, szText);

			if (pstHeader->m_dwIsLink)
			{
				szText = _T("LINK型");
			}
			else
			{
				if (pstHeader->m_stType.dwType == ARGB_8888)
				{
					szText = _T("32位色[A8R8G8B8]");
				}
				else if (pstHeader->m_stType.dwType == ARGB_1555)
				{
					szText = _T("16位色[A1R5G5B5]");
				}
				else if (pstHeader->m_stType.dwType == ARGB_4444)
				{
					szText = _T("16位色[A4R4G4B4]");
				}
				else
				{
					szText = _T("Unkown");
				}
			}
			m_wndPngList.SetItemText(i, 1, szText);

			szText.Format(_T("%4d x %4d"), pstHeader->m_stHeader.width, pstHeader->m_stHeader.height);
			m_wndPngList.SetItemText(i, 2, szText);

			szText.Format(_T("(%4d,%4d )"), pstHeader->m_stHeader.key_x, pstHeader->m_stHeader.key_y);
			m_wndPngList.SetItemText(i, 3, szText);

			szText.Format(_T("%4d x %4d"), pstHeader->m_stHeader.max_width, pstHeader->m_stHeader.max_height);
			m_wndPngList.SetItemText(i, 4, szText);

			m_wndPngList.SetItemData(i, (DWORD_PTR)pstHeader);
		}
	}

	if (nCount > 0)
	{
		m_wndPngList.EnsureVisible(0, FALSE);
		m_wndPngList.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		m_wndPngList.SetFocus();
	}
}

void CDNFExtractorDlg::UpdatePngRender(int nPos)
{
	if (m_wndPngRender.m_bIsLoadPng)
	{
		m_wndPngRender.Clear();
	}

	if (m_poExtractor == NULL)
	{
		return;
	}

	CExtractor::NImgF_Index* pstIndex = m_poExtractor->GetPngByPos(nPos);
	if (pstIndex == NULL || pstIndex->m_pData == NULL)
	{
		return;
	}

	m_wndPngRender.SetPngInfo(pstIndex);
}

void CDNFExtractorDlg::OnBnClickedRealposCheck()
{
	bool bPre = m_wndPngRender.m_nIsRealPos;
	UpdateData(TRUE);
	if (bPre != m_wndPngRender.m_nIsRealPos)
	{
		m_wndPngRender.Redraw();
	}
}
