
// Parking_projectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Parking_project.h"
#include "Parking_projectDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <iostream>

#pragma comment(lib, "sqlite3.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CParkingprojectDlg 대화 상자



CParkingprojectDlg::CParkingprojectDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PARKING_PROJECT_DIALOG, pParent)
	, m_strInNum(_T(""))
	, m_strMoney(_T(""))
	, m_strOutNum(_T(""))
	, m_bViewAC(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_strMoney.Format(_T("원"));
}

void CParkingprojectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INNUM, m_strInNum);
	DDX_Text(pDX, IDC_EDIT_MONEY, m_strMoney);
	DDX_Text(pDX, IDC_EDIT_OUTNUM, m_strOutNum);
	DDX_Control(pDX, IDC_PARKING, m_PicCtrl);
}

BEGIN_MESSAGE_MAP(CParkingprojectDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_IN, &CParkingprojectDlg::OnClickedButtonIn)
	ON_BN_CLICKED(IDC_BUTTON_OUT, &CParkingprojectDlg::OnClickedButtonOut)
	ON_BN_CLICKED(IDC_BUTTON_MONEY, &CParkingprojectDlg::OnClickedButtonMoney)
END_MESSAGE_MAP()

// CParkingprojectDlg 메시지 처리기

BOOL CParkingprojectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	timer = CTime::GetCurrentTime();	
	pre_doweek = timer.GetDayOfWeek();		// 처음 요일을 받는다. 날이 변하면 하루 수익(daypropit)을 초기화하기 위해
	daypropit = 0;							// 일 수익을 처음에는 0으로 초기화
	park_row = 2;							// 주차장 가로
	park_column = 4;						// 주차장 세로
	car_cnt = 0;							// 현재 주차되어 있는 총 자동차의 수
	week = (timer.GetDay() / 7) + 1;
	dow = timer.GetDayOfWeek();
	SHGetSpecialFolderPath(NULL, desktop_path, CSIDL_DESKTOP, FALSE);
	//AfxMessageBox(desktop_path);
	GetCurrentDirectory(100, program_path);
	//AfxMessageBox(program_path);


	str.Format(_T("%s/ParkManagement.db"), program_path);
	strA = CStringA(str);
	access_ch = strA.GetBuffer();
	isFirst = _access(access_ch, 0); // 존재하지 않는 경우 -1
	//str.Format(_T("isFirst: %d"), isFirst);
	//AfxMessageBox(str);


	//car = (Car*)malloc(sizeof(Car) * (park_row * park_column));

	//마지막 일수 구하기
	int year = timer.GetYear();
	int month = timer.GetMonth();

	if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
		lastday = 31;
	}
	else if (month == 4 || month == 6 || month == 9 || month == 11) {
		lastday = 30;
	}
	else if (month == 2) {
		if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) {
			lastday = 29;
		}
		else {
			lastday = 28;
		}
	}
	//str.Format(_T("이달의 마지막 날짜 %d"), lastday);
	//MessageBox(str);

	// 폰트 크기
	m_font.CreatePointFont(120, _T("굴림"));
	GetDlgItem(IDC_EDIT_MONEY)->SetFont(&m_font);

	// picture control 너비, 높이 구하기
	CStatic* staticSize = (CStatic*)GetDlgItem(IDC_PARKING);
	CRect rect;
	staticSize->GetClientRect(rect);

	picture_width = rect.Width() - 4;
	picture_height = rect.Height() - 4;

	park_width = picture_width / park_row;
	park_height = picture_height / park_column;

	if (isFirst == -1) { // 처음은 0이니까 !isFirst
		// 아무것도 없는 상태의 처음 실행이라면
		//폴더 만들기 //요기
		// timer.GetDay() != 1
		if (timer.GetDay() != 1) {
			folder_path.Format(_T("%s/%d.%d월"), desktop_path, timer.GetYear(), timer.GetMonth());
			CreateDirectory(folder_path, NULL);
		}

		// 데이터베이스 열기 (안되면 오류)
		if (sqlite3_open("ParkManagement.db", &db) != SQLITE_OK) {
			AfxMessageBox(_T("no access"));
		}

		sql_str.Format(_T("CREATE TABLE car(\
		car_num VARCHAR(4), \
		in_month INTEGER, \
		in_day INTEGER, \
		in_hour INTEGER, \
		in_minute INTEGER, \
		out_month INTEGER default 0, \
		out_day INTEGER default 0, \
		out_hour INTEGER default 0, \
		out_minute INTEGER default 0, \
		parking_num INTEGER);"));
		length = sql_str.GetLength() + 1;
		sql = new char[length];
		WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

		if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {	// sqlite3_prepare_v2() : sql(char*) -> stmt(sqlite3_stmt*) 형변환
			sqlite3_step(stmt);											// sqlite3_step() : 쿼리문 실행
			result = sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			//str.Format(_T("update result:%d"), result);
			//AfxMessageBox(str);
		}
		else {
			puts("테이블 생성에 실패했습니다.");
		}
		sqlite3_close(db);


		//차량출입기록 데이터베이스 열기
		sprintf_s(weekRecords_db, sizeof(weekRecords_db), "week%d_records.db", week);
		if (sqlite3_open(weekRecords_db, &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
			AfxMessageBox(_T("no access"));
		}

		//dow != 2 //요기
		if (dow != 2) {
			// 차량출입기록 매주 월요일에 데이터베이스 테이블 생성
			sql_str.Format(_T("CREATE TABLE records(\
			id INTEGER PRIMARY KEY AUTOINCREMENT, \
			car_num VARCHAR(4), \
			in_day INTEGER, \
			in_hour INTEGER, \
			in_minute INTEGER, \
			out_day INTEGER default 0, \
			out_hour INTEGER default 0, \
			out_minute INTEGER default 0, \
			parking_num INTEGER);"));
			length = sql_str.GetLength() + 1;
			sql = new char[length];
			WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

			if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {	// sqlite3_prepare_v2() : sql(char*) -> stmt(sqlite3_stmt*) 형변환
				result = sqlite3_step(stmt);								// sqlite3_step() : 쿼리문 실행
				sqlite3_finalize(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);											
			}
			else {
				puts("테이블 생성에 실패했습니다.");
			}	
		}
		sqlite3_close(db);

		//주별결산 데이터베이스 열기
		sprintf_s(weekMoney_db, sizeof(weekMoney_db), "week%d_money.db", week);
		if (sqlite3_open(weekMoney_db, &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
			AfxMessageBox(_T("no access"));
		}

		//dow != 2 //요기
		if (dow != 2) {
			// 주별결산 매주 월요일에 데이터베이스 테이블 생성
			sql_str.Format(_T("CREATE TABLE money (\
			mon_Accounts INTEGER default 0,\
			tue_Accounts INTEGER default 0,\
			wed_Accounts INTEGER default 0,\
			thur_Accounts INTEGER default 0,\
			fri_Accounts INTEGER default 0,\
			sat_Accounts INTEGER default 0,\
			sun_Accounts INTEGER default 0);"));
			length = sql_str.GetLength() + 1;
			sql = new char[length];
			WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

			if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {	// sqlite3_prepare_v2() : sql(char*) -> stmt(sqlite3_stmt*) 형변환
				result = sqlite3_step(stmt);								// sqlite3_step() : 쿼리문 실행
				sqlite3_finalize(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);											
			}
			else {
				puts("테이블 생성에 실패했습니다.");
			}

			//데이터 생성
			sql_str.Format(_T("insert into money values (0,0,0,0,0,0,0);"));
			length = sql_str.GetLength() + 1;
			sql = new char[length];
			WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

			if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {	// sqlite3_prepare_v2() : sql(char*) -> stmt(sqlite3_stmt*) 형변환
				result = sqlite3_step(stmt);								// sqlite3_step() : 쿼리문 실행
				sqlite3_finalize(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);											
			}
			sqlite3_close(db);
		}
	}
	else {
		//폴더 만들기 //요기
		// timer.GetDay() == 1
		if (timer.GetDay() == 1) {
			folder_path.Format(_T("%s/%d.%d월"), desktop_path, timer.GetYear(), timer.GetMonth());
			CreateDirectory(folder_path, NULL);
		}
		else {
			folder_path.Format(_T("%s/%d.%d월"), desktop_path, timer.GetYear(), timer.GetMonth());
		}

		if (sqlite3_open("ParkManagement.db", &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
			AfxMessageBox(_T("no access"));
		}

		//전날에 주차된차의 정보 받아오고 car_cnt초기화
		sql_str.Format(_T("select * from car;"));
		length = sql_str.GetLength() + 1;
		sql = new char[length];
		WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
			int result;
			result = sqlite3_step(stmt);
			//str.Format(_T("update result:%d"), result);
			//AfxMessageBox(str);
			while (result == SQLITE_ROW) {
				car[car_cnt].car_num = (char*)sqlite3_column_text(stmt, 0);
				car[car_cnt].in_month = sqlite3_column_int(stmt, 1);
				car[car_cnt].in_day = sqlite3_column_int(stmt, 2);
				car[car_cnt].in_hour = sqlite3_column_int(stmt, 3);
				car[car_cnt].in_minute = sqlite3_column_int(stmt, 4);
				car[car_cnt].out_month = sqlite3_column_int(stmt, 5);
				car[car_cnt].out_day = sqlite3_column_int(stmt, 6);
				car[car_cnt].out_hour = sqlite3_column_int(stmt, 7);
				car[car_cnt].out_minute = sqlite3_column_int(stmt, 8);
				car[car_cnt].parking_num = sqlite3_column_int(stmt, 9);
				car_cnt++;

				result = sqlite3_step(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);
				if (result == SQLITE_DONE) break;
			}
			sqlite3_finalize(stmt);
		}
		//str.Format(_T("전날 주차:%d"), car_cnt);
		//AfxMessageBox(str);
		sqlite3_close(db);

		int monthsub, daysub;
		int monthp;
		int curmonth = timer.GetMonth();
		int curday = timer.GetDay();
		bool isOldCar = FALSE;
		CString str2;

		int monthday[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };	// 월이 다를 경우 계산을 위해

		// 윤년이면 2월의 일수를 29로 바꿔준다.
		if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) {
			monthday[2] = 29;
		}

		str.Format(_T(""));

		for (int i = 0; i < car_cnt; i++) {
			monthsub = curmonth - car[i].in_month;

			if (monthsub != 0) {		// 월이 달라지면 전달의 일수를 out_day에 더해준 후 계산
				monthp = monthday[car[i].in_month];
				daysub = (curday + monthp) - car[i].in_day;
			}
			else {
				daysub = curday - car[i].in_day;
			}
			
			if (daysub >= 3) {
				str2.Format(_T("%d "), car[i].parking_num+1);
				isOldCar = TRUE;
				str += str2;
			}			
		}

		if (isOldCar) {
			str = _T("주차번호 ") + str + _T("는 장기주차입니다.");
			AfxMessageBox(str);
		}

		if (monthday[2] == 29) {
			monthday[2] = 28;
		}

		
		str.Format(_T("%s/week%d_money.db"), program_path, week);
		//AfxMessageBox(str);
		strA = CStringA(str);
		access_ch = strA.GetBuffer();
		isFirst = _access(access_ch, 0); // 존재하지 않는 경우 -1

		if (isFirst != -1) {
			sprintf_s(weekMoney_db, sizeof(weekMoney_db), "week%d_money.db", week);
			if (sqlite3_open(weekMoney_db, &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
				AfxMessageBox(_T("no access"));
			}


			// 데이터베이스에서 가져와 결산창 업데이트
			sql_str.Format(_T("select * from money"));
			/*
			strA = CStringA(sql_str);
			sql = strA.GetBuffer();
			*/

			length = sql_str.GetLength() + 1;
			sql = new char[length];
			WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

			if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
				result = sqlite3_step(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);
				int i = 0;
				if (result == SQLITE_ROW) {
					for (i = 0; i < 7; i++) {
						accounts[i] = sqlite3_column_int(stmt, i);
						//str.Format(_T("money: %d, result: %d"), accounts[i], result);
						//AfxMessageBox(str);
					}
				}
				sqlite3_finalize(stmt);
			}
			sqlite3_close(db);

			//str.Format(_T("aa: %d"), accounts[2]);
			//AfxMessageBox(str);
			(m_dlgAC.m_strMon).Format(_T("%d원"), accounts[0]);
			(m_dlgAC.m_strTue).Format(_T("%d원"), accounts[1]);
			(m_dlgAC.m_strWed).Format(_T("%d원"), accounts[2]);
			(m_dlgAC.m_strThur).Format(_T("%d원"), accounts[3]);
			(m_dlgAC.m_strFri).Format(_T("%d원"), accounts[4]);
			(m_dlgAC.m_strSat).Format(_T("%d원"), accounts[5]);
			(m_dlgAC.m_strSun).Format(_T("%d원"), accounts[6]);
		}

	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CParkingprojectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CParkingprojectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		//일단 전부 하얀색으로
		int x, y;
		CDC* pDC = m_PicCtrl.GetWindowDC();	// 이제부터 picture control에 그리겠다? 그런 의미인듯
		for (int i = 0; i < park_column; i++) {
			for (int j = 0; j < park_row; j++) {
				x = park_width * j;
				y = park_height * i;
				pDC->Rectangle(x + 2, y + 2, x + park_width + 2, y + park_height + 2);
			}
		}

		if (sqlite3_open("ParkManagement.db", &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
			AfxMessageBox(_T("no access"));
		}

		//전날 주차된 데이터베이스에 있는 주차공간은 초록색으로
		sql_str.Format(_T("select parking_num from car"));
		length = sql_str.GetLength() + 1;
		sql = new char[length];
		WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
			int parked;
			result = sqlite3_step(stmt);
			//str.Format(_T("update result:%d"), result);
			//AfxMessageBox(str);
			while (result == SQLITE_ROW) {
				parked = sqlite3_column_int(stmt, 0);
				str.Format(_T("%d"), parked);
				//AfxMessageBox(str);
				ToGreen(parked);

				result = sqlite3_step(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);
				if (result == SQLITE_DONE) break;
			}
			sqlite3_finalize(stmt);
		}
		sqlite3_close(db);
	}
}



