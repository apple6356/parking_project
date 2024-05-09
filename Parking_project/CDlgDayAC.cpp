// CDlgDayAC.cpp: 구현 파일
//

#include "pch.h"
#include "Parking_project.h"
#include "Parking_projectDlg.h"
#include "CDlgDayAC.h"
#include "afxdialogex.h"


// CDlgDayAC 대화 상자

IMPLEMENT_DYNAMIC(CDlgDayAC, CDialogEx)

CDlgDayAC::CDlgDayAC(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_AC, pParent)
	, m_strMon(_T("0원"))
	, m_strTue(_T("0원"))
	, m_strWed(_T("0원"))
	, m_strThur(_T("0원"))
	, m_strFri(_T("0원"))
	, m_strSat(_T("0원"))
	, m_strSun(_T("0원"))
{
}

CDlgDayAC::~CDlgDayAC()
{
}

void CDlgDayAC::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MONDAY, m_strMon);
	DDX_Text(pDX, IDC_EDIT_TUESDAY, m_strTue);
	DDX_Text(pDX, IDC_EDIT_WEDNESDAY, m_strWed);
	DDX_Text(pDX, IDC_EDIT_THURSDAY, m_strThur);
	DDX_Text(pDX, IDC_EDIT_FRIDAY, m_strFri);
	DDX_Text(pDX, IDC_EDIT_SATURDAY, m_strSat);
	DDX_Text(pDX, IDC_EDIT_SUNDAY, m_strSun);
}


BEGIN_MESSAGE_MAP(CDlgDayAC, CDialogEx)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDlgDayAC 메시지 처리기


void CDlgDayAC::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	

	CParkingprojectDlg* pWind = (CParkingprojectDlg*)GetParent();
	pWind->m_bViewAC = FALSE;
	ShowWindow(SW_HIDE);
	DestroyWindow();
	
	CDialogEx::OnClose();
}