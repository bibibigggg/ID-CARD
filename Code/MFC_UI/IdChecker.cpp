#include "pch.h"
#include "IdChecker.h"

// 관리자 회원가입 아이디 확인 버튼

// IdChecker 생성자
IdChecker::IdChecker(CWnd* parent, int idInputID)
    : m_parent(parent), m_idInputID(idInputID) // 부모 창과 아이디 입력 필드 ID 초기화
{
}

// 아이디 고유성 체크 함수
bool IdChecker::CheckIdUniqueness(const CString& id)
{
    // DB와 연결하기 위해 CLoginDB 객체 생성
    CLoginDB db;
    if (!db.Connect()) {
        AfxMessageBox(L"데이터베이스 연결에 실패했습니다.");
        return false; // 연결 실패 시 중복으로 간주
    }

    // CString -> std::string 변환
    std::string userId = CT2A(id);

    // 중복 체크 쿼리 실행
    std::string query = "SELECT id FROM Manager WHERE id = '" + userId + "';";
    if (!db.ExecuteQuery(query)) {
        AfxMessageBox(L"쿼리 실행에 실패했습니다.");
        return false; // 실패 시 중복으로 간주
    }

    // 쿼리 결과 확인: 존재하면 true, 없으면 false
    if (db.GetResult()->next()) {
        return false; // 중복된 아이디가 있음
    }
    return true; // 중복된 아이디가 없음
}

// 아이디 확인 버튼을 클릭했을 때 호출되는 함수
void IdChecker::NotifyUser()
{
    CString id;
    // 부모 창에서 아이디 텍스트 가져오기
    m_parent->GetDlgItemText(m_idInputID, id);

    // 아이디 중복 체크 후 사용자에게 알림
    if (CheckIdUniqueness(id)) {
        AfxMessageBox(L"아이디가 사용 가능합니다."); // 사용 가능한 아이디일 경우
    } else {
        AfxMessageBox(L"아이디가 이미 사용 중입니다."); // 이미 사용 중인 아이디일 경우
    }
}
