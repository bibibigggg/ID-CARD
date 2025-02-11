#include "pch.h"
#include "CamDB.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <afx.h>

// 생성자: 부모 클래스의 데이터베이스 연결 수행
CCamDB::CCamDB() {
    Connect();
}

// 소멸자: 부모 클래스의 연결 해제
CCamDB::~CCamDB() {
    Disconnect();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 이미지 데이터를 BLOB으로 변환하고 DB에 저장
bool CCamDB::Send_Img2DB(int imageID, const std::string& InputPhoneNum, cv::Mat& img_frame) {
    try {
        // 이미지가 비어있다면
        if (img_frame.empty()) {
            AfxMessageBox(_T("Failed to load image."));
            return false; // 이미지 로드 실패시 처리
        }

        // 이미지 데이터를 BLOB으로 변환
        // *** 해당 함수는 CamDB.cpp 77 라인 참고 ***
        // BLOB으로 변환되지 않았다면
        if (!ConvertBlob(img_frame, img_Blob)) {
            AfxMessageBox(_T("Failed to convert image to BLOB format."));
            return false; // 변환 실패시 처리
        }

        // 쿼리 준비 및 실행
        // 쿼리문에서 image 테이블에 id, idcard, phnumber 값 삽입
        std::string query = "REPLACE INTO image (id, idcard, phnumber) VALUES (?, ?, ?)";
        sql::PreparedStatement* stmt = m_conn->prepareStatement(query);

        // 데이터 값 세팅
        stmt->setInt(1, imageID); // 이미지 ID
        std::istringstream blobStream(std::string(img_Blob.begin(), img_Blob.end())); // BLOB 데이터 스트림
        stmt->setBlob(2, &blobStream); // BLOB 데이터 설정
        stmt->setString(3, InputPhoneNum); // 전화번호 입력

        // 쿼리 실행
        stmt->executeUpdate();
        delete stmt; // 실행 후 PreparedStatement 객체 삭제

        OutputDebugString(L"Image data successfully inserted into the database.");
        return true; // 성공적으로 데이터베이스에 저장
    }
    catch (sql::SQLException& e) {
        CString error;
        error.Format(L"SQL Error: %s", CString(e.what()));
        AfxMessageBox(error);
    }
    catch (std::exception& ex) {
        CString error;
        error.Format(L"Standard Exception: %s", CString(ex.what()));
        AfxMessageBox(error);
    }
    return false;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 이미지 데이터를 BLOB 형식으로 변환
bool CCamDB::ConvertBlob(cv::Mat& img, std::vector<BYTE>& imageBlob) {
    try {
        // OpenCV의 imencode 함수로 이미지를 BLOB 형식으로 인코딩 (JPEG 포맷으로 인코딩)
        // .jpg 확장자를 사용하여 JPEG 형식으로 인코딩하고, 결과를 imageBlob 벡터에 저장
        if (!cv::imencode(".jpg", img, imageBlob)) {
            // 인코딩 실패 시 false 반환
            return false;
        }
        // 인코딩이 성공했으면 true 반환
        return true;
    }
    catch (cv::Exception& e) {
        // OpenCV 예외 처리
        CString error;
        error.Format(L"OpenCV Error: %s", CString(e.what()));
        AfxMessageBox(error); // 에러 메시지 출력
    }
    // 예외 발생 시 false 반환
    return false;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 이미지 존재 여부 확인
bool CCamDB::CheckImageExists(int imageID) {
    try {
        // 데이터베이스에서 해당 이미지 ID로 검색할 쿼리 준비
        std::string query = "SELECT * FROM image WHERE id = ?";
        sql::PreparedStatement* stmt = m_conn->prepareStatement(query);
        stmt->setInt(1, imageID); // 쿼리 파라미터에 이미지 ID 설정

        // 쿼리 실행 및 결과 집합 얻기
        sql::ResultSet* result = stmt->executeQuery();

        // 결과가 존재하면 true 반환 (이미지가 존재하면)
        bool exists = result->next();

        // 사용한 리소스 해제
        delete result;
        delete stmt;

        return exists;  // 이미지가 존재하면 true, 아니면 false 반환
    }
    catch (sql::SQLException& e) {
        // SQL 예외가 발생하면 에러 메시지 출력
        CString error;
        error.Format(L"SQL Error: %s", CString(e.what()));
        AfxMessageBox(error);
    }
    return false;  // 예외 발생 시 false 반환
}
