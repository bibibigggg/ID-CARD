#pragma once
#include <afxwin.h>
#include <string>
#include "CDB.h" // DB 연결 클래스 포함

class IdChecker {
public:
    IdChecker(CWnd* parent, int idInputID); // 생성자
    void NotifyUser(); // 사용자 알림 함수

private:
    CWnd* m_parent; // 부모 창
    int m_idInputID; // 아이디 입력 필드 ID

    bool CheckIdUniqueness(const CString& id); // 아이디 고유성 확인
};
