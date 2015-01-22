// PngListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "Extractor.h"
#include "DNFExtractor.h"
#include "DNFExtractorDlg.h"
#include "PngListCtrl.h"

// CPngListCtrl

#define SAVE_FILE_PATH "D:\\UserFile\\Desktop\\DNF_Ext_Output"

IMPLEMENT_DYNAMIC(CPngListCtrl, CListCtrl)

CPngListCtrl::CPngListCtrl()
{

}

CPngListCtrl::~CPngListCtrl()
{
}


BEGIN_MESSAGE_MAP(CPngListCtrl, CListCtrl)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_COMMAND(IDC_EXPORT, &CPngListCtrl::OnExportClick)
	ON_COMMAND(IDC_EXPORT_TEAM, &CPngListCtrl::OnExportTeamClick)
END_MESSAGE_MAP()



// CPngListCtrl 消息处理程序



int CPngListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void CPngListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int nSelectCount = GetSelectedCount();

	if (nSelectCount <= 0)
	{
		return;
	}

	CMenu oMenu, *pPopup;
	oMenu.LoadMenu(IDR_PNG_PROPERTY);
	pPopup = oMenu.GetSubMenu(0);
	if (pPopup == NULL)
	{
		return;
	}

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

void CPngListCtrl::OnExportClick()
{
	ExportFile();
}


void CPngListCtrl::OnExportTeamClick()
{
	ExportFile(true);
}

void CPngListCtrl::ExportFile(bool bIsTeam)
{
	int nSelectCount = GetSelectedCount();

	if (nSelectCount <= 0)
	{
		return;
	}

	CDNFExtractorDlg* pstMainDlg = (CDNFExtractorDlg*)GetParent();
	if (pstMainDlg == NULL)
	{
		return;
	}

	CExtractor* pExtractor = pstMainDlg->GetExtractor();
	if (pExtractor == NULL)
	{
		return;
	}

	CString szPath;
	szPath.Format("%s\\%s\\%s", SAVE_FILE_PATH, pExtractor->GetFileName(), pExtractor->GetActiveImgName());
	PathFileExists(szPath);
	if (!PathIsDirectory(szPath))
	{
		if (!custom::CreateMuliteDirectory(szPath))
		{
			MessageBox("导出失败，对应文件夹创建失败。", "Error", MB_OK);
			return;
		}
	}

	int nTeamMinX = 0x7fffffff, nTeamMinY = 0x7fffffff, nTeamMaxX = 0, nTeamMaxY = 0;
	if (bIsTeam)
	{
		POSITION pos = GetFirstSelectedItemPosition();
		while (pos)
		{
			int nIndex = GetNextSelectedItem(pos);
			CExtractor::NImgF_Index* pstIndex = pExtractor->GetPngByPos(nIndex);
			if (pstIndex == NULL)
			{
				continue;
			}

			CExtractor::NPngF_Info stHeader = pstIndex->m_stHeader;
			if (nTeamMinX > stHeader.key_x)
			{
				nTeamMinX = stHeader.key_x;
			}

			if (nTeamMinY > stHeader.key_y)
			{
				nTeamMinY = stHeader.key_y;
			}

			if (nTeamMaxX < (stHeader.key_x + stHeader.width))
			{
				nTeamMaxX = stHeader.key_x + stHeader.width;
			}

			if (nTeamMaxY < (stHeader.key_y + stHeader.height))
			{
				nTeamMaxY = stHeader.key_y + stHeader.height;
			}
		}
	}

	int nTeamWidth = nTeamMaxX - nTeamMinX, nTeamHeight = nTeamMaxY - nTeamMinY;

	BYTE* pBuff = new BYTE[CExtractor::s_dwBufferSize];
	int nSuccess = 0;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = GetNextSelectedItem(pos);
		CExtractor::NImgF_Index* pstIndex = pExtractor->GetPngByPos(nIndex);
		if (pstIndex == NULL)
		{
			continue;
		}
		memset(pBuff, 0, CExtractor::s_dwBufferSize);

		UINT dwBufferSize = CExtractor::s_dwBufferSize;
		int ret = 0;

		if (bIsTeam)
		{
			int x = pstIndex->m_stHeader.key_x - nTeamMinX;
			int y = pstIndex->m_stHeader.key_y - nTeamMinY;
			ret = CExtractor::NpkToPng(pBuff, dwBufferSize, pstIndex->m_pData, pstIndex->m_dwBufferSize, pstIndex->m_stHeader.width, pstIndex->m_stHeader.height, pstIndex->m_stType.dwType, x, y, nTeamWidth, nTeamHeight);
		}
		else
		{
			ret = CExtractor::NpkToPng(pBuff, dwBufferSize, pstIndex->m_pData, pstIndex->m_dwBufferSize, pstIndex->m_stHeader.width, pstIndex->m_stHeader.height, pstIndex->m_stType.dwType);
		}

		if (ret)
		{
			TRACE("Failed to Transform Npk to Png. Index:%03d", nIndex);
			continue;
		}

		CFile oFile;
		CString szFile;
		szFile.Format("%s\\%04d.png", szPath, nIndex);
		if (!(oFile.Open(szFile, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive)))
		{
			TRACE("Failed to Open File，File:%s", szFile);
			continue;
		}

		oFile.Write(pBuff, dwBufferSize);
		oFile.Close();
		nSuccess++;
	}

	delete[] pBuff;

	CString szMsg;
	szMsg.Format("导出完成。总共：%d，成功：%d。", nSelectCount, nSuccess);

	MessageBox(szMsg, "提示", MB_OK);
}