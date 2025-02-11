#include "pch.h"
#include "Main.h"
#include "afxdialogex.h"
#include "CLoginDlg.h"
#include "MainDlg.h"
#include "CLogin.h"
#include "JoinDlg.h"
#include <gdiplus.h>  // GDI+ 헤더

using namespace Gdiplus;

// GDI+ 초기화 전역 변수
ULONG_PTR g_GdiPlusToken;

// CLoginDlg 대화 상자
IMPLEMENT_DYNAMIC(CLoginDlg, CDialogEx)

CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_LOGIN_DIALOG, pParent)
{
}

CLoginDlg::~CLoginDlg()
{
}

// 데이터 교환
void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

// 다이얼로그 초기화
BOOL CLoginDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // GDI+ 초기화
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_GdiPlusToken, &gdiplusStartupInput, NULL);

    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE 반환
}


void CLoginDlg::OnPaint()
{
    CPaintDC dc(this);  // 화면 DC 생성
    Graphics graphics(dc.GetSafeHdc());

    // PNG 파일 경로 지정
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
    // 이미지, x, y 좌표, 높이, 너비
    graphics.DrawImage(&img, 0, 0, rect.Width(), rect.Height());
}

void CLoginDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    // GDI+ 종료
    GdiplusShutdown(g_GdiPlusToken);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLoginDlg::OnBnClickedButtonLogin)
    ON_BN_CLICKED(IDC_BUTTON_JOIN, &CLoginDlg::OnBnClickedButtonJoin)
    ON_WM_PAINT()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// 키보드 이벤트
BOOL CLoginDlg::PreTranslateMessage(MSG* pMsg)
{
    // 키보드 메시지를 보냈을 때 이것이 KEYDOWN이고 KEYDOWN한 Parameter가 Enter키 일 경우
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) 
    {
        OnBnClickedButtonLogin();  // 로그인 버튼 이벤트 실행
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


// 로그인 버튼 이벤트
void CLoginDlg::OnBnClickedButtonLogin()
{
    GetDlgItemText(IDC_EDIT_ID, InputID);   // EditControl에 ID를 입력
    GetDlgItemText(IDC_EDIT_PW, InputPW);   // EditControl에 PW를 입력

    // 혹시 모를 공백이 생길 상황때문에 공백을 없애줌
    InputID.Trim();
    InputPW.Trim();

    // 로그인 백엔드 기능을 사용할 클래스 객체 생성
    CLogin login;

    // 아이디 비밀번호 일치 확인 기능
    // IF를 넣은 이유는 아이디와 비밀번호가 틀렸는지 맞았는지 확인하기 위해 사용
    // *** 해당 함수 CLogin.cpp 20 라인 참고 ***
    if (login.CheckLogin(InputID, InputPW)) // 맞은경우
    {
        // ID와 PW를 공백으로 만듬
        // 다른 관리자 아이디로 접근할 시 유용하라고 넣음
        SetDlgItemText(IDC_EDIT_ID, L""); 
        SetDlgItemText(IDC_EDIT_PW, L"");

        CMainDlg dlg; // 신분증 등록 화면을 출력하기 위한 객체 생성
        dlg.DoModal(); // Modal 형식으로 신분증 등록화면 생성
    }
    else // 틀린경우
    {
        // 메시지 박스 출력
        AfxMessageBox(_T("로그인 정보가 잘못 되었습니다."));
        
        // 공백으로 만듬
        SetDlgItemText(IDC_EDIT_ID, L"");
        SetDlgItemText(IDC_EDIT_PW, L"");
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 회원가입 버튼 이벤트
void CLoginDlg::OnBnClickedButtonJoin()
{
    JoinDlg dlg; // 회원가입 화면을 위한 객체 생성
    dlg.DoModal(); // 회원가입 화면 모달 형식으로 출력
}
