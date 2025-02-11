// 신분증 등록 화면

#include "pch.h"
#include "MainDlg.h"
#include "framework.h"
#include "Cam.h"
#include "Id_Card_DB.h"
#include "afxdialogex.h"
#include "resource.h"
#include "StartCshap.h"
#include "SearchDlg.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory>
#include <afxwin.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 사용자 정의 메시지 정의
#define WM_UPDATE_CAMERA_FRAME (WM_USER + 1)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 생성자
CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_MAIN_DIALOG, pParent), m_pCam(nullptr) // CString 초기화
{
    // 각 Static Text에 들어갈 내용
    m_Name = L"이름";
    m_PNumber = L"주민번호";
    m_Address = L"주소";
    m_PhoneNum = L"핸드폰 번호";

    // 프로그램 아이콘 초기화
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// 소멸자
CMainDlg::~CMainDlg()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 데이터가 변경됬을 때 실행되는 이벤트
void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    // 각 내용들을 업데이트 해준다..
    DDX_Control(pDX, IDC_PICTURE, m_PictureControl);    // 신분증 등록 화면 영상
    DDX_Control(pDX, IDC_FACE_PIC, m_facePic);          // 추출한 신분증의 얼굴 사진
    DDX_Text(pDX, IDC_EDIT_NAME, m_Name);               // 신분증 이름
    DDX_Text(pDX, IDC_EDIT_ID, m_PNumber);              // 주민번호
    DDX_Text(pDX, IDC_EDIT_ADRESS, m_Address);          // 주소
    DDX_Text(pDX, IDC_EDIT_PH_NUM, m_PhoneNum);         // 핸드폰 번호
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 메시지 맵
BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CMainDlg::OnBnClickedButtonSearch)
    ON_MESSAGE(WM_RUN_FETCH_ROW, &CMainDlg::OnRunFetchRow)
    ON_WM_DESTROY() 
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 핸들 에러 예외처리
void CMainDlg::HandleException(const std::exception& e, const CString& context)
{
    CString msg;
    msg.Format(_T("%s 예외 발생: %S"), context, e.what());
    OutputDebugString(msg);
    AfxMessageBox(msg);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 알 수 없는 핸들 예외처리
void CMainDlg::HandleUnknownException(const CString& context)
{
    CString msg;
    msg.Format(_T("%s: 알 수 없는 예외 발생"), context);
    OutputDebugString(msg);
    AfxMessageBox(msg);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 초기화 이벤트 폼이 초기화 됬을 때 이벤트가 실행됨
BOOL CMainDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ICON 세팅
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // 텍스트 설정
    SetDlgItemText(IDC_STATIC_NAME, L"이름");
    SetDlgItemText(IDC_STATIC_ID, L"주민번호");
    SetDlgItemText(IDC_STATIC_ADRESS, L"주소");
    SetDlgItemText(IDC_STATIC_PHONE, L"핸드폰 번호");
    SetDlgItemText(IDC_STATIC_PH_NUM, L"핸드폰 번호");

    //// 텍스트 상자 초기화
    SetDlgItemText(IDC_EDIT_NAME, m_Name);
    SetDlgItemText(IDC_EDIT_ID, m_PNumber);
    SetDlgItemText(IDC_EDIT_ADRESS, m_Address);
    SetDlgItemText(IDC_EDIT_PH_NUM, m_PhoneNum);
    SetDlgItemText(IDC_EDIT_PHONE, L" ");

    // PICTURE 컨트롤 초기화
    m_PictureControl.SubclassDlgItem(IDC_PICTURE, this);
    m_facePic.SubclassDlgItem(IDC_FACE_PIC, this);

    // 컨트롤이 정상적으로 초기화되었으면 Invalidate 호출
    GetDlgItem(IDC_CAMERA2)->Invalidate();  // 그룹박스를 다시 그립니다
    GetDlgItem(IDC_CAMERA2)->UpdateWindow(); // 즉시 화면을 업데이트

    // 캠 객체가 nullptr일 때 -> 카메라가 켜지지 않았을 때
    if (m_pCam == nullptr)
    {
        try {
            // Cam 스마트 포인터 객체 생성
            // 스마트 포인터를 사용한 이유는 카메라가 꺼질 때, 
            // 연결이 끊겼을 때 자동으로 메모리를 해제하기 위해 사용함
            m_pCam = std::make_unique<Cam>(this, this);

            // 카메라가 열리지 않았다면 예외처리
            // *** 해당함수 Cam.cpp 94 라인 참고 ***
            if (!m_pCam->OpenCamera())
            {
                OutputDebugString(_T("카메라를 열 수 없습니다.\n"));
                m_pCam.reset(); // 스마트 포인터로 메모리 해제
            }
        }
        catch (const std::exception& e) {
            CString errorMsg;
            errorMsg.Format(L"Cam 객체 생성 중 예외 발생: %S", e.what());
            OutputDebugString(errorMsg);
            AfxMessageBox(errorMsg);
        }
        catch (...) {
            OutputDebugString(L"Cam 객체 생성 중 알 수 없는 예외 발생\n");
            AfxMessageBox(L"Cam 객체 생성 중 알 수 없는 예외 발생");
        }
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 키보드 이벤트
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
    // ESC 키가 눌렸는지 확인
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        // 대화상자 종료
        OnOK();
        return TRUE; // 메시지가 처리되었음을 나타냄
    }

    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        // Simulate button click when Enter is pressed
        OnBnClickedButtonSearch();
        return TRUE; // Prevent the Enter key from being processed normally
    }

    return CDialogEx::PreTranslateMessage(pMsg); // 기본 처리
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    CDialogEx::OnSysCommand(nID, lParam);
}

void CMainDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);

        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
        CDialogEx::OnPaint();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HCURSOR CMainDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 상대경로를 현재 실행되는 위치를 기점으로 절대경로로 자동 변환해주는 함수
std::string GetExecutablePath() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}


// 카메라 자동캡쳐를 실행하기 위한 함수
void CMainDlg::CaptureImageAutomatically(cv::Mat& img_frame) {
    try {
        // 캠이 열리지 않았을 때의 예외처리
        if (m_pCam == nullptr) {
            AfxMessageBox(_T("카메라가 열려있지 않습니다.")); // 카메라가 열려있지 않은 경우 메시지
            OutputDebugString(_T("m_pCam이 nullptr입니다.\n")); // 디버그 로그 추가
            return;
        }

        CString Input_Phone_num; // 핸드폰 번호를 입력받을 변수
        
        // Edit Control에 핸드폰 번호를 입력받는다
        // if를 사용하는 이유는 핸드폰 번호를 입력받지 않고 신분증 데이터 추출하는것을 방지하기 위해 사용
        if (GetDlgItemText(IDC_EDIT_PHONE, Input_Phone_num)) {
            Input_Phone_num.Trim(); // 공백 제거
            std::string Phone_num = CT2A(Input_Phone_num); // CString에서 string 형식으로 변환

            // 핸드폰 번호가 비어있지 않을 시...
            if (!Phone_num.empty()) {

                // 신분증 데이터를 전송하기 위한 데이터베이스 객체 생성 및 연결
                CCamDB camDB;

                // DB연결 예외처리
                if (!camDB.Connect()) {
                    AfxMessageBox(_T("데이터베이스 연결에 실패했습니다."));
                    return;
                }

                // 이미지 ID 설정
                // 해당 DB 테이블에는 행 1개만 존재한다.
                // 혹시 모를 오류를 위해 ID 값을 명시적으로 1로 정해둠 
                int imageID = 1;

                // CameraCapture 호출 및 DB 저장
                // *** Cam.cpp 438라인 참고 ***
                if (m_pCam->CameraCapture(camDB, imageID, Phone_num, img_frame)) {
                    AfxMessageBox(_T("이미지 저장 및 데이터베이스 전송 성공."));

                    // C# 코드를 실행하기 위한 경로 설정
                    std::string exePath = GetExecutablePath();
                    std::wstring programpath = std::wstring(exePath.begin(), exePath.end()) + 
                        L"\\idcard_C\\idcard_C#\\bin\\Release\\net8.0\\idcard_C#.exe";

                    // 경로 설정이 완료 되었으면 C# 코드 실행
                    // *** 해당함수 StartCshap.cpp 21 라인 참고 ***
                    if (csp.RunCSharpApp(programpath)) {
                        // 추출한 신분증 데이터를 출력하기 위한 이벤트 호출
                        PostMessage(WM_RUN_FETCH_ROW);

                    }
                    else {
                        OutputDebugString(L"C# 코드가 제대로 실행되지 않았습니다.\n");
                    }
                }
                else {
                    AfxMessageBox(_T("이미지 저장 또는 데이터베이스 전송 실패."));
                }

                // 데이터베이스 연결 해제
                camDB.Disconnect();

                // IDC_STATIC_PHONE 텍스트를 공백으로 설정
                SetDlgItemText(IDC_EDIT_PHONE, L""); // 공백으로 설정
            }
            else {
                AfxMessageBox(L"먼저 핸드폰 번호를 입력하세요");
            }
        }
    }
    catch (const std::exception& e) {
        HandleException(e, _T("CaptureImageAutomatically"));
    }
    catch (...) {
        HandleUnknownException(_T("CaptureImageAutomatically"));
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// C# 코드를 실행하고 추출한 신분증 데이터 처리를 위한 함수
LRESULT CMainDlg::OnRunFetchRow(WPARAM wParam, LPARAM lParam) {
    try {
        // 신분증 데이터를 조회할 데이터베이스 연결 확인
        CId_Card_DB cId_Card_DB;
        OutputDebugString(L"데이터베이스 연결 시도 중...\n");

        // 데이터베이스 연결 여부 확인
        if (!cId_Card_DB.Connect()) {
            AfxMessageBox(L"데이터베이스 연결에 실패했습니다.");
            return 0; // 실패 시 0 반환
        }

        OutputDebugString(L"데이터베이스 연결 성공, 행 읽기 시도 중...\n");

        // CMainDlg 객체를 fetchRecentRow에 전달하여 UI 업데이트 수행
        // *** Id_Card_DB.cpp 20 라인 참고 *** 
        cId_Card_DB.fetchRecentRow(this); // `this`는 현재 CMainDlg 객체를 의미 
    }
    catch (const std::exception& e) {
        CString msg;
        msg.Format(L"예외 발생: %S", e.what());
        OutputDebugString(msg);
        AfxMessageBox(msg);
    }
    catch (...) {
        OutputDebugString(L"알 수 없는 예외 발생\n");
        AfxMessageBox(L"알 수 없는 예외 발생");
    }

    return 0; // 메시지 처리 완료
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 등록한 신분증 DB에서 값 불러올 때 사용
// 값을 불러오고 DB 값을 MFC 출력에 맞게 변환하여 각 Control에 출력
void CMainDlg::SetDataFromIdCard(const CString& name, 
    const CString& pnumber, 
    const CString& address, 
    const CString& phnumber, 
    cv::Mat db_image
)
{
    try {
        // 입력값에 공백을 제거하고 비어있는지 체크
        CString trimmedName = name;
        CString trimmedPNumber = pnumber;
        CString trimmedAddress = address;
        CString trimmedPhoneNum = phnumber;

        trimmedName.Trim();  // 앞뒤 공백 제거
        trimmedPNumber.Trim();
        trimmedAddress.Trim();
        trimmedPhoneNum.Trim();

        // 비어있는지 확인
        if (trimmedName.IsEmpty() || trimmedPNumber.IsEmpty() || trimmedAddress.IsEmpty() || trimmedPhoneNum.IsEmpty()) {
            AfxMessageBox(_T("입력값이 비어있습니다."));
            return;
        }

        // 세터 함수를 사용하여 데이터를 설정
        SetName(trimmedName);
        SetPNumber(trimmedPNumber);
        SetAddress(trimmedAddress);
        SetPhoneNum(trimmedPhoneNum);

        // 이미지 출력
        m_hBitmap = MatToHBITMAP(db_image);
        if (m_hBitmap) {
            m_facePic.SetBitmap(m_hBitmap);  // Picture Control에 이미지 설정
        }

        // 데이터 UI에 반영
        UpdateData(FALSE);  // FALSE: UI로 데이터를 설정
    }
    catch (const std::exception& e) {
        // 일반 예외 처리
        CString errorMsg;
        errorMsg.Format(L"예외 발생: %S", e.what());
        AfxMessageBox(errorMsg);  // 예외 메시지 표시
        OutputDebugString(L"예외 발생: ");
        OutputDebugString(CString(e.what()));
    }
    catch (...) {
        // 알 수 없는 예외 처리
        AfxMessageBox(L"알 수 없는 예외가 발생했습니다.");
        OutputDebugString(L"알 수 없는 예외 발생\n");
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 얼굴 출력을 위한 MAT 객체 BITMAP 변환
HBITMAP CMainDlg::MatToHBITMAP(const cv::Mat& mat)
{
    // 입력 Mat 객체가 비어 있는 경우 NULL 반환
    if (mat.empty())
    {
        return NULL;
    }

    // 신분증 얼굴을 출력할 Picture Control의 RECT 크기를 가져옴
    CRect rect;
    m_facePic.GetClientRect(&rect);

    // Picture Control의 크기에 맞게 비트맵 크기 계산
    int width = rect.Width();   // Picture Control의 가로 크기
    int height = rect.Height(); // Picture Control의 세로 크기

    // Mat 이미지를 Picture Control 크기에 맞게 리사이즈
    cv::Mat resizedMat;
    cv::resize(mat, resizedMat, cv::Size(width, height));

    // Picture Control에서 HDC를 가져옴
    HDC hdc = m_facePic.GetDC()->m_hDC;

    // HDC를 가져오지 못한 경우 디버그 로그 출력 후 NULL 반환
    if (hdc == nullptr)
    {
        OutputDebugString(_T("HDC를 가져올 수 없습니다.\n"));
        return NULL;
    }

    // 24비트 RGB 비트맵 이미지를 저장할 CImage 객체 생성
    CImage image;
    image.Create(resizedMat.cols, resizedMat.rows, 24);

    // OpenCV Mat 데이터의 픽셀 정보를 CImage 객체로 복사
    for (int y = 0; y < resizedMat.rows; y++)
    {
        // Mat의 한 행 데이터를 CImage 객체의 행에 복사
        memcpy(image.GetPixelAddress(0, y), resizedMat.ptr(y), resizedMat.cols * 3);
    }

    // Picture Control에 이미지를 출력
    image.BitBlt(hdc, 0, 0);

    // HDC 해제 (ReleaseDC를 통해 자원 반환)
    m_facePic.ReleaseDC(CDC::FromHandle(hdc));

    // 이 함수에서 HBITMAP을 반환하지 않는 경우 NULL 반환
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 신분증 정보 검색 버튼 이벤트
void CMainDlg::OnBnClickedButtonSearch()
{
    // 검색어를 입력받기 위한 변수
    CString searchstr;

    // 검색어가 존재할 때
    if (GetDlgItemText(IDC_EDIT_SEARCH, searchstr))
    {
        // 검색창을 띄우기 위한 객체 설정
        SearchDlg dlg;

        // *** 해당 함수 SearchDlg.cpp 103 라인 참고 ***
        dlg.SetdataFromMainDlgSearch(searchstr); // 검색한 검색어를 기반으로 검색창 출력
        dlg.DoModal();
        SetDlgItemText(IDC_EDIT_SEARCH, L""); 
    }
    // 존재하지 않을 때
    else
    {
        SearchDlg dlg;
        dlg.DoModal();
        SetDlgItemText(IDC_EDIT_SEARCH, L"");
    }
}
