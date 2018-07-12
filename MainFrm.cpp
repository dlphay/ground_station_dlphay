
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "DllUseDemo.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// �������ڻ��������û�����Ԫ�ص��Ӿ�������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("δ�ܴ����˵���\n");
		return -1;      // δ�ܴ���
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);


	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: �������ϣ���������Ͳ˵�����ͣ������ɾ��������
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);


	// ���� Visual Studio 2005 ��ʽͣ��������Ϊ
	CDockingManager::SetDockingMode(DT_SMART);
	// ���� Visual Studio 2005 ��ʽͣ�������Զ�������Ϊ
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// ���ù�������ͣ�����ڲ˵��滻
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ���ÿ���(��ס Alt �϶�)�������Զ���
	CMFCToolBar::EnableQuickCustomization();

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* ɨ��˵�*/);
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnFullScreen()    //����ȫ��
{
	GetWindowPlacement(&m_OldWndPlacement);
	CRect WindowRect;
	GetWindowRect(&WindowRect);
	CRect ClientRect;
	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &ClientRect);
	ClientToScreen(&ClientRect);
	// ��ȡ��Ļ�ķֱ���
	int nFullWidth = GetSystemMetrics(SM_CXSCREEN);
	int nFullHeighth = GetSystemMetrics(SM_CYSCREEN);
	// ������������Ŀͻ���ȫ����ʾ����(0,0)��(nFullWidth, nFullHeight)����, ��(0,0)��(nFullWidth, nFullHeight)������������ԭ���ںͳ�������֮��Ŀ�
	//����λ�ü�Ĳ�ֵ, �͵õ�ȫ����ʾ�Ĵ���λ�� 
	m_FullScreenRect.left = WindowRect.left - ClientRect.left + nFullWidth;
	m_FullScreenRect.top = WindowRect.top - ClientRect.top;
	m_FullScreenRect.right = WindowRect.right - ClientRect.right + nFullWidth + 1920;
	m_FullScreenRect.bottom = WindowRect.bottom - ClientRect.bottom + 1080;
	m_bFullScreen = TRUE;
	// ����ȫ����ʾ״̬
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	wndpl.flags = 0;
	wndpl.showCmd = SW_SHOWNORMAL;
	wndpl.rcNormalPosition = m_FullScreenRect;
	SetWindowPlacement(&wndpl);
	//����״̬��
	//����ǿɼ�������Ϊ����
	m_wndStatusBar.ModifyStyle(WS_VISIBLE, 0);
	m_wndMenuBar.ShowWindow(SW_HIDE);

	//ʹ״̬��״̬��Ч
	SendMessage(WM_SIZE);
}
void CMainFrame::EndFullScreen()
{
	if (m_bFullScreen)
	{
		//�˳�ȫ����ʾ, �ָ�ԭ������ʾ
		SetWindowPlacement(&m_OldWndPlacement);
		//����ǲ��ɼ��ģ�����Ϊ�ɼ�
		m_wndStatusBar.ModifyStyle(0, WS_VISIBLE);
		m_wndMenuBar.ShowWindow(SW_SHOWNORMAL);
		//��ʾ״̬��
		//ʹ״̬��״̬��Ч
		SendMessage(WM_SIZE);//�ж��Ƿ�״̬���ɼ�
		m_bFullScreen = FALSE;


	}
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_bFullScreen)
	{
		lpMMI->ptMaxSize.x = m_FullScreenRect.Width();
		lpMMI->ptMaxSize.y = m_FullScreenRect.Height();
		lpMMI->ptMaxPosition.x = m_FullScreenRect.left;
		lpMMI->ptMaxPosition.y = m_FullScreenRect.top;
		lpMMI->ptMaxTrackSize.x = m_FullScreenRect.Width();
		lpMMI->ptMaxTrackSize.y = m_FullScreenRect.Height();
	}

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}