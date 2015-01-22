#pragma once

#include "Extractor.h"

// CPngRenderDialog 对话框

class CPngRenderDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CPngRenderDialog)

public:
	int m_nIsRealPos;
	bool m_bIsLoadPng;
	CExtractor::NImgF_Index* m_pstPngIndex;

	CPngRenderDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPngRenderDialog();

// 对话框数据
	enum { IDD = IDD_PNG_DIALOG };

protected:
	CStatic m_wndPngRender;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	void ResizeRender();

	void Clear();
	void SetPngInfo(CExtractor::NImgF_Index* pstIndex);
	void Redraw();
};
