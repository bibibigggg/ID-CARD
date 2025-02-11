#include "pch.h"
#include "JoinDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "PasswordChecker.h"
#include "IdChecker.h"
#include <windows.h>  // WideCharToMultiByte를 사용하기 위한 헤더
#include <gdiplus.h>  // GDI+ 헤더

using namespace Gdiplus;
IMPLEMENT_DYNAMIC(JoinDlg, CDialogEx)

JoinDlg::JoinDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_JOIN_DIALOG, pParent),
    m_pwChecker(nullptr), // PasswordChecker 객체 초기화
    m_idChecker(nullptr), // IdChecker 객체 초기화
    m_bIsIdChecked(false) // 아이디 중복 확인 여부 초기화
{}

JoinDlg::~JoinDlg() {
    // 객체 해제
    if (m_pwChecker) {
        delete m_pwChecker;
    }
    if (m_idChecker) {
        delete m_idChecker;
    }
}

void JoinDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}


// 메시지 맵 MFC 대화상자에 이벤트를 생성하면 자동으로 생성됨
BEGIN_MESSAGE_MAP(JoinDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_JOIN, &JoinDlg::OnBnClickedButtonJoin)
    ON_BN_CLICKED(IDC_BUTTON_ID_CHECK, &JoinDlg::OnBnClickedButtonIdCheck)
    ON_EN_CHANGE(IDC_PW, &JoinDlg::OnEnChangePassword) 
    ON_EN_CHANGE(IDC_CHECK_PW, &JoinDlg::OnEnChangeConfirmPassword)
    ON_WM_PAINT()
    ON_STN_CLICKED(IDC_STATIC_PW_CHECK, &JoinDlg::OnStnClickedStaticPwCheck)
END_MESSAGE_MAP()

// 초기화 이벤트 다이얼로그가 초기화 되면 발생하는 이벤트
BOOL JoinDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    // DB 연결
    if (!m_db.Connect()) {
        EndDialog(IDCANCEL);  // 대화 상자를 종료
        return FALSE;
    }

    // PasswordChecker 객체 생성
    m_pwChecker = new PasswordChecker(this, IDC_PW, IDC_CHECK_PW, IDC_STATIC_CHECK);
    // IdChecker 객체 생성
    m_idChecker = new IdChecker(this, IDC_ID);

    // 초기 상태 메시지 설정
    GetDlgItem(IDC_STATIC_NAME)->SetWindowText(L"이름");
    GetDlgItem(IDC_STATIC_ID)->SetWindowText(L"아이디");
    GetDlgItem(IDC_STATIC_PW)->SetWindowText(L"비밀번호");
    GetDlgItem(IDC_STATIC_PW_CHECK)->SetWindowText(L"비밀번호 확인");
    GetDlgItem(IDC_STATIC_CHECK)->SetWindowText(L"*비밀번호가 일치하지 않습니다");
    GetDlgItem(IDC_STATIC_ID_CHECK)->SetWindowText(L"*영문, 숫자, 특수문자를 반드시 1개씩 포함해야 합니다.");

    return TRUE;
}

// CString -> std::string 변환 함수 (UTF-8로 변환)
std::string JoinDlg::CStringToUtf8(const CString& cstr) {
    int length = WideCharToMultiByte(CP_UTF8, 0, cstr.GetString(), -1, NULL, 0, NULL, NULL);
    if (length == 0) {
        return "";  // 변환 실패 시 빈 문자열 반환
    }

    std::string utf8Str(length - 1, 0);  // null-terminated 문자열을 포함하는 std::string
    WideCharToMultiByte(CP_UTF8, 0, cstr.GetString(), -1, &utf8Str[0], length, NULL, NULL);

    return utf8Str;
}

// 키보드 이벤트
BOOL JoinDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        // Simulate button click when Enter is pressed
        OnBnClickedButtonJoin();
        return TRUE; // Prevent the Enter key from being processed normally
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}

// 회원가입 버튼 클릭
void JoinDlg::OnBnClickedButtonJoin() {
    GetDlgItemText(IDC_NAME, inputNAME);
    GetDlgItemText(IDC_ID, inputID);
    GetDlgItemText(IDC_PW, inputPW);
    GetDlgItemText(IDC_CHECK_PW, inputCHPW);

    inputNAME.Trim();
    inputID.Trim();
    inputPW.Trim();
    inputCHPW.Trim();

    if (inputNAME.IsEmpty() || inputID.IsEmpty() || inputPW.IsEmpty() || inputCHPW.IsEmpty()) {
        AfxMessageBox(L"아이디 또는 비밀번호를 입력하세요.");
        return;
    }

    // ID 중복 체크
    if (!m_bIsIdChecked) {
        AfxMessageBox(L"아이디 중복 확인을 먼저 해주세요.");
        return;
    }

    if (Check_PW(inputPW) != "")
    {
        // string -> CString(UTF-8) 형변환
        std::string stdPW = CStringToUtf8(inputPW);     // PW
        std::string stdName = CStringToUtf8(inputNAME); // 이름
        std::string stdID = CStringToUtf8(inputID);     // ID

        // 회원가입 처리
        // *** CLoginDB.cpp 156 라인 참고 ***
        if (m_db.RegisterUser(stdID, stdPW, stdName))
        {
            AfxMessageBox(L"회원가입이 완료되었습니다.");
            EndDialog(IDOK);
        }
    }
}

