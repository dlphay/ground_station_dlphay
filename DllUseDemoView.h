
// DllUseDemoView.h : CDllUseDemoView 类的接口
//

#pragma once
#include "MainFrm.h"

class CDllUseDemoView : public CScrollView
{
protected: // 仅从序列化创建
	CDllUseDemoView();
	DECLARE_DYNCREATE(CDllUseDemoView)

// 特性
public:
	CDllUseDemoDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CDllUseDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	/**************************************************************************************/
	/***************************此处为添加的代码******************************************/
	/**************************************************************************************/
	 HANDLE m_pdispThread;//显示线程
	 HANDLE m_pCapThread;//调用DLL线程
	 /**************************************************************************************/
	 /*******************************添加代码结束*******************************************/
	 /**************************************************************************************/
	 afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	 afx_msg void Ondehaze();
	 afx_msg void Onhuifu();
	 afx_msg void Detector_CAMERA();
	 afx_msg void Detector_ORB_CAMERA();
	 afx_msg void Detector_ORB_LOCAL();
	 afx_msg void Detector_ANA_CONFIDENCE();
	 afx_msg void Detector_PARA_OVERFIT();
	 afx_msg void Detector_CONNECTED_DOMAIN();  // 连通域方法
	 afx_msg void Detector_HOGSVM();  // 行人检测方法
	 afx_msg void Detector_HOUGH();  // 霍夫方法
	 //跟踪
	 afx_msg void Tracker_tld();
	 afx_msg void Tracker_meanshift();
	 afx_msg void Tracker_particle();
	 afx_msg void Tracker_mtilti();
	 //识别
	 afx_msg void Detector_yolo();
	 afx_msg void Detector_hog_svm();
};

#ifndef _DEBUG  // DllUseDemoView.cpp 中的调试版本
inline CDllUseDemoDoc* CDllUseDemoView::GetDocument() const
   { return reinterpret_cast<CDllUseDemoDoc*>(m_pDocument); }
#endif

