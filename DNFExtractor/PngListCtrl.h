#pragma once


// CPngListCtrl

class CPngListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CPngListCtrl)

public:
	CPngListCtrl();
	virtual ~CPngListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnExportClick();
	afx_msg void OnExportTeamClick();
	void CPngListCtrl::ExportFile(bool bIsTeam = false);
};