void CParkingprojectDlg::OnClickedButtonIn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str, parked_str;
	int parking_num, parked_int;
	int result = 0;

	srand(time(NULL));							// 난수 사용
	UpdateData(TRUE);

	if (sqlite3_open("ParkManagement.db", &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
		AfxMessageBox(_T("no access"));
	}

	// 네자리만 받는 함수
	if (m_strInNum.GetLength() != 4) {
		AfxMessageBox(_T("4자리만 입력해주세요."));
		return;
	}

	if (car_cnt == park_row * park_column) {
		AfxMessageBox(_T("주차장이 가득찼습니다"));
		return;
	}

	// 차번호 겹치는지 확인 시작
	sql_str.Format(_T("select car_num from car"));
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
		result = sqlite3_step(stmt);
		//str.Format(_T("update result:%d"), result);
		//AfxMessageBox(str);
		while (1) {
			str.Format(_T("차번호:%s"), m_strInNum);
			AfxMessageBox(str);

			//str.Format(_T("result:%d"), result);  // result = 100은 테이블이 뒤에 더있다. 101은 테이블이 끝났다, 끝까지 갔다
			//AfxMessageBox(str);
			if (result == SQLITE_ROW) {
				for (int col = 0; col < car_cnt; col++) {
					//AfxMessageBox(_T("for"));
					parked_str = (char*)sqlite3_column_text(stmt, 0);
					//str.Format(_T("parked:%s"), parked_str);
					//AfxMessageBox(str);
					//str.Format(_T("park:%s, parked:%s"), m_strInNum, parked_str);
					//AfxMessageBox(str);
					if (m_strInNum.Compare(parked_str) == 0) {
						AfxMessageBox(_T("! 겹침 !"));
						AfxMessageBox(_T("차번호를 확인하세요"));
						//AfxMessageBox(_T("차번호 다시받기"));
						m_strInNum.Empty();
						return;
					}
					result = sqlite3_step(stmt);
					//str.Format(_T("update result:%d"), result);
					//AfxMessageBox(str);
					if (result == SQLITE_DONE) break;
				}
				//AfxMessageBox(_T("for문 out"));
				//AfxMessageBox(_T("겹치지않음"));
				car[car_cnt].car_num = m_strInNum;
				break;
			}
			else if (result == 101) {
				//AfxMessageBox(_T("첫번째 차"));
				//str.Format(_T("car_num:%s %d %d"), m_strInNum, car_cnt, car[car_cnt].car_num);
				//AfxMessageBox(str);
				//car[car_cnt].car_num = _T('1000');
				//str.Format(_T("car_num:%s"), car[car_cnt].car_num);
				//AfxMessageBox(str);
				car[car_cnt].car_num = m_strInNum;
				//str.Format(_T("car_num:%s"), car[car_cnt].car_num);
				//AfxMessageBox(str);
				break;
			}
		}
		sqlite3_finalize(stmt);
	}
	// 차번호 겹치는지 확인 끝

	// 주차공간 겹치는지 확인 시작
	sql_str.Format(_T("select parking_num from car"));
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
		result = sqlite3_step(stmt);
		//str.Format(_T("update result:%d"), result);
		//AfxMessageBox(str);
		while (1) {
			parking_num = rand() % 8;				// 난수를 이용하여 주차공간 설정
			//str.Format(_T("주차번호:%d"), parking_num);
			//AfxMessageBox(str);

			//str.Format(_T("result:%d"), result);  // result = 100은 응답이있다. 101은 테이블이 끝났다, 끝까지 갔다
			//AfxMessageBox(str);
			if (result == SQLITE_ROW) {
				for (int col = 0; col < car_cnt; col++) {
					//AfxMessageBox(_T("for"));
					parked_int = sqlite3_column_int(stmt, 0);
					//str.Format(_T("parked:%s"), parked_str);
					//AfxMessageBox(str);
					//str.Format(_T("park:%d, parked:%d"), parking_num, parked_int);
					//AfxMessageBox(str);
					result = sqlite3_step(stmt);
					//str.Format(_T("update result:%d"), result);
					//AfxMessageBox(str);
					if (parking_num == parked_int) {
						//AfxMessageBox(_T("! 겹침 !"));
						parking_num = -1;
						sqlite3_finalize(stmt);
						sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
						result = sqlite3_step(stmt);
						//str.Format(_T("update result:%d"), result);
						//AfxMessageBox(str);
					
						break;
					}
				}
				//AfxMessageBox(_T("for문 out"));
				if (parking_num != -1) {
					car[car_cnt].parking_num = parking_num;
					//AfxMessageBox(_T("겹치지않음"));
					break;
				}
			}
			else if(result == 101){
				car[car_cnt].parking_num = parking_num;
				break;
			}
			//AfxMessageBox(_T("주차번호 다시받기"));
		}
		sqlite3_finalize(stmt);
	}
	// 주차공간 겹치는지 확인 끝

	car[car_cnt].car_num = m_strInNum;			// 에디트 박스에 있는 차 번호를 idx번째 구조체에 저장
	m_strInNum.Empty();
	GetInTime();

	str.Format(_T("In : 시간:%d.%d.%d  차번호:%s  주차번호:%d"), 
		car[car_cnt].in_day,
		car[car_cnt].in_hour,
		car[car_cnt].in_minute,
		car[car_cnt].car_num,
		car[car_cnt].parking_num);
	AfxMessageBox(str);							// 제대로 들어갔나 확인

	// 데이터베이스 추가
	//AfxMessageBox(_T("insert"));
	sql_str.Format(_T("insert into car (car_num,in_month,in_day,in_hour,in_minute,parking_num) values (%s,%d,%d,%d,%d,%d);"),
		car[car_cnt].car_num,
		car[car_cnt].in_month,
		car[car_cnt].in_day,
		car[car_cnt].in_hour,
		car[car_cnt].in_minute,
		car[car_cnt].parking_num);
	//AfxMessageBox(sql_str);
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
		result = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		//str.Format(_T("update result:%d"), result);
		//AfxMessageBox(str);
	}
	sqlite3_close(db);

	ToGreen(car[car_cnt].parking_num);

	car_cnt++;
	UpdateData(FALSE);
}


