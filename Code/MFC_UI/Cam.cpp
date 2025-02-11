#include "pch.h"
#include "Main.h"
#include "afxdialogex.h"
#include "Cam.h"
#include "CamDB.h"
#include "MainDlg.h"
#include <thread>
#include <chrono>
#include <Windows.h>

// MFC 카메라 기능
IMPLEMENT_DYNAMIC(Cam, CDialogEx)

// Cam 생성자
Cam::Cam(CWnd* pParent /*=nullptr*/, CMainDlg* pMainDlg /*= nullptr*/)
    : CDialogEx(IDD_MAIN_DIALOG, pParent), m_pMainDlg(pMainDlg), m_stopThread(false)
{
    // 디버그 메시지로 생성자 호출 로그 출력
    CString debugMsg;
    debugMsg.Format(_T("Cam: 생성자 호출됨. pParent: %p, pMainDlg: %p\n"), pParent, pMainDlg);
    OutputDebugString(debugMsg);

    // 부모 다이얼로그 초기화
    if (m_pMainDlg == nullptr) // m_pMainDlg가 nullptr인 경우
    {
        m_pMainDlg = dynamic_cast<CMainDlg*>(pParent); // 부모 창을 CMainDlg* 타입으로 변환 시도
        if (m_pMainDlg == nullptr) // 변환 실패 시
        {
            OutputDebugString(_T("Cam: 부모 다이얼로그를 초기화할 수 없습니다.\n"));
            return; // 초기화 실패로 생성자 종료
        }
    }

    // Picture Control 포인터 초기화
    if (m_pMainDlg != nullptr) // m_pMainDlg가 올바르게 초기화된 경우
    {
        m_pPictureControl = &m_pMainDlg->m_PictureControl; // 메인 다이얼로그의 Picture Control 포인터 설정
        debugMsg.Format(_T("Cam: PictureControl 주소: %p\n"), m_pPictureControl);
        OutputDebugString(debugMsg); // Picture Control 포인터 주소를 디버그 출력
    }
    else // m_pMainDlg가 nullptr인 경우
    {
        m_pPictureControl = nullptr; // Picture Control 포인터를 nullptr로 설정
        OutputDebugString(_T("Cam: CMainDlg 포인터가 nullptr입니다.\n")); // 오류 메시지 출력
    }
}


// Cam 소멸자
Cam::~Cam()
{
    StopCamera(); // 스레드와 카메라 자원을 해제
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// 데이터 교환 함수
void Cam::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    // 부모 다이얼로그 가져오기
    if (m_pMainDlg == nullptr)
    {
        m_pMainDlg = dynamic_cast<CMainDlg*>(GetParent());
    }

    // Picture Control 설정
    if (m_pMainDlg) {
        m_pPictureControl = &m_pMainDlg->m_PictureControl;
        if (m_pPictureControl != nullptr) {
            OutputDebugString(_T("Cam: PictureControl이 정상적으로 연결되었습니다.\n"));
        } 
        else {
            OutputDebugString(_T("Cam: PictureControl이 nullptr입니다.\n"));
        }
    } 
    else {
        OutputDebugString(_T("Cam: 부모 다이얼로그를 가져오지 못했습니다.\n"));
    }
}

