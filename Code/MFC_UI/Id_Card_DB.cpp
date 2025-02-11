// 등록할 때 등록된 신분증 데이터 출력
#include "pch.h"
#include "Id_Card_DB.h"
#include "MainDlg.h"


CId_Card_DB::CId_Card_DB()
{
    Connect();
}


CId_Card_DB::~CId_Card_DB()
{
    Disconnect();
}

// 입력받은 신분증 데이터를 MFC에 맞게 변환하는 함수
void CId_Card_DB::fetchRecentRow(CMainDlg* pDlg)
{
    try {
        // 데이터베이스 연결이 됬는지 안됬는지 확인
        if (!m_conn) {
            OutputDebugString(L"Database connection is not established.\n");
            return;
        }

        // 최신 데이터를 가져오기 위한 SQL 쿼리
        // `idcard` 테이블에서 최근 1개의 행을 가져오도록 요청
        std::string query = "SELECT name, pnumber, address, face, phnumber FROM idcard ORDER BY id DESC LIMIT 1";
        sql::PreparedStatement* stmt = m_conn->prepareStatement(query); // 쿼리 준비
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery()); // 쿼리 실행 및 결과 저장

        // 결과가 있는지 확인
        if (result->next()) {
            // 쿼리에서 가져온 데이터를 변수에 저장
            std::string name = result->getString("name");           // 이름
            std::string pnumber = result->getString("pnumber");     // 주민번호
            std::string address = result->getString("address");     // 주소
            std::string phnumber = result->getString("phnumber");   // 핸드폰 번호

            // 이미지(BLOB) 처리
            // `face` 열에서 이미지를 BLOB 형식으로 읽어옴
            std::istream* imageBuffer = result->getBlob("face");
            // BLOB 데이터를 `std::vector<uchar>`로 변환
            std::vector<uchar> db_image_data((std::istreambuf_iterator<char>(*imageBuffer)), std::istreambuf_iterator<char>());
            // OpenCV를 사용하여 이미지를 컬러로 디코딩
            cv::Mat db_image = cv::imdecode(db_image_data, cv::IMREAD_COLOR);

            // CString으로 변환 (MFC에서는 CString을 사용해야 함)
            CString m_name, m_address, m_pnumber, m_phnumber;
            MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, m_name.GetBuffer(name.length()), name.length());
            m_name.ReleaseBuffer();  // CString의 버퍼 해제
            MultiByteToWideChar(CP_UTF8, 0, address.c_str(), -1, m_address.GetBuffer(address.length()), address.length());
            m_address.ReleaseBuffer();
            MultiByteToWideChar(CP_UTF8, 0, pnumber.c_str(), -1, m_pnumber.GetBuffer(pnumber.length()), pnumber.length());
            m_pnumber.ReleaseBuffer();
            MultiByteToWideChar(CP_UTF8, 0, phnumber.c_str(), -1, m_phnumber.GetBuffer(phnumber.length()), phnumber.length());
            m_phnumber.ReleaseBuffer();

            // MainDlg의 메서드 호출하여 UI 업데이트
            // *** MainDlg.cpp 354 라인 참고 ***
            if (pDlg != nullptr) {
                pDlg->SetDataFromIdCard(m_name, m_pnumber, m_address, m_phnumber, db_image);
            }
        }
        else 
        {
            // 데이터베이스에서 최근 데이터가 없을 경우
            OutputDebugString(L"No recent row found in the database. Update failed.\n");  // 디버그 메시지 출력
            if (pDlg != nullptr) {
                AfxMessageBox(L"안됨");  // 메시지 박스를 띄워 사용자에게 알림
            }
        }
    }
    catch (sql::SQLException& e) {
        OutputDebugString(L"SQL Error occurred in fetchRecentRow: ");
        CString error;
        error.Format(L"%s", e.what());
        OutputDebugString(error);
    }
    catch (std::exception& e) {
        OutputDebugString(L"Standard Exception occurred: ");
        CString error;
        error.Format(L"%s", e.what());
        OutputDebugString(error);
    }
    catch (...) {
        OutputDebugString(L"Unknown error occurred.\n");
    }
}