void CParkingprojectDlg::OnClickedButtonOut()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str, parked_str;
	int i, col, target=0, result;

	if (sqlite3_open("ParkManagement.db", &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
		AfxMessageBox(_T("no access"));
	}

	UpdateData(TRUE);

	// 네자리만 받는 함수
	if (m_strOutNum.GetLength() != 4) {
		AfxMessageBox(_T("4자리만 입력해주세요."));
		return;
	}

	// 차번호 존재하는지 확인 시작
	sql_str.Format(_T("select car_num from car"));
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
		result = sqlite3_step(stmt);
		//str.Format(_T("update result:%d"), result);
		//AfxMessageBox(str);
		str.Format(_T("차번호:%s"), m_strOutNum);
		AfxMessageBox(str);

		//str.Format(_T("result:%d"), result);  // result = 100은 테이블이 뒤에 더있다. 101은 테이블이 끝났다, 끝까지 갔다
		//AfxMessageBox(str);
		if (result == SQLITE_ROW) {
			for (col = 0; col < car_cnt; col++) {
				//AfxMessageBox(_T("for"));
				parked_str = (char*)sqlite3_column_text(stmt, 0);
				//str.Format(_T("parked:%s"), parked_str);
				//AfxMessageBox(str);
				//str.Format(_T("park:%s, parked:%s"), m_strOutNum, parked_str);
				//AfxMessageBox(str);
				if (m_strOutNum.Compare(parked_str) == 0) {
					target = col;
					AfxMessageBox(_T("존재함"));
					m_strOutNum.Empty();
					GetOutTime(target);
					CostCal(target);						// 결산창에 요일에 맞춰 요금 저장
					m_strMoney.Format(_T("%d원"), cost);
					break;
				}
				result = sqlite3_step(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);
				if (result == SQLITE_DONE) {
					col = -1;
					break;
				}
			}
			//AfxMessageBox(_T("for문 out"));
			if (col == -1) {
				AfxMessageBox(_T("존재하지 않음"));
				AfxMessageBox(_T("차번호를 확인하세요"));
				return;
			}
		}
		else if (result == 101) {
			AfxMessageBox(_T("주차장에 1대도 없습니다"));
			m_strOutNum.Empty();
			return;
		}
		sqlite3_finalize(stmt);
	}
	// 차번호 존재하는지 확인 끝
	
	/*
	for (i = 0; i < 8; i++) {
		if (car[i].car_num == m_strOutNum) {
			m_strOutNum.Empty();
			GetOutTime(i);
			CostCal(i);						// 결산창에 요일에 맞춰 요금 저장
			m_strMoney.Format(_T("%d"), cost);
			break;
		}
	}
	*/

	UpdateData(FALSE);

	timer = CTime::GetCurrentTime();
	str.Format(_T("Out : 시간:%d.%d.%d  차번호:%s  주차번호:%d"), 
		timer.GetDay(),
		timer.GetHour(),
		timer.GetMinute(),
		parked_str,
		car[target].parking_num);
	AfxMessageBox(str);						// 제대로 나갔나 확인
											// *이거 나가고 나서 초기화 안됨

	//업데이트
	//AfxMessageBox(_T("update"));
	sql_str.Format(_T("update car set out_month=%d,out_day=%d,out_hour=%d,out_minute=%d where car_num=%s;"),
		timer.GetMonth(),
		timer.GetDay(),
		timer.GetHour(),
		timer.GetMinute(),
		parked_str);
	//AfxMessageBox(sql_str);
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
		result = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		//str.Format(_T("update result:%d"), result);
		//AfxMessageBox(str);
	}
	
	ToWhite(car[target].parking_num);

	// 가져오기
	//AfxMessageBox(_T("select"));
	str.Format(_T(""));
	sql_str.Format(_T("select * from car where car_num=%s;"), car[target].car_num);  // 가로줄 한줄만 가져오기
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
		int cols = sqlite3_column_count(stmt);
		//str.Format(_T("cols:%d"), cols);
		int result = 0;
		while (1) {
			result = sqlite3_step(stmt);
			//str.Format(_T("update result:%d"), result);
			//AfxMessageBox(str);
			if (result == SQLITE_ROW) {
				for (i = 0; i < cols; i++) {
					parked_str = (char*)sqlite3_column_text(stmt, i);
					str.Format(_T("%s %s"), str, parked_str);
				}
				//AfxMessageBox(str);
			}
			else {
				break;
			}
		}
		sqlite3_finalize(stmt);
	}
	
	//삭제하기
	//AfxMessageBox(_T("delete"));
	sql_str.Format(_T("delete from car where car_num=%s;"), car[target].car_num);
	//AfxMessageBox(sql_str);
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
		result = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		//str.Format(_T("update result:%d"), result);
		//AfxMessageBox(str);
	}
	sqlite3_close(db);
	
	//str.Format(_T("target:%d, car_cnt:%d"), target, car_cnt);
	//AfxMessageBox(str);


	//dow == 2 //요기
	str.Format(_T("week%d_records.db"), week);
	str.Format(_T("%s/%s"), program_path, str);
	//AfxMessageBox(str);
	strA = CStringA(str);
	access_ch = strA.GetBuffer();
	isFirst = _access(access_ch, 0); // 존재하지 않는 경우 -1

	if (isFirst == -1) {
		//차량출입기록 데이터베이스 열기
		sprintf_s(weekRecords_db, sizeof(weekRecords_db), "week%d_records.db", week);
		if (sqlite3_open(weekRecords_db, &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
			AfxMessageBox(_T("no access"));
		}

		if (timer.GetDay() == 1 || dow == 2) {
			// 차량출입기록 매주 월요일에 데이터베이스 테이블 생성
			sql_str.Format(_T("CREATE TABLE records(\
			id INTEGER PRIMARY KEY AUTOINCREMENT, \
			car_num VARCHAR(4), \
			in_day INTEGER, \
			in_hour INTEGER, \
			in_minute INTEGER, \
			out_day INTEGER default 0, \
			out_hour INTEGER default 0, \
			out_minute INTEGER default 0, \
			parking_num INTEGER);"));
			length = sql_str.GetLength() + 1;
			sql = new char[length];
			WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

			if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {	// sqlite3_prepare_v2() : sql(char*) -> stmt(sqlite3_stmt*) 형변환
				result = sqlite3_step(stmt);								// sqlite3_step() : 쿼리문 실행
				sqlite3_finalize(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);											
			}
			else {
				puts("테이블 생성에 실패했습니다.");
			}
		}

	}
	else {
		//차량출입기록 데이터베이스 열기
		sprintf_s(weekRecords_db, sizeof(weekRecords_db), "week%d_records.db", week);
		if (sqlite3_open(weekRecords_db, &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
			AfxMessageBox(_T("no access"));
		}
	}

	//데이터 생성
	sql_str.Format(_T("insert into records (car_num,in_day,in_hour,in_minute,out_day,out_hour,out_minute,parking_num) values (%s,%d,%d,%d,%d,%d,%d,%d);"),
		car[target].car_num,
		car[target].in_day,
		car[target].in_hour,
		car[target].in_minute,
		car[target].out_day,
		car[target].out_hour,
		car[target].out_minute,
		car[target].parking_num);
	//AfxMessageBox(sql_str);
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {	// sqlite3_prepare_v2() : sql(char*) -> stmt(sqlite3_stmt*) 형변환
		result = sqlite3_step(stmt);								// sqlite3_step() : 쿼리문 실행
		sqlite3_finalize(stmt);
		//str.Format(_T("update result:%d"), result);
		//AfxMessageBox(str);										
	}
	sqlite3_close(db);

	//target구조체 삭제하고 한칸씩 앞으로
	for (i = target; i < car_cnt; i++) {
		str.Format(_T("차번호:%s   In : 시간:%d.%d.%d.%d   Out : 시간:%d.%d.%d.%d   주차번호:%d"),
			car[i].car_num,
			car[i].in_month,
			car[i].in_day,
			car[i].in_hour,
			car[i].in_minute,
			car[i].out_month,
			car[i].out_day,
			car[i].out_hour,
			car[i].out_minute,
			car[i].parking_num);
		//AfxMessageBox(str);
		if (i == car_cnt - 1) break;
		car[i] = car[i + 1];
		/*
		car[i].car_num = car[i + 1].car_num;
		car[i].in_day = car[i + 1].in_day;
		car[i].in_hour = car[i + 1].in_hour;
		car[i].in_minute = car[i + 1].in_minute;
		car[i].out_day = car[i + 1].out_day;
		car[i].out_hour = car[i + 1].out_hour;
		car[i].out_minute = car[i + 1].out_minute;
		car[i].parking_num = car[i + 1].parking_num;
		*/
		str.Format(_T("차번호:%s   In : 시간:%d.%d.%d.%d   Out : 시간:%d.%d.%d.%d   주차번호:%d"),
			car[i].car_num,
			car[i].in_month,
			car[i].in_day,
			car[i].in_hour,
			car[i].in_minute,
			car[i].out_month,
			car[i].out_day,
			car[i].out_hour,
			car[i].out_minute,
			car[i].parking_num);
		//AfxMessageBox(str);
	}

	car_cnt--;
}


void CParkingprojectDlg::OnClickedButtonMoney()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 결산창 띄우는 함수
	

	if (m_bViewAC == FALSE) {
		m_dlgAC.Create(IDD_DIALOG_AC, this);

		CRect rectMain, rectAC;
		GetWindowRect(&rectMain);

		m_dlgAC.GetWindowRect(&rectAC);
		m_dlgAC.MoveWindow(rectMain.right, rectMain.top, rectAC.Width(), rectAC.Height());

		m_dlgAC.ShowWindow(SW_SHOW);
		m_bViewAC = TRUE;
	}
	else {
		m_dlgAC.ShowWindow(SW_HIDE);
		m_dlgAC.DestroyWindow();
		m_bViewAC = FALSE;
	}

}


