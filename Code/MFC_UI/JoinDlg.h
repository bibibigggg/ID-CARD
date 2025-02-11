#pragma once
#include "CDB.h"
#include "PasswordChecker.h"
#include "IdChecker.h"

class JoinDlg : public CDialogEx {
    DECLARE_DYNAMIC(JoinDlg) // 클래스에 런타임 클래스 정보를 추가하는 매크로

public:
    JoinDlg(CWnd* pParent = nullptr);
    virtual ~JoinDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_JOIN_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    CLoginDB m_db;               // DB 연결 객체
    CString inputNAME;           // 입력된 이름
    CString inputID;             // 입력된 아이디
    CString inputPW;             // 입력된 비밀번호
    CString inputCHPW;           // 입력된 비밀번호 확인
    PasswordChecker* m_pwChecker;  // 비밀번호 확인 객체
    IdChecker* m_idChecker;        // 아이디 중복 확인 객체
    bool m_bIsIdChecked;         // 아이디 중복 확인 여부

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButtonJoin();
    afx_msg void OnBnClickedButtonIdCheck();
    afx_msg BOOL OnInitDialog();
    afx_msg void OnEnChangePassword();  // 비밀번호 입력 시 상태 업데이트
    afx_msg void OnEnChangeConfirmPassword(); // 비밀번호 확인 입력 시 상태 업데이트
    virtual void OnOK();  // 대화상자 종료 시 객체 삭제
    CString Check_PW(const CString& InputPW);

    // CString을 UTF-8로 변환하는 함수
    std::string CStringToUtf8(const CString& cstr);
    BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnPaint();
    afx_msg void OnStnClickedStaticPwCheck();
};
