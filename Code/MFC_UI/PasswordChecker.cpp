#include "pch.h"
#include "PasswordChecker.h"


// 비밀번호, 비밀번호 확인 실시간으로 일치한지 아닌지 라벨 띄우는 기능

// PasswordChecker 생성자
PasswordChecker::PasswordChecker(CWnd* parent, int pwID, int checkPwID, int staticCheckID)
    : m_parent(parent), m_pwID(pwID), m_checkPwID(checkPwID), m_staticCheckID(staticCheckID)
{
   
}

// 비밀번호 상태 업데이트 함수
void PasswordChecker::UpdatePasswordStatus()
{
    // 비밀번호와 비밀번호 확인 텍스트 가져오기
    m_parent->GetDlgItemText(m_pwID, m_password);  // 비밀번호 가져오기
    m_parent->GetDlgItemText(m_checkPwID, m_confirmPassword);  // 비밀번호 확인 가져오기

    // 비밀번호 확인 상태 업데이트
    if (!m_confirmPassword.IsEmpty()) // 비밀번호 확인 필드가 비어있지 않으면
    {
        if (m_password == m_confirmPassword) {  // 비밀번호와 확인 비밀번호가 일치하면
            m_parent->GetDlgItem(m_staticCheckID)->SetWindowText(L"*비밀번호가 일치합니다.");  // 일치 메시지 출력
        }
        else {  // 비밀번호와 확인 비밀번호가 일치하지 않으면
            m_parent->GetDlgItem(m_staticCheckID)->SetWindowText(L"*비밀번호가 일치하지 않습니다.");  // 불일치 메시지 출력
        }
    }
}