void CParkingprojectDlg::GetInTime()
{
	// TODO: 여기에 구현 코드 추가.
	timer = CTime::GetCurrentTime();  // 시간 받기

	car[car_cnt].in_month = timer.GetMonth();
	car[car_cnt].in_day = timer.GetDay();
	car[car_cnt].in_hour = timer.GetHour();
	car[car_cnt].in_minute = timer.GetMinute();
}


void CParkingprojectDlg::GetOutTime(int car_cnt)
{
	// TODO: 여기에 구현 코드 추가.
	timer = CTime::GetCurrentTime();  // 시간 받기

	car[car_cnt].out_month = timer.GetMonth();
	car[car_cnt].out_day = timer.GetDay();
	car[car_cnt].out_hour = timer.GetHour();
	car[car_cnt].out_minute = timer.GetMinute();
}


void CParkingprojectDlg::CostCal(int car_cnt)
{
	// TODO: 여기에 구현 코드 추가.
		// 주차요금 계산기
	int monthsub, daysub, hoursub;
	int monthp;		// 전달의 일수를 저장
	int year = timer.GetYear();	// 윤년계산용

	//                   0  1   2   3   4   5   6   7   8   9   10  11  12
	int monthday[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };	// 월이 다를 경우 계산을 위해


	// 윤년이면 2월의 일수를 29로 바꿔준다.
	if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0) {
		monthday[2] = 29;
	}

	monthsub = car[car_cnt].out_month - car[car_cnt].in_month;

	if (monthsub != 0) {		// 월이 달라지면 전달의 일수를 out_day에 더해준 후 계산
		monthp = monthday[car[car_cnt].in_month];
		daysub = (car[car_cnt].out_day + monthp) - car[car_cnt].in_day;
	}
	else {
		daysub = car[car_cnt].out_day - car[car_cnt].in_day;
	}

	hoursub = car[car_cnt].out_hour;

	if (daysub > 0) {
		hoursub = hoursub + (24 * daysub);
	}

	hoursub = hoursub - car[car_cnt].in_hour;

	if (monthday[2] == 29) {
		monthday[2] = 28;
	}

	cost = 2000;
	if (hoursub != 0) {
		cost = 2000 * hoursub;
	}
	DayAccounts();
}


