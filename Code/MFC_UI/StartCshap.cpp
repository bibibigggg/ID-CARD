#include "pch.h"
#include "StartCshap.h"
#include <stdexcept>

CRunCSharpProgram::CRunCSharpProgram() 
{
    // STARTUPINFO 초기화
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi)); // PROCESS_INFORMATION 초기화
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // 콘솔 창 숨기기
}

CRunCSharpProgram::~CRunCSharpProgram() 
{
    CloseProcessHandles();  // 소멸자에서 핸들 닫기
}

// C# 프로그램 실행 함수
bool CRunCSharpProgram::RunCSharpApp(const std::wstring& programPath)
{
    try
    {
        // C# 프로그램 시작
        if (!StartProcess(programPath))
        {
            AfxMessageBox(L"프로세스 시작 실패!");
            return false;  // 프로세스 시작에 실패한 경우
        }

        // 프로세스가 종료될 때까지 대기 (프로세스 핸들만 대기)
        DWORD dwWaitResult = WaitForSingleObject(pi.hProcess, INFINITE);
        if (dwWaitResult != WAIT_OBJECT_0)
        {
            // 프로세스 종료 대기 중 오류 발생
            CString errorMsg;
            errorMsg.Format(L"프로세스 종료 대기 중 오류 발생. 오류 코드: %lu", GetLastError());
            AfxMessageBox(errorMsg);
            CloseProcessHandles();  // 핸들 닫기
            return false;
        }

        // 프로세스 종료 후 핸들 닫기
        CloseProcessHandles();
        OutputDebugString(L"C# 프로그램 실행 완료!\n");
        return true;
    }
    catch (const std::exception& e)
    {
        // C++ 예외 처리
        CString errorMsg;
        errorMsg.Format(L"예외 발생: %hs", e.what());
        AfxMessageBox(errorMsg);
        CloseProcessHandles();  // 핸들 닫기
        return false;
    }
    catch (...)
    {
        // 알 수 없는 예외 처리
        AfxMessageBox(L"알 수 없는 예외 발생.");
        CloseProcessHandles();  // 핸들 닫기
        return false;
    }
}


// 프로세스 시작 함수
bool CRunCSharpProgram::StartProcess(const std::wstring& programPath)
{
    ZeroMemory(&pi, sizeof(pi));  // PROCESS_INFORMATION 구조체 초기화

    // CreateProcess로 C# 프로그램 실행
    if (!CreateProcess(
        programPath.c_str(),  // 실행할 프로그램의 경로
        NULL,                 // 명령줄 인수 (없으면 NULL)
        NULL,                 // 프로세스 보안 속성 (기본 값 사용)
        NULL,                 // 스레드 보안 속성 (기본 값 사용)
        FALSE,                // 자식 프로세스가 핸들을 상속하지 않음
        0,                    // 프로세스 플래그 (기본 값 사용)
        NULL,                 // 환경 변수 (기본 값 사용)
        NULL,                 // 현재 디렉토리 (기본 값 사용)
        &si,                  // STARTUPINFO 구조체
        &pi                   // PROCESS_INFORMATION 구조체
    )) {
        DWORD error = GetLastError();  // 오류 코드 획득

        // 오류 코드에 따른 추가적인 오류 메시지 제공
        CString errorMsg;
        errorMsg.Format(L"CreateProcess 실패. 오류 코드: %lu", error);

        // 오류 코드에 대한 구체적인 메시지를 제공
        switch (error) {
        case ERROR_FILE_NOT_FOUND:
            errorMsg += L"\n파일을 찾을 수 없습니다. 경로를 확인하세요.";
            break;
        case ERROR_ACCESS_DENIED:
            errorMsg += L"\n액세스가 거부되었습니다. 권한을 확인하세요.";
            break;
        case ERROR_INVALID_PARAMETER:
            errorMsg += L"\n잘못된 매개변수입니다. 프로그램 경로를 확인하세요.";
            break;
        default:
            errorMsg += L"\n알 수 없는 오류가 발생했습니다.";
            break;
        }

        AfxMessageBox(errorMsg);  // 오류 메시지 박스 출력
        return false;  // 실패 시 false 반환
    }
    else {
        OutputDebugString(L"CreateProcess 성공!\n");  // 디버그 창에 출력
        return true;  // 성공 시 true 반환
    }
}


// 프로세스 핸들 닫기
void CRunCSharpProgram::CloseProcessHandles() {
    try 
    {
        // 핸들 유효성 확인
        if (pi.hProcess != NULL && pi.hProcess != INVALID_HANDLE_VALUE) 
        {
            if (!CloseHandle(pi.hProcess)) 
            {
                DWORD error = GetLastError();
                CString errorMsg;
                errorMsg.Format(L"CloseHandle 실패. 오류 코드: %lu", error);
                AfxMessageBox(errorMsg);
            }
            else 
            {
                OutputDebugString(L"프로세스 핸들 닫기 성공.\n");
            }
            pi.hProcess = NULL;  // 핸들 초기화
        }
        else 
        {
            return;
        }
    }
    catch (...) 
    {
        AfxMessageBox(L"알 수 없는 예외 발생!");
    }
}
