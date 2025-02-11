#include "pch.h"
#include "PasswordChecker.h"


// ��й�ȣ, ��й�ȣ Ȯ�� �ǽð����� ��ġ���� �ƴ��� �� ���� ���

// PasswordChecker ������
PasswordChecker::PasswordChecker(CWnd* parent, int pwID, int checkPwID, int staticCheckID)
    : m_parent(parent), m_pwID(pwID), m_checkPwID(checkPwID), m_staticCheckID(staticCheckID)
{
   
}

// ��й�ȣ ���� ������Ʈ �Լ�
void PasswordChecker::UpdatePasswordStatus()
{
    // ��й�ȣ�� ��й�ȣ Ȯ�� �ؽ�Ʈ ��������
    m_parent->GetDlgItemText(m_pwID, m_password);  // ��й�ȣ ��������
    m_parent->GetDlgItemText(m_checkPwID, m_confirmPassword);  // ��й�ȣ Ȯ�� ��������

    // ��й�ȣ Ȯ�� ���� ������Ʈ
    if (!m_confirmPassword.IsEmpty()) // ��й�ȣ Ȯ�� �ʵ尡 ������� ������
    {
        if (m_password == m_confirmPassword) {  // ��й�ȣ�� Ȯ�� ��й�ȣ�� ��ġ�ϸ�
            m_parent->GetDlgItem(m_staticCheckID)->SetWindowText(L"*��й�ȣ�� ��ġ�մϴ�.");  // ��ġ �޽��� ���
        }
        else {  // ��й�ȣ�� Ȯ�� ��й�ȣ�� ��ġ���� ������
            m_parent->GetDlgItem(m_staticCheckID)->SetWindowText(L"*��й�ȣ�� ��ġ���� �ʽ��ϴ�.");  // ����ġ �޽��� ���
        }
    }
}