void CParkingprojectDlg::DayAccounts()
{
	// TODO: 여기에 구현 코드 추가.
	// 일수익 
	/*	
		daypropit에 그날의 수익 저장
		날이 바뀌면 0으로 초기화
		switch문을 사용하여 요일별로 출력
	*/

	timer = CTime::GetCurrentTime();
	CTime today_doweek;

	today_doweek = timer.GetDayOfWeek();

	if (today_doweek != pre_doweek) {
		daypropit = 0;
		pre_doweek = timer.GetDayOfWeek();
		daypropit = daypropit + cost;
	}
	else if (today_doweek == pre_doweek) {
		daypropit = daypropit + cost;
	}

	switch (dow) {	// 일요일부터 월요일까지 일별 수익저장
	case 1:
		//dayAc.sun_Accounts += daypropit;						// 일 수익을 각 요일별 결산 값에 넣음(데이터베이스 활용?)
		accounts[6] += daypropit;
		(m_dlgAC.m_strSun).Format(_T("%d원"), accounts[6]);	// edit control에 출력
		weekMoney_str.Format(_T("sun_Accounts = %d"), accounts[6]);
		break;

	case 2:
		//dayAc.mon_Accounts += daypropit;
		accounts[0] += daypropit;
		(m_dlgAC.m_strMon).Format(_T("%d원"), accounts[0]);
		weekMoney_str.Format(_T("mon_Accounts = %d"), accounts[0]);
		break;

	case 3:
		//dayAc.tue_Accounts += daypropit;
		accounts[1] += daypropit;
		(m_dlgAC.m_strTue).Format(_T("%d원"), accounts[1]);
		weekMoney_str.Format(_T("tue_Accounts = %d"), accounts[1]);
		//AfxMessageBox(weekMoney_str);
		break;

	case 4:
		//dayAc.wed_Accounts += daypropit;
		accounts[2] += daypropit;
		(m_dlgAC.m_strWed).Format(_T("%d원"), accounts[2]);
		weekMoney_str.Format(_T("wed_Accounts = %d"), accounts[2]);
		//AfxMessageBox(weekMoney_str);
		break;

	case 5:
		//dayAc.thur_Accounts += daypropit;
		accounts[3] += daypropit;
		(m_dlgAC.m_strThur).Format(_T("%d원"), accounts[3]);
		weekMoney_str.Format(_T("thur_Accounts = %d"), accounts[3]);
		break;

	case 6:
		//dayAc.fri_Accounts += daypropit;
		accounts[4] += daypropit;
		(m_dlgAC.m_strFri).Format(_T("%d원"), accounts[4]);
		weekMoney_str.Format(_T("fri_Accounts = %d"), accounts[4]);
		break;

	case 7:
		//dayAc.sat_Accounts += daypropit;
		accounts[5] += daypropit;
		(m_dlgAC.m_strSat).Format(_T("%d원"), accounts[5]);
		weekMoney_str.Format(_T("sat_Accounts = %d"), accounts[5]);
		break;
	}
}

