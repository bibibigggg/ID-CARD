#pragma once
#include <afxwin.h>

class PasswordChecker
{
public:
    PasswordChecker(CWnd* parent, int pwID, int checkPwID, int staticCheckID);
    void UpdatePasswordStatus();

private:
    CWnd* m_parent;
    int m_pwID;
    int m_checkPwID;
    int m_staticCheckID;

    CString m_password;
    CString m_confirmPassword;
};
