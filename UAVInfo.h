#pragma once


// CUAVInfo �Ի���

class CUAVInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CUAVInfo)

public:
	CUAVInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUAVInfo();

// �Ի�������
	enum { IDD = IDD_DIALOG_UAV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_Lon;
	double m_Lat;
	double m_Height;
	double m_Pitch;
	double m_roll;
	double m_Yaw;
	afx_msg void OnEnUpdateEdit1();
	afx_msg void OnEnChangeEdit1();
	double m_speed;
	int m_type;
	double m_pitch;
	double m_head;
	double m_pixelSize;
	double m_focus;
	float m_time;
};
