#pragma once
#include "afxdialogex.h"
#include "opencv2/opencv.hpp"
#include <thread>
#include <atomic>
#include <chrono>
#include "CamDB.h"

// 포워드 선언
class CMainDlg;
class CCamDB;

class Cam : public CDialogEx
{
    DECLARE_DYNAMIC(Cam)

public:
    // 생성자 및 소멸자
    Cam(CWnd* pParent = nullptr, CMainDlg* pMainDlg = nullptr);
    virtual ~Cam();

    // 주요 함수
    bool OpenCamera();                     // 카메라 열기
    bool UpdateFrame();                    // 프레임 업데이트
    bool CameraCapture(CCamDB& camDB, int imageID, const std::string PhoneNum, cv::Mat& img_frame); // 이미지 캡처 및 DB 전송
    void StopCamera();                     // 카메라 종료
    void CaptureImageAutomatically(CMainDlg& mg);
    std::string GetExecutablePath();

    // 멤버 변수
    cv::Mat img_frame;                     // 현재 프레임
    CString msg;                           // 로그 메시지
    CStatic* m_pPictureControl = nullptr;  // 이미지 표시 컨트롤
    bool cameraST = false;                 // 카메라 상태 플래그

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_MAIN_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // 데이터 교환
    DECLARE_MESSAGE_MAP()

private:
    // 내부 멤버 변수
    CMainDlg* m_pMainDlg = nullptr;        // 메인 다이얼로그 포인터
    cv::VideoCapture cap;                  // OpenCV 카메라 객체
    bool m_bCameraInitialized = false;     // 카메라 초기화 상태 플래그

    // 스레드 관련
    std::thread m_cameraThread;            // 카메라 프레임 처리 스레드
    std::atomic<bool> m_stopThread;        // 스레드 종료 플래그
    void CameraThreadFunction();           // 스레드 작업 함수

    std::chrono::time_point<std::chrono::steady_clock> detectionStartTime;  // 감지 시작 시간
    bool isObjectDetected = false;  // 객체 감지 여부
    bool isTimerRunning = false;    // 타이머가 실행 중인지 여부

public:
    afx_msg void OnDestroy();              // 다이얼로그 종료 처리
};