/*
	나가는 차의 번호를 받아서 그 차가 빠지는 요일을 얻는다.
	요일별로 하면 요일별 수익을 받는 변수가 있어야 하나
	암튼 요일별 수익을 각 요일의 에디트 박스에 출력.
	리스트 컨트롤로 하고 싶었으나 리스트 컨트롤이 어려워 포기함
*/

void CParkingprojectDlg::ToGreen(int parking_num)
{
	// TODO: 여기에 구현 코드 추가.
	int row, column;
	int x, y;
	row = parking_num % park_row;
	column = parking_num / park_row;

	CBrush myBrush(RGB(0, 200, 0));
	CDC* pDC = m_PicCtrl.GetWindowDC();
	CBrush* pOld = pDC->SelectObject(&myBrush);

	x = row * park_width;
	y = column * park_height;
	pDC->Rectangle(x + 2, y + 2, x + park_width + 2, y + park_height + 2);

	pDC->SelectObject(pOld);
}


void CParkingprojectDlg::ToWhite(int parking_num)
{
	// TODO: 여기에 구현 코드 추가.
	int row, column;
	int x, y;
	row = parking_num % park_row;
	column = parking_num / park_row;

	CBrush myBrush(RGB(255, 255, 255));
	CDC* pDC = m_PicCtrl.GetWindowDC();
	CBrush* pOld = pDC->SelectObject(&myBrush);

	x = row * park_width;
	y = column * park_height;
	pDC->Rectangle(x + 2, y + 2, x + park_width + 2, y + park_height + 2);

	pDC->SelectObject(pOld);
}


