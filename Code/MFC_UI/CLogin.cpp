// 로그인 기능
// 아이디와 비밀번호가 일치한지 아닌지 기능이 포함되어 있음

// 헤더
#include "pch.h"
#include "CLogin.h"
#include "CDB.h"

// 생성자
CLogin::CLogin() {}

// 소멸자
CLogin::~CLogin() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 로그인 체크 함수
bool CLogin::CheckLogin(CString InputID, CString InputPW)
{
	// CLoginDB 객체 생성 (데이터베이스 연결을 위해 사용)
	CLoginDB db;

	// DB 연결
	if (!db.Connect())
	{
		// DB 연결 실패 시 false 반환
		AfxMessageBox(_T("데이터베이스 연결에 실패했습니다."));
		return false;
	}

	// 입력된 아이디와 비밀번호를 std::string으로 변환 (CLoginDB에서는 std::string 사용)
	std::string userID = CT2A(InputID);
	std::string password = CT2A(InputPW);

	// DB에서 아이디와 비밀번호 체크
	// *** CLoginDB.cpp 123 라인 참고 ***
	if (db.CheckLogin(userID, password))
	{
		// 로그인 성공
		return true;
	}
	else
	{
		// 로그인 실패
		return false;
	}
}
