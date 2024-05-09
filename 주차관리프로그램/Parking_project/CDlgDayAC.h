#pragma once


// CDlgDayAC 대화 상자

class CDlgDayAC : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDayAC)

public:
	CDlgDayAC(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDayAC();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMon;
	CString m_strTue;
	CString m_strWed;
	CString m_strThur;
	CString m_strFri;
	CString m_strSat;
	CString m_strSun;
	afx_msg void OnClose();
};
