#pragma once


// CImgListCtrl

class CImgListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CImgListCtrl)

public:
	CImgListCtrl();
	virtual ~CImgListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


