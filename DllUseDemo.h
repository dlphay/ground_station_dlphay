
// DllUseDemo.h : DllUseDemo Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CDllUseDemoApp:
// �йش����ʵ�֣������ DllUseDemo.cpp
//

class CDllUseDemoApp : public CWinAppEx
{
public:
	CDllUseDemoApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDllUseDemoApp theApp;
