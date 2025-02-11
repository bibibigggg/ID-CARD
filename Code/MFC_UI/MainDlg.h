#pragma once

#include "afxwin.h"
#include "CamDB.h"
#include "Cam.h"
#include "StartCshap.h"
#include <memory>
#include <atomic>   // 스레드 안전성을 위한 atomic 추가
#include <exception>  // 예외 처리에 필요한 헤더 추가

class Cam;

// CMainDlg 대화 상자 클래스 정의
class CMainDlg : public CDialogEx {
public:
    // 생성자, 소멸자
    CMainDlg(CWnd* pParent = nullptr);
    virtual ~CMainDlg();

    // Cam 객체 포인터
    std::unique_ptr<Cam> m_pCam;   // 스마트 포인터로 변경

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MAIN_DIALOG };    // 대화 상자 리소스 ID
#endif

protected:
    // DDX/DDV 데이터 교환 및 유효성 검사 함수
    virtual void DoDataExchange(CDataExchange* pDX);

    // 키 입력 처리
    virtual BOOL PreTranslateMessage(MSG* pMsg) override;

    HBITMAP m_hBitmap;     // 로드된 이미지 비트맵 핸들

private:
    // 멤버 변수
    std::atomic<bool> m_stopThread;    // 스레드 종료 플래그 (스레드 안전성 보장)
    HICON m_hIcon;                     // 대화 상자 아이콘

    CRunCSharpProgram csp;             // C# 코드 실행 객체
   

    CString msg;                       // 메시지 문자열

    // UI 컨트롤
    CStatic m_StaticName;              // 이름 표시 컨트롤
    CStatic m_StaticId;                // 주민번호 표시 컨트롤
    CStatic m_StaticAddress;           // 주소 표시 컨트롤

    // 입력 변수

    CString Input_Phone_num;           // 핸드폰 번호 입력 필드
    std::string Phone_num;

    // 예외 처리 함수
    void HandleException(const std::exception& e, const CString& context);  // 일반 예외 처리
    void HandleUnknownException(const CString& context);                    // 알 수 없는 예외 처리
    HBITMAP MatToHBITMAP(const cv::Mat& mat);



    // 메시지 핸들러
    afx_msg LRESULT OnRunFetchRow(WPARAM wParam, LPARAM lParam);  // 데이터베이스에서 최근 행을 가져오는 이벤트 핸들러

public:
    // 멤버 함수
    virtual BOOL OnInitDialog();  // 대화 상자 초기화 함수
    void SetDataFromIdCard(const CString& name, const CString& pnumber, const CString& address, const CString& phnumber, cv::Mat db_image);

    CString m_Name;
    CString m_PNumber;
    CString m_Address;
    CString m_PhoneNum;

    // 게터
    CString GetName() const { return m_Name; }
    CString GetPNumber() const { return m_PNumber; }
    CString GetAddress() const { return m_Address; }
    CString GetPhoneNum() const { return m_PhoneNum; }

    //세터
    void SetName(const CString& name) { m_Name = name; }
    void SetPNumber(const CString& pNumber) { m_PNumber = pNumber; }
    void SetAddress(const CString& address) { m_Address = address; }
    void SetPhoneNum(const CString& phoneNum) { m_PhoneNum = phoneNum; }

    // UI 관련 메시지 핸들러
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam); // 시스템 명령 처리 함수
    afx_msg void OnPaint();                             // 대화 상자 그리기 함수
    afx_msg HCURSOR OnQueryDragIcon();                  // 드래그 시 커서 아이콘 반환 함수

    void CaptureImageAutomatically(cv::Mat& img_frame);
    afx_msg void OnBnClickedButtonSearch();

    // 카메라 관련 UI
    CStatic m_PictureControl;     // Picture Control
    CStatic m_facePic;            // face pic

    DECLARE_MESSAGE_MAP()         // 메시지 맵 선언

        // 사용자 정의 메시지 정의
#define WM_UPDATE_CAMERA_FRAME (WM_USER + 1)   // 카메라 프레임 업데이트 메시지
#define WM_RUN_FETCH_ROW       (WM_USER + 2)   
    void DisplayMessage(const CString& message);
    // 데이터베이스 행 가져오기 메시지
};
