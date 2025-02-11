#pragma once
#pragma once

#include "afxwin.h"
#include <windows.h>
#include <string>

class CRunCSharpProgram
{
public:
    // ������ �� �Ҹ���
    CRunCSharpProgram();
    ~CRunCSharpProgram();

    // C# ���α׷� ���� �Լ�
    bool RunCSharpApp(const std::wstring& programPath);

private:
    STARTUPINFO si;             // ���μ��� ���� ����
    PROCESS_INFORMATION pi;     // ���μ��� ����

    // ���μ��� ���� �� ���� ó�� �Լ�
    bool StartProcess(const std::wstring& programPath);
    void CloseProcessHandles();
};