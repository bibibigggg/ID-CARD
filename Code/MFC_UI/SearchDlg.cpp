// SearchDlg.cpp: 구현 파일

#include "pch.h"
#include "Main.h"
#include "afxdialogex.h"
#include "SearchDlg.h"
#include "CSearchDB.h"


// SearchDlg 대화 상자
IMPLEMENT_DYNAMIC(SearchDlg, CDialogEx)

SearchDlg::SearchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SEARCH_DIALOG, pParent)
{

}

SearchDlg::~SearchDlg()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
    //DrawListBox(); // 리스트 그리기
}

// SearchDlg 메시지 처리기
BEGIN_MESSAGE_MAP(SearchDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH2, &SearchDlg::OnBnClickedButtonSearch)
END_MESSAGE_MAP()



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


BOOL SearchDlg::OnInitDialog()
{
    if (CDialogEx::OnInitDialog())
    {
        CSearchDB sdb;
        DrawListBox(); // 리스트 그리기

        if (!m_searchstr.IsEmpty())
        {
            // DB에서 검색 결과 가져오기

            sdb.Search_In_DB(m_searchstr, this);
        }
        else
        {
            sdb.Search_In_DB(m_searchstr, this);
        }
    }
    return TRUE;
}

// 키보드 이벤트
// 엔터 키 누를 시 검색버튼 실행
BOOL SearchDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        OnBnClickedButtonSearch();
        return TRUE; 
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SearchDlg::OnBnClickedButtonSearch()
{
    CString searchstr;  // 검색어를 저장할 변수
    CSearchDB sdb;      // 데이터베이스 검색을 위한 CSearchDB 객체

    // IDC_EDIT_SEARCH2에서 입력된 텍스트를 searchstr에 가져오기
    if (GetDlgItemText(IDC_EDIT_SEARCH2, searchstr))
    {
        searchstr.Trim(); // 문자열 앞뒤 공백 제거

        // 검색어가 비어 있지 않다면 DB에서 검색 실행
        // *** CSearchDB.cpp 27 라인 참고 ***
        sdb.Search_In_DB(searchstr, this);
    }
    else
    {
        searchstr.Trim(); // 문자열 앞뒤 공백 제거 (빈 문자열의 경우에도 처리)

        // 검색어가 비어 있는 경우에도 DB에서 검색을 실행 (전체 데이터를 가져오기 위해)
        sdb.Search_In_DB(searchstr, this);
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 등록화면에서 가져온 검색어를 검색창에 전송
void SearchDlg::SetdataFromMainDlgSearch(const CString& searchMstr)
{
    // 전달된 검색어를 새로운 CString 변수에 저장
    CString Trimedstr = searchMstr;

    // 검색어 앞뒤 공백 제거
    Trimedstr.Trim();

    // 처리된 검색어를 클래스 내부의 변수에 설정
    Setstr(Trimedstr);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SearchDlg::DrawListBox()
{
    // 리스트 컨트롤의 스타일을 설정
    // LVS_REPORT: 리스트 컨트롤을 보고서 형식(열 기반)으로 설정
    // LVS_SHOWSELALWAYS: 항상 선택된 항목을 보여줌
    m_List.ModifyStyle(0, LVS_REPORT | LVS_SHOWSELALWAYS);

    // 확장 스타일 설정
    // LVS_EX_GRIDLINES: 그리드 선을 표시
    // LVS_EX_FULLROWSELECT: 행 전체를 선택할 수 있도록 설정
    m_List.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

    // 열 추가
    m_List.InsertColumn(0, _T("No"), LVCFMT_CENTER, 40);           // 첫 번째 열: "No", 중앙 정렬, 너비 40
    m_List.InsertColumn(1, _T("이름"), LVCFMT_CENTER, 100);        // 두 번째 열: "이름", 중앙 정렬, 너비 100
    m_List.InsertColumn(2, _T("주민번호"), LVCFMT_CENTER, 100);    // 세 번째 열: "주민번호", 중앙 정렬, 너비 100
    m_List.InsertColumn(3, _T("주소"), LVCFMT_CENTER, 300);        // 네 번째 열: "주소", 중앙 정렬, 너비 300
    m_List.InsertColumn(4, _T("패스키"), LVCFMT_CENTER, 100);      // 다섯 번째 열: "패스키", 중앙 정렬, 너비 100
    m_List.InsertColumn(5, _T("핸드폰 번호"), LVCFMT_CENTER, 100); // 여섯 번째 열: "핸드폰 번호", 중앙 정렬, 너비 100
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 리스트 박스에 결과 출력하는 함수
void SearchDlg::DisplayResults(std::unique_ptr<sql::ResultSet>& result)
{
    CSearchDB sdb;

    // 기존 리스트 컨트롤의 아이템을 모두 삭제
    m_List.DeleteAllItems();

    int rowIndex = 0;  // 행 인덱스를 0부터 시작

    // 데이터베이스 쿼리 결과를 한 행씩 처리
    while (result->next()) {
        // 각 필드 값을 CString으로 변환
        CString name = sdb.ConvertUTF8ToCString(result->getString("name"));             // 이름
        CString pnumber = sdb.ConvertUTF8ToCString(result->getString("pnumber"));       // 주민번호
        CString address = sdb.ConvertUTF8ToCString(result->getString("address"));       // 주소
        CString passnumber;                                                             // 안드로이드 어플에서 사용할 4자리수 고유 인증번호
        passnumber.Format(_T("%d"), result->getInt("passnumber"));                      // 패스키 (정수형을 CString으로 변환)
        CString phnumber = sdb.ConvertUTF8ToCString(result->getString("phnumber"));     // 핸드폰 번호

        // "No" 열에 해당 행의 인덱스 (1-based) 삽입
        CString strNo;
        strNo.Format(_T("%d"), rowIndex + 1); // 인덱스는 1부터 시작

        // 리스트 컨트롤에 각 항목 삽입
        m_List.InsertItem(rowIndex, strNo);          // 첫 번째 열에 "No" 삽입
        m_List.SetItemText(rowIndex, 1, name);       // 두 번째 열에 이름 삽입
        m_List.SetItemText(rowIndex, 2, pnumber);    // 세 번째 열에 주민번호 삽입
        m_List.SetItemText(rowIndex, 3, address);    // 네 번째 열에 주소 삽입
        m_List.SetItemText(rowIndex, 4, passnumber); // 다섯 번째 열에 패스키 삽입
        m_List.SetItemText(rowIndex, 5, phnumber);   // 여섯 번째 열에 핸드폰 번호 삽입

        rowIndex++; // 다음 행으로 넘어감
    }
}
