
// DllUseDemoView.h : CDllUseDemoView ��Ľӿ�
//

#pragma once
#include "MainFrm.h"

class CDllUseDemoView : public CScrollView
{
protected: // �������л�����
	CDllUseDemoView();
	DECLARE_DYNCREATE(CDllUseDemoView)

// ����
public:
	CDllUseDemoDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CDllUseDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	/**************************************************************************************/
	/***************************�˴�Ϊ��ӵĴ���******************************************/
	/**************************************************************************************/
	 HANDLE m_pdispThread;//��ʾ�߳�
	 HANDLE m_pCapThread;//����DLL�߳�
	 /**************************************************************************************/
	 /*******************************��Ӵ������*******************************************/
	 /**************************************************************************************/
	 afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	 afx_msg void Ondehaze();
	 afx_msg void Onhuifu();
	 afx_msg void Detector_CAMERA();
	 afx_msg void Detector_ORB_CAMERA();
	 afx_msg void Detector_ORB_LOCAL();
	 afx_msg void Detector_ANA_CONFIDENCE();
	 afx_msg void Detector_PARA_OVERFIT();
	 afx_msg void Detector_CONNECTED_DOMAIN();  // ��ͨ�򷽷�
	 afx_msg void Detector_HOGSVM();  // ���˼�ⷽ��
	 afx_msg void Detector_HOUGH();  // ���򷽷�
	 //����
	 afx_msg void Tracker_tld();
	 afx_msg void Tracker_meanshift();
	 afx_msg void Tracker_particle();
	 afx_msg void Tracker_mtilti();
	 //ʶ��
	 afx_msg void Detector_yolo();
	 afx_msg void Detector_hog_svm();
};

#ifndef _DEBUG  // DllUseDemoView.cpp �еĵ��԰汾
inline CDllUseDemoDoc* CDllUseDemoView::GetDocument() const
   { return reinterpret_cast<CDllUseDemoDoc*>(m_pDocument); }
#endif

