// ImgListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "DNFExtractor.h"
#include "ImgListCtrl.h"


// CImgListCtrl

IMPLEMENT_DYNAMIC(CImgListCtrl, CListCtrl)

CImgListCtrl::CImgListCtrl()
{

}

CImgListCtrl::~CImgListCtrl()
{
}


BEGIN_MESSAGE_MAP(CImgListCtrl, CListCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CImgListCtrl 消息处理程序




int CImgListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}