// 아이디 중복 확인 버튼 클릭
void JoinDlg::OnBnClickedButtonIdCheck() {
    // 아이디 입력값 가져오기
    GetDlgItemText(IDC_ID, inputID);
    inputID.Trim();

    // 입력값 유효성 검사
    if (inputID.IsEmpty()) {
        AfxMessageBox(L"아이디를 입력하세요.");
        return;
    }

    // 아이디 중복 확인
    // *** IdChecker.cpp 참고 ***
    if (m_idChecker) {
        m_idChecker->NotifyUser();
        m_bIsIdChecked = true; // 아이디 중복 확인 완료 상태 설정
    }
}

// 비밀번호 입력 시 상태 업데이트
// ***PasswordChecker.cpp 참고***
void JoinDlg::OnEnChangePassword() {
    if (m_pwChecker) {
        m_pwChecker->UpdatePasswordStatus();
    }
}

// 비밀번호 확인 입력 시 상태 업데이트
void JoinDlg::OnEnChangeConfirmPassword() {
    if (m_pwChecker) {
        m_pwChecker->UpdatePasswordStatus();
    }
}

// 폼을 닫을때 이벤트 발생
void JoinDlg::OnOK()
{
}


// 비밀번호 정규식
// 8 ~ 12자리
// 영문, 숫자, 특수문자 반드시 1개 포함
CString JoinDlg::Check_PW(const CString& InputPW)
{
    // 입력받은 비밀번호의 길이를 저장
    int nCount = InputPW.GetLength();

    // 길이가 8보다 작고 13보다 크다면
    if (nCount < 8 && nCount > 13)
    {
        AfxMessageBox(L"비밀번호 길이는 8~12 자리 입니다.");
        return NULL;   // 에러 메시지 출력 후 NULL값 return 
    }

    // 영어입력, 숫자입력, 특수문자 입력 상태 플래그 설정
    bool bEngFlag = false, bNumFlag = false, bExFlag = false;

    // 특수문자 배열
    char szExt[] = { '!', '@', '#', '$', '%', '^', '&', '^', '(', ')' };

    // 입력받은 비밀번호 길이 만큼 반복
    for (int i = 0; i < nCount; i++)
    {
        // 현재 문자 (비밀번호의 i번째 문자)
        char cCnk = InputPW.GetAt(i);

        // bEngFlag가 아직 false라면, 현재 문자가 알파벳인지 체크
        if (!bEngFlag)
            bEngFlag = isalpha(cCnk);  // isalpha()는 문자가 알파벳인지 확인하는 함수

        // bNumFlag가 아직 false라면, 현재 문자가 숫자인지 체크
        if (!bNumFlag)
            bNumFlag = isdigit(cCnk);  // isdigit()는 문자가 숫자인지 확인하는 함수

        // bExFlag가 아직 false라면, 현재 문자가 특수문자 집합(szExt) 중 하나인지 확인
        if (!bExFlag)
        {
            // szExt는 특수문자들의 집합으로 가정, 예를 들어: `!@#$%^&*`
            for (int j = 0; j < sizeof(szExt); j++)
            {
                // 특수문자 배열(szExt)에서 현재 문자 cCnk가 존재하면
                if (cCnk == szExt[j])
                {
                    bExFlag = true;  // 특수문자 포함 플래그를 true로 설정
                    break;  // 특수문자를 찾았으므로 더 이상 반복할 필요 없음
                }
            }
        }
    }
    // 논리연산 영문, 숫자, 특수문자 하나라도 참이 아니라면 
    if (!bEngFlag || !bNumFlag || !bExFlag)
    {
        AfxMessageBox(L"반드시 특수문자 또는 영문, 숫자를 한 개 이상 포함해야 합니다.");
        return false;
    }

    else // 정규식 조건에 부합하다면
    {
        // 비밀번호 return
        return InputPW;
    }
}

void JoinDlg::OnPaint()
{
    CPaintDC dc(this);  // 화면 DC 생성
    Graphics graphics(dc.GetSafeHdc());

    // PNG 파일 경로 지정
    CString imagePath = _T("BCImage\\Join.png");  // 프로젝트 디렉토리에 PNG 파일 저장
    Image img(imagePath);

    // 이미지 로드 상태 확인
    if (img.GetLastStatus() != Ok) {
        OutputDebugString(_T("이미지 로드에 실패했습니다."));
        return;
    }

    // 창 크기 가져오기
    CRect rect;
    GetClientRect(&rect);

    // 이미지를 창 크기에 맞게 출력
    graphics.DrawImage(&img, 0, 0, rect.Width(), rect.Height());
}

// 잘못 만듦
void JoinDlg::OnStnClickedStaticPwCheck()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
