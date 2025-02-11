#include "pch.h"
#include "StartCshap.h"
#include <stdexcept>

CRunCSharpProgram::CRunCSharpProgram() 
{
    // STARTUPINFO �ʱ�ȭ
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi)); // PROCESS_INFORMATION �ʱ�ȭ
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;  // �ܼ� â �����
}

CRunCSharpProgram::~CRunCSharpProgram() 
{
    CloseProcessHandles();  // �Ҹ��ڿ��� �ڵ� �ݱ�
}

// C# ���α׷� ���� �Լ�
bool CRunCSharpProgram::RunCSharpApp(const std::wstring& programPath)
{
    try
    {
        // C# ���α׷� ����
        if (!StartProcess(programPath))
        {
            AfxMessageBox(L"���μ��� ���� ����!");
            return false;  // ���μ��� ���ۿ� ������ ���
        }

        // ���μ����� ����� ������ ��� (���μ��� �ڵ鸸 ���)
        DWORD dwWaitResult = WaitForSingleObject(pi.hProcess, INFINITE);
        if (dwWaitResult != WAIT_OBJECT_0)
        {
            // ���μ��� ���� ��� �� ���� �߻�
            CString errorMsg;
            errorMsg.Format(L"���μ��� ���� ��� �� ���� �߻�. ���� �ڵ�: %lu", GetLastError());
            AfxMessageBox(errorMsg);
            CloseProcessHandles();  // �ڵ� �ݱ�
            return false;
        }

        // ���μ��� ���� �� �ڵ� �ݱ�
        CloseProcessHandles();
        OutputDebugString(L"C# ���α׷� ���� �Ϸ�!\n");
        return true;
    }
    catch (const std::exception& e)
    {
        // C++ ���� ó��
        CString errorMsg;
        errorMsg.Format(L"���� �߻�: %hs", e.what());
        AfxMessageBox(errorMsg);
        CloseProcessHandles();  // �ڵ� �ݱ�
        return false;
    }
    catch (...)
    {
        // �� �� ���� ���� ó��
        AfxMessageBox(L"�� �� ���� ���� �߻�.");
        CloseProcessHandles();  // �ڵ� �ݱ�
        return false;
    }
}


// ���μ��� ���� �Լ�
bool CRunCSharpProgram::StartProcess(const std::wstring& programPath)
{
    ZeroMemory(&pi, sizeof(pi));  // PROCESS_INFORMATION ����ü �ʱ�ȭ

    // CreateProcess�� C# ���α׷� ����
    if (!CreateProcess(
        programPath.c_str(),  // ������ ���α׷��� ���
        NULL,                 // ����� �μ� (������ NULL)
        NULL,                 // ���μ��� ���� �Ӽ� (�⺻ �� ���)
        NULL,                 // ������ ���� �Ӽ� (�⺻ �� ���)
        FALSE,                // �ڽ� ���μ����� �ڵ��� ������� ����
        0,                    // ���μ��� �÷��� (�⺻ �� ���)
        NULL,                 // ȯ�� ���� (�⺻ �� ���)
        NULL,                 // ���� ���丮 (�⺻ �� ���)
        &si,                  // STARTUPINFO ����ü
        &pi                   // PROCESS_INFORMATION ����ü
    )) {
        DWORD error = GetLastError();  // ���� �ڵ� ȹ��

        // ���� �ڵ忡 ���� �߰����� ���� �޽��� ����
        CString errorMsg;
        errorMsg.Format(L"CreateProcess ����. ���� �ڵ�: %lu", error);

        // ���� �ڵ忡 ���� ��ü���� �޽����� ����
        switch (error) {
        case ERROR_FILE_NOT_FOUND:
            errorMsg += L"\n������ ã�� �� �����ϴ�. ��θ� Ȯ���ϼ���.";
            break;
        case ERROR_ACCESS_DENIED:
            errorMsg += L"\n�׼����� �źεǾ����ϴ�. ������ Ȯ���ϼ���.";
            break;
        case ERROR_INVALID_PARAMETER:
            errorMsg += L"\n�߸��� �Ű������Դϴ�. ���α׷� ��θ� Ȯ���ϼ���.";
            break;
        default:
            errorMsg += L"\n�� �� ���� ������ �߻��߽��ϴ�.";
            break;
        }

        AfxMessageBox(errorMsg);  // ���� �޽��� �ڽ� ���
        return false;  // ���� �� false ��ȯ
    }
    else {
        OutputDebugString(L"CreateProcess ����!\n");  // ����� â�� ���
        return true;  // ���� �� true ��ȯ
    }
}


// ���μ��� �ڵ� �ݱ�
void CRunCSharpProgram::CloseProcessHandles() {
    try 
    {
        // �ڵ� ��ȿ�� Ȯ��
        if (pi.hProcess != NULL && pi.hProcess != INVALID_HANDLE_VALUE) 
        {
            if (!CloseHandle(pi.hProcess)) 
            {
                DWORD error = GetLastError();
                CString errorMsg;
                errorMsg.Format(L"CloseHandle ����. ���� �ڵ�: %lu", error);
                AfxMessageBox(errorMsg);
            }
            else 
            {
                OutputDebugString(L"���μ��� �ڵ� �ݱ� ����.\n");
            }
            pi.hProcess = NULL;  // �ڵ� �ʱ�ȭ
        }
        else 
        {
            return;
        }
    }
    catch (...) 
    {
        AfxMessageBox(L"�� �� ���� ���� �߻�!");
    }
}
