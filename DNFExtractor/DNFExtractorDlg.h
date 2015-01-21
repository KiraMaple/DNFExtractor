
// DNFExtractorDlg.h : 头文件
//

#pragma once

#include "Extractor.h"

// CDNFExtractorDlg 对话框
class CDNFExtractorDlg : public CDialogEx
{
// 构造
public:
	CDNFExtractorDlg(CWnd* pParent = NULL);	// 标准构造函数
	void UpdatePngList();
	void UpdatePngRender(int nPos);

// 对话框数据
	enum { IDD = IDD_DNFEXTRACTOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HBITMAP m_hPngBitmap;
	bool m_bIsLoadPng;
	CString m_szFilename;
	CString m_szSearchTxt;
	CListCtrl* m_wndImgList;
	CListCtrl* m_wndPngList;
	CButton* m_wndOpenBtn;
	CButton* m_wndCloseBtn;
	CButton* m_wndSearchClrBtn;
	CExtractor* m_poExtractor;
	CExtractor::NImgF_Index* m_pstPngIndex;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeFilenameEdit();
	afx_msg void OnBnClickedSearchClear();
	afx_msg void OnEnChangeSearchEdit();
	afx_msg void OnBnClickedOpenFile();
	afx_msg void OnBnClickedCloseFile();
	afx_msg void OnLvnItemchangedImgList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedPngList(NMHDR *pNMHDR, LRESULT *pResult);
};