// 메시지 맵
BEGIN_MESSAGE_MAP(Cam, CDialogEx)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 카메라 열기 함수
bool Cam::OpenCamera()
{
    // 부모 다이얼로그 및 Picture Control 확인
    if (m_pMainDlg == nullptr)
    {
        m_pMainDlg = dynamic_cast<CMainDlg*>(GetParent());
        if (m_pMainDlg == nullptr)
        {
            OutputDebugString(_T("Cam: 부모 다이얼로그를 가져올 수 없습니다.\n"));
            return false;
        }
    }

    if (m_pPictureControl == nullptr)
    {      
        m_pPictureControl = &m_pMainDlg->m_PictureControl;
        if (m_pPictureControl == nullptr)
        {
            OutputDebugString(_T("Cam: PictureControl이 nullptr입니다.\n"));
            return false;
        }
    }

    // 이미 카메라가 열려 있는 경우
    if (m_bCameraInitialized)
    {
        OutputDebugString(_T("Cam: 카메라가 이미 열려 있습니다.\n"));
        return true;
    }

    // 카메라 열기 시도
    try {
        cap.open(0, cv::CAP_ANY); // 카메라 OPEN
        cameraST = true;          // 카메라 상태 플래그 TRUE

        if (cap.isOpened())
        {
            msg.Format(_T("카메라가 열렸습니다. ID: %d\n"), 0);
            OutputDebugString(msg);
        }
        else
        {
            OutputDebugString(_T("카메라를 열 수 없습니다.\n"));
            cameraST = false;
            return false;
        }

        // 기존 스레드 종료
        // 영상 송출이기 때문에 쓰레드가 중복으로 생성할 수 있기 때문에 사용
        if (m_cameraThread.joinable())
        {
            try
            {
                // 현재 실행 중인 쓰레드가 종료될 때 까지 기다리는 메서드
                m_cameraThread.join();

                // 쓰레드 종료 됬다면 쓰레드 종료 플래그 TRUE
                m_stopThread = true;
            }
            catch (const std::exception& e) {
                OutputDebugString(L"Exception caught during thread join: ");
                CString errorMessage;
                errorMessage.Format(L"%s", e.what());
                OutputDebugString(errorMessage);
            }
            catch (const std::system_error& e) {
                OutputDebugString(_T("Cam: OpenCamera에서 시스템 오류 발생\n"));
                msg.Format(_T("시스템 오류: %s"), e.what());
                AfxMessageBox(msg);
                cameraST = false;
                return false;
            }
        }
        // 스레드 시작
        m_bCameraInitialized = true; // 현재 카메라가 켜져있는지 아닌지 확인하는 플래그 
        m_stopThread = false;       // 카메라 쓰레드 정지 확인 플래그

        // 카메라 쓰레드 생성
        // *** Cam.cpp 203 라인 참고 ***
        m_cameraThread = std::thread(&Cam::CameraThreadFunction, this);
    }
    catch (const std::system_error& e)
    {
        OutputDebugString(_T("Cam: OpenCamera에서 시스템 오류 발생\n"));
        msg.Format(_T("시스템 오류: %s"), e.what());
        AfxMessageBox(msg);
        cameraST = false;
        return false;
    }
    catch (const std::exception& e)
    {
        OutputDebugString(_T("Cam: OpenCamera에서 일반 예외 발생\n"));
        msg.Format(_T("예외: %s"), e.what());
        AfxMessageBox(msg);
        cameraST = false;
        return false;
    }
    catch (...)
    {
        OutputDebugString(_T("Cam: OpenCamera에서 알 수 없는 예외 발생\n"));
        AfxMessageBox(_T("알 수 없는 오류가 발생했습니다."));
        cameraST = false;
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 카메라 스레드 함수
void Cam::CameraThreadFunction()
{
    // 쓰레드가 멈춰있지 않다면
    while (!m_stopThread)
    {
        // 프레임 업데이트
        // *** Cam.cpp 230 라인 참고 ***
        // 프레임 업데이트가 되지 않는다면
        if (!UpdateFrame())
        {
            OutputDebugString(_T("Cam: 카메라 프레임을 업데이트할 수 없습니다."));
            cameraST = false; // 카메라 상태 False
            break; // 쓰레드 정지
        }

        // 약 30 FPS로 프레임 업데이트
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }

    StopCamera(); // 스레드 종료 시 카메라 정리
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 프레임 처리
bool Cam::UpdateFrame()
{
    // Picture Control 유효성 검사
    // m_pPictureControl이 nullptr이거나, Picture Control의 핸들이 유효하지 않으면 프레임 업데이트를 진행할 수 없음
    if (m_pPictureControl == nullptr || !::IsWindow(m_pPictureControl->GetSafeHwnd())) {
        OutputDebugString(_T("Picture Control이 유효하지 않습니다.\n"));
        cameraST = false;
        return false;
    }

    // 카메라에서 프레임 읽기
    // 카메라에서 프레임을 읽을 수 없으면 처리 중지
    if (!cap.read(img_frame)) {
        OutputDebugString(_T("카메라 프레임을 읽을 수 없습니다.\n"));
        cameraST = false;
        return false;
    }

    // 이미지가 비어있는지 확인
    // 읽은 이미지가 비어 있으면 프레임 업데이트를 진행할 수 없음
    if (img_frame.empty()) {
        OutputDebugString(_T("이미지 읽기 실패: img_frame 비어 있음"));
        cameraST = false;
        __debugbreak(); // 여기에서 중단점 삽입
        return false;
    }

    // PictureControl의 클라이언트 영역 크기 가져오기
    CRect rect;
    m_pPictureControl->GetClientRect(&rect);  // Picture Control의 크기를 가져옴
    int width = rect.Width();
    int height = rect.Height();

    // 화면 비율을 유지하며 이미지 크기 조정
    // 이미지의 비율을 계산하여 Picture Control에 맞는 크기로 조정
    double aspectRatio = (double)img_frame.cols / img_frame.rows;
    if (width / aspectRatio <= height) {
        // 너비에 맞추어 이미지 크기 조정
        cv::resize(img_frame, img_frame, cv::Size(width, static_cast<int>(width / aspectRatio)));
    }
    else {
        // 높이에 맞추어 이미지 크기 조정
        cv::resize(img_frame, img_frame, cv::Size(static_cast<int>(height * aspectRatio), height));
    }

    // 이미지 전처리
    cv::Mat gray_img, blur, canny;

    try {
        // 그레이스케일 변환
        cv::cvtColor(img_frame, gray_img, cv::COLOR_BGR2GRAY);
        // 엣지 검출
        cv::Canny(gray_img, canny, 50, 150);
    }
    catch (const cv::Exception& e) {
        // OpenCV 예외 처리
        CString errorMsg;
        errorMsg.Format(_T("OpenCV 예외: %s"), CString(e.what()));
        AfxMessageBox(errorMsg);
        __debugbreak(); // 여기에서 중단점 삽입
        return false;
    }

    // 컨투어 및 조건에 맞는 객체 확인
    std::vector<std::vector<cv::Point>> contours;
    // 'canny' 이미지에서 외곽선 찾기 (Canny 엣지 검출 후)
    cv::findContours(canny, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    int detectedCount = 0;  // 감지된 신분증 개수 카운터
    bool objectDetected = false;  // 객체 감지 여부

    // 각 외곽선을 확인하여 조건에 맞는 객체 감지
    for (const auto& contour : contours) {
        // 외곽선의 면적 계산
        double area = cv::contourArea(contour);
        // 외곽선의 경계 사각형 계산 (bounding box)
        cv::Rect boundingBox = cv::boundingRect(contour);
        // 사각형 비율 계산 (가로:세로 비율)
        double aspectRatio = (double)boundingBox.width / boundingBox.height;

        std::vector<cv::Point> approx;

        // 외곽선을 다각형으로 근사화 (정확도는 0.02 * 외곽선 길이에 비례)
        cv::approxPolyDP(contour, approx, 0.02 * cv::arcLength(contour, true), true);

        // 조건을 만족하는 사각형 객체(예: 신분증)를 찾음
        if (approx.size() == 4 && area > 5000 && aspectRatio > 1.5 && aspectRatio < 2.5) {
            detectedCount++;

            // 객체 개수가 1일 때 (첫 번째 신분증 감지 시)
            if (detectedCount == 1)
            {
                // 객체에 경계상자 그리기 (신분증의 경계에 초록색 직사각형을 그림)
                cv::rectangle(img_frame, boundingBox, cv::Scalar(0, 255, 0), 2);

                // 객체 감지 플래그 TRUE로 설정
                objectDetected = true;
            }
        }
        detectedCount = 0; // 감지된 개수를 초기화
    }

    // 객체가 감지되었을 때의 처리
    if (objectDetected) 
    {
        // 객체가 감지되고 있을 경우
        if (!isObjectDetected) 
        {
            // 감지된 시점의 시간을 기록
            detectionStartTime = std::chrono::steady_clock::now();

            // 객체 감지 상태를 TRUE로 설정
            isObjectDetected = true;

            // 타이머가 작동 중임을 나타내는 플래그를 TRUE로 설정
            isTimerRunning = true;
        }

        // 감지되지 않았다면
        else 
        {
            // 현재 시간을 가져옴
            auto now = std::chrono::steady_clock::now();

            // 현재 시간과 객체 감지 시작 시간의 차이를 초단위로 계산해준다.
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - detectionStartTime).count();

            // 1초가 경과했고 타이머 러닝 플래그가 TRUE라면 자동 캡처 실행
            if (duration >= 1 && isTimerRunning) {
                // *** 해당함수 MainDlg.cpp  234 라인 참고 ***
                m_pMainDlg->CaptureImageAutomatically(img_frame);  // 이미지 자동 캡처 실행
                isTimerRunning = false;  // 타이머를 멈추고, 캡처 실행 후 더 이상 타이머를 진행하지 않음
            }
        }
    }
    else {
        // 객체가 더 이상 감지되지 않으면 감지 상태 초기화
        isObjectDetected = false;
        isTimerRunning = false;
    }

    // 이미지 표시 (Picture Control에 출력)
    CDC* pDC = m_pPictureControl->GetDC();
    if (!pDC) {
        // Picture Control의 CDC(디바이스 컨텍스트)를 가져옴
        //    -> CDC는 GDI(Graphics Device Interface)를 사용하여 화면에 그리기 위한 객체
        //    -> 여기서 CDC를 얻지 못하면 디버그 로그를 출력하고 함수 종료
        OutputDebugString(_T("HDC를 가져올 수 없습니다.\n"));
        return false; // CDC를 가져올 수 없으므로 이미지 출력 FALSE
    }

    HDC hdc = pDC->m_hDC;
    if (!hdc) {
        // CDC로부터 하드웨어 디바이스 컨텍스트(HDC)를 가져옴
        //    -> HDC는 GDI 호출을 통해 실제 화면이나 창에 그릴 수 있는 디바이스 핸들
        //    -> HDC가 유효하지 않으면 얻은 CDC를 해제하고 로그를 출력한 후 함수 종료
        m_pPictureControl->ReleaseDC(pDC); // 리소스 누수를 방지하기 위해 CDC 해제
        OutputDebugString(_T("HDC를 가져올 수 없습니다.\n"));
        return false; // HDC를 가져올 수 없으므로 이미지 출력 FALSE
    }


    // OpenCV Mat 객체의 이미지 데이터를 MFC CImage 객체로 변환
    CImage image;
    image.Create(img_frame.cols, img_frame.rows, 24);
    // CImage 객체를 생성하고, 변환할 이미지의 크기와 색상 깊이(24비트)를 설정

    for (int y = 0; y < img_frame.rows; y++) {
        // OpenCV Mat 객체의 각 행 데이터를 CImage의 픽셀 데이터로 복사
        memcpy(image.GetPixelAddress(0, y), img_frame.ptr(y), img_frame.cols * 3);
        // CImage는 RGB 형식으로 저장되므로, OpenCV Mat의 각 행 데이터를 픽셀 주소로 복사
    }

    // Picture Control에 변환된 이미지 출력
    image.BitBlt(hdc, 0, 0);
    // HDC(디바이스 컨텍스트)를 사용하여 CImage 객체의 내용을 Picture Control에 출력
    // (0, 0)은 이미지의 출력 시작 좌표를 나타냄

    // Picture Control의 DC(디바이스 컨텍스트) 해제
    m_pPictureControl->ReleaseDC(pDC);
    // DC는 시스템 리소스이므로 사용 후 반드시 해제해야 리소스 누수를 방지할 수 있음

    return true;
    // 이미지 출력 성공 시 true 반환

}

// 상대경로를 절대경로로 자동 변환하는 함수
std::string Cam::GetExecutablePath() {
    char buffer[MAX_PATH]; // 실행 파일 경로를 저장할 버퍼 (최대 경로 길이는 MAX_PATH)
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    // 현재 실행 중인 모듈(프로그램)의 전체 경로를 가져옴
    // GetModuleFileNameA: ANSI 버전 함수 사용
    // 첫 번째 인자 NULL은 현재 실행 중인 프로세스를 나타냄

    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    // 경로에서 마지막 슬래시(\\ 또는 /)의 위치를 찾음
    // Windows와 Unix 기반 시스템의 경로 구분자를 모두 고려함

    return std::string(buffer).substr(0, pos);
    // 슬래시 이전 부분을 잘라내서 실행 파일의 디렉토리 경로를 반환
}


// 캡처한 이미지 로컬 파일에 저장 한 후 DB에 업로드
// 로컬로 저장한 이유는 메모리로 메서드 처리 할 시 메모리 충돌나서 로컬에 저장하도록 바꿈
bool Cam::CameraCapture(CCamDB& camDB, int imageID, const std::string PhoneNum, cv::Mat& img_frame)
{
    try
    {
        // img_frame이 비어있는지 확인
        if (img_frame.empty())
        {
            AfxMessageBox(_T("Error: img_frame is empty. Cannot save image."));
            return false; // 이미지가 비어있다면, 오류 메시지 출력 후 False 반환 캡처 안됨
        }

        // 이미지 저장 경로
        std::string exePath = GetExecutablePath();
        std::string imagePath = exePath + "\\Images\\photo.jpg";

        // OpenCV를 사용해 이미지 파일로 저장
        if (!cv::imwrite(imagePath, img_frame))
        {
            AfxMessageBox(_T("Failed to save image to file."));
            return false;
        }
        
        // 이미지를 데이터베이스로 전송
        // *** 해당 함수  CamDB.cpp 21 라인 참고 ***
        if (camDB.Send_Img2DB(imageID, PhoneNum, img_frame))
        {
            return true; // 이미지 전송이 성공하면 true 반환
        }
        else
        {
            AfxMessageBox(_T("Failed to send image to the database."));
            return false; // 이미지 전송 실패 시 오류 메시지 출력 후 종료
        }
    }
    catch (const cv::Exception& e)
    {
        // OpenCV 관련 예외 처리
        CString errorMsg;
        errorMsg.Format(_T("OpenCV Error: %s"), CString(e.what()));
        AfxMessageBox(errorMsg); // 오류 메시지를 디버깅용으로 출력
    }
    catch (const std::exception& ex)
    {
        // 표준 예외 처리
        CString errorMsg;
        errorMsg.Format(_T("Standard Exception: %s"), CString(ex.what()));
        AfxMessageBox(errorMsg); // 오류 메시지를 디버깅용으로 출력
    }
    return false; // 예외 발생 시 false 반환
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 카메라 종료 함수
void Cam::StopCamera()
{
    // 1. 스레드 종료 플래그 설정
    m_stopThread = true; // 이 플래그가 true로 설정되면 카메라 스레드가 종료될 때까지 기다리게 됨

    // 2. 스레드 안전하게 종료
    if (m_cameraThread.joinable()) // 만약 스레드가 실행 중이면
    {
        try
        {
            m_cameraThread.join(); // 스레드가 종료될 때까지 대기
        }
        catch (...)
        {
            // 예외 발생 시 스레드 종료 시 아무 것도 하지 않음
            // 예외를 처리하지 않음, 단순히 스레드를 종료시키는 데 중점을 둠
        }
    }

    // 3. 카메라 자원 해제
    if (cap.isOpened() && cameraST == true) // 카메라가 열려 있고, 카메라 상태가 TRUE인 경우
    {
        try
        {
            cap.release(); // 카메라 자원 해제
            OutputDebugString(_T("Cam: 카메라가 종료되었습니다.\n")); // 디버그 메시지 출력
        }
        catch (...)
        {
            // 카메라 해제 중 예외 발생 시 메시지 출력
            OutputDebugString(_T("Cam: 카메라 해제 중 예외 발생\n"));
        }
    }

    // 4. 카메라 상태 초기화
    m_bCameraInitialized = false; // 카메라 초기화 상태를 FALSE로 설정
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 다이얼로그 소멸자 호출 시 처리
void Cam::OnDestroy()
{
    StopCamera();
    CDialogEx::OnDestroy();
}