#pragma once
#pragma once

#include "afxwin.h"
#include <windows.h>
#include <string>

class CRunCSharpProgram
{
public:
    // 생성자 및 소멸자
    CRunCSharpProgram();
    ~CRunCSharpProgram();

    // C# 프로그램 실행 함수
    bool RunCSharpApp(const std::wstring& programPath);

private:
    STARTUPINFO si;             // 프로세스 시작 정보
    PROCESS_INFORMATION pi;     // 프로세스 정보

    // 프로세스 시작 및 종료 처리 함수
    bool StartProcess(const std::wstring& programPath);
    void CloseProcessHandles();
};