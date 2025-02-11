#include "pch.h"
#include "CSearchDB.h"
#include "SearchDlg.h"
#include <afxstr.h>

CSearchDB::CSearchDB()
{
	Connect();
}

CSearchDB::~CSearchDB()
{
	Disconnect();
}

CString CSearchDB::ConvertUTF8ToCString(const std::string& utf8Str) {
    CString result;
    int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
    if (sizeRequired > 0) {
        MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, result.GetBuffer(sizeRequired), sizeRequired);
        result.ReleaseBuffer();
    }
    return result;
}

// 입력된 문자가 공백일 때 출력함
void CSearchDB::Search_In_DB(const CString& str, SearchDlg* sd) {
    std::string query;
    sql::PreparedStatement* stmt = nullptr;  // SQL 쿼리를 준비할 PreparedStatement 객체
    std::unique_ptr<sql::ResultSet> result;  // 쿼리 실행 결과를 받을 ResultSet 객체
    std::string str_utf8;  // UTF-8 형식으로 변환된 검색어

    try {
        // 데이터베이스 연결이 되어 있는지 확인
        if (!m_conn) {
            OutputDebugString(L"Database connection is not established.\n");
            return;
        }

        if (!str.IsEmpty()) {
            // 검색어가 비어 있지 않으면 이름 또는 전화번호로 검색
            query = "SELECT name, pnumber, address, passnumber, phnumber FROM idcard WHERE name LIKE ? OR phnumber LIKE ?";
            stmt = m_conn->prepareStatement(query);

            // 입력된 검색어를 UTF-8로 변환하여 양쪽에 '%'를 추가 (LIKE 쿼리에서 사용)
            str_utf8 = "%" + std::string(CT2A(str, CP_UTF8)) + "%";

            stmt->setString(1, str_utf8);  // 첫 번째 파라미터에 이름을 LIKE 검색
            stmt->setString(2, str_utf8);  // 두 번째 파라미터에 전화번호를 LIKE 검색

            result.reset(stmt->executeQuery());  // 쿼리 실행 후 결과 받기
        }
        else {
            // 검색어가 비어 있으면 모든 데이터를 가져옴
            query = "SELECT name, pnumber, address, passnumber, phnumber FROM idcard";
            stmt = m_conn->prepareStatement(query);
            result.reset(stmt->executeQuery());  // 전체 데이터를 가져오는 쿼리 실행
        }

        // 쿼리 결과를 SearchDlg의 DisplayResults 메서드를 통해 표시
        // *** SearchDlg.cpp 147 라인 참고 ***
        sd->DisplayResults(result);
    }
    catch (sql::SQLException& e) {
        // SQL 예외가 발생한 경우 에러 메시지 출력
        CString errorMsg;
        errorMsg.Format(L"SQL Error occurred: %S", e.what());
        OutputDebugString(errorMsg);
    }
    catch (std::exception& e) {
        // 일반적인 예외 발생 시 에러 메시지 출력
        CString errorMsg;
        errorMsg.Format(L"Standard Exception occurred: %S", e.what());
        OutputDebugString(errorMsg);
    }
    catch (...) {
        // 알 수 없는 예외 발생 시 에러 메시지 출력
        OutputDebugString(L"Unknown error occurred.\n");
    }
}



