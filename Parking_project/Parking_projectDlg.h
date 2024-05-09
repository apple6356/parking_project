
// Parking_projectDlg.h: 헤더 파일
//

#pragma once
#include "CDlgDayAC.h"
#include "sqlite3.h"
#include <io.h>

#define CREATE_TABLE_CAR "CREATE TABLE car (car_num VARCHAR(4),in_day INTEGER,in_hour INTEGER,in_minute INTEGER,out_day INTEGER default 0,out_hour INTEGER default 0,out_minute INTEGER default 0,parking_num INTEGER); ";


struct Car {		// 구조체 선언
	CString car_num;
	int in_month;
	int in_day;
	int in_hour;
	int in_minute;
	int out_month;
	int out_day;
	int out_hour;
	int out_minute;
	int parking_num;
};


// CParkingprojectDlg 대화 상자
class CParkingprojectDlg : public CDialogEx
{
// 생성입니다.
public:
	CParkingprojectDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARKING_PROJECT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	//합친거1
	afx_msg void OnClickedButtonIn();
	afx_msg void OnClickedButtonOut();
	afx_msg void OnClickedButtonMoney();
	Car car[8];				// 구조체 배열을 선언(주차 공간 수만큼 선언), 모두 0으로 선언
	void GetInTime();
	void GetOutTime(int car_cnt);
	int car_cnt;			// 구조체 구분용
	CString m_strInNum;		// 들어오는 차 번호
	CString m_strOutNum;	// 나가는 차 번호
	CString m_strMoney;		// 요금
	CListCtrl m_ListCtrl;	// 리스트 컨트롤 변수
	int list_column;
	int list_row;
	int list_height;


	//합친거2
	int daypropit;			// 일일 결산용 변수
	int cost;				// 주차요금
	void CostCal(int car_cnt);
	CDlgDayAC m_dlgAC;
	bool m_bViewAC;			// 결산창이 열려있는지 확인하는 변수
	void DayAccounts();
	CTime pre_doweek;

	/*
	typedef struct {
		int mon_Accounts;
		int tue_Accounts;
		int wed_Accounts;
		int thur_Accounts;
		int fri_Accounts;
		int sat_Accounts;
		int sun_Accounts;
	} DayAC;
	DayAC dayAc;
	*/

	CStatic m_PicCtrl;
	int picture_width;
	int picture_height;
	int park_row;
	int park_column;
	int park_width;
	int park_height;
	void ToGreen(int parking_num);
	void ToWhite(int parking_num);

	sqlite3* db = NULL;			// sqlite 변수 추가
	sqlite3_stmt* stmt = NULL;	// 쿼리문 동작 확인
	char* sql;					// 쿼리문 변수 추가
	int isFirst;
	CString sql_str;

	// 영원3
	int length;
	virtual BOOL DestroyWindow();
	//Car* car;					// 이거 왜 안되지?

	// 합친거3
	CTime timer;
	int week;
	CFont m_font;	// 폰트 크기 설정용
	char weekMoney_db[20];
	char weekRecords_db[20];
	int dow;
	CString weekMoney_str;
	CString str;
	TCHAR desktop_path[100];
	TCHAR program_path[100];
	CString folder_path;
	int result;

	// 디버깅1
	int lastday;
	int IsExist;
	CStringA strA;
	int accounts[7];
	char* access_ch;
};