BOOL CParkingprojectDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//주별결산 데이터베이스 열기
	//dow == 2 //요기

	str.Format(_T("%s/week%d_money.db"), program_path, week);
	//AfxMessageBox(str);
	strA = CStringA(str);
	access_ch = strA.GetBuffer();
	isFirst = _access(access_ch, 0); // 존재하지 않는 경우 -1

	if (isFirst == -1) {
		//차량출입기록 데이터베이스 열기
		sprintf_s(weekRecords_db, sizeof(weekRecords_db), "week%d_money.db", week);
		if (sqlite3_open(weekRecords_db, &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
			AfxMessageBox(_T("no access"));
		}

		if (timer.GetDay() == 1 || dow == 2) {
			// 주별결산 매주 월요일에 데이터베이스 테이블 생성
			sql_str.Format(_T("CREATE TABLE money (\
			mon_Accounts INTEGER default 0,\
			tue_Accounts INTEGER default 0,\
			wed_Accounts INTEGER default 0,\
			thur_Accounts INTEGER default 0,\
			fri_Accounts INTEGER default 0,\
			sat_Accounts INTEGER default 0,\
			sun_Accounts INTEGER default 0);"));
			length = sql_str.GetLength() + 1;
			sql = new char[length];
			WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

			if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {	// sqlite3_prepare_v2() : sql(char*) -> stmt(sqlite3_stmt*) 형변환
				result = sqlite3_step(stmt);								// sqlite3_step() : 쿼리문 실행
				sqlite3_finalize(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);											
			}
			else {
				puts("테이블 생성에 실패했습니다.");
			}

			//데이터 생성
			sql_str.Format(_T("insert into money values (0,0,0,0,0,0,0);"));
			length = sql_str.GetLength() + 1;
			sql = new char[length];
			WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);

			if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {	// sqlite3_prepare_v2() : sql(char*) -> stmt(sqlite3_stmt*) 형변환
				result = sqlite3_step(stmt);								// sqlite3_step() : 쿼리문 실행
				sqlite3_finalize(stmt);
				//str.Format(_T("update result:%d"), result);
				//AfxMessageBox(str);											
			}
		}
	}
	else {
		sprintf_s(weekRecords_db, sizeof(weekRecords_db), "week%d_money.db", week);
		if (sqlite3_open(weekRecords_db, &db) != SQLITE_OK) {	// 데이터베이스 열기 (안되면 오류)
			AfxMessageBox(_T("no access"));
		}
	}



	//업데이트
	//AfxMessageBox(_T("update"));
	sql_str.Format(_T("update money set %s"), weekMoney_str);
	//AfxMessageBox(sql_str);
	length = sql_str.GetLength() + 1;
	sql = new char[length];
	WideCharToMultiByte(CP_ACP, 0, sql_str, -1, sql, length, NULL, NULL);
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
		result = sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		//str.Format(_T("update result:%d"), result);
		//AfxMessageBox(str);
	}

	sqlite3_close(db);
	delete[] sql;
	//free(car);

	//파일 복사하기
	CString source;
	CString target;

	str.Format(_T("week%d_money.db"), week);
	source.Format(_T("%s/%s"), program_path, str);
	target.Format(_T("%s/%s"), folder_path, str);
	//AfxMessageBox(source);
	::CopyFileW(source, target, FALSE);

	str.Format(_T("week%d_records.db"), week);
	source.Format(_T("%s/%s"), program_path, str);
	target.Format(_T("%s/%s"), folder_path, str);
	//AfxMessageBox(target);
	::CopyFileW(source, target, FALSE);

	//매주 일요일 데이터베이스 삭제하기 // 요기
	//dow == 1
	if (timer.GetDay() == lastday || dow == 1) {
		str.Format(_T("%s/week%d_money.db"), program_path, week);
		//AfxMessageBox(str);
		::DeleteFileW(str);
		str.Format(_T("%s/week%d_records.db"), program_path, week);
		//AfxMessageBox(str);
		::DeleteFileW(str);
	}


	return CDialogEx::DestroyWindow();
}