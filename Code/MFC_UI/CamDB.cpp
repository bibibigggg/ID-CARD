#include "pch.h"
#include "CamDB.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <afx.h>

// ������: �θ� Ŭ������ �����ͺ��̽� ���� ����
CCamDB::CCamDB() {
    Connect();
}

// �Ҹ���: �θ� Ŭ������ ���� ����
CCamDB::~CCamDB() {
    Disconnect();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// �̹��� �����͸� BLOB���� ��ȯ�ϰ� DB�� ����
bool CCamDB::Send_Img2DB(int imageID, const std::string& InputPhoneNum, cv::Mat& img_frame) {
    try {
        // �̹����� ����ִٸ�
        if (img_frame.empty()) {
            AfxMessageBox(_T("Failed to load image."));
            return false; // �̹��� �ε� ���н� ó��
        }

        // �̹��� �����͸� BLOB���� ��ȯ
        // *** �ش� �Լ��� CamDB.cpp 77 ���� ���� ***
        // BLOB���� ��ȯ���� �ʾҴٸ�
        if (!ConvertBlob(img_frame, img_Blob)) {
            AfxMessageBox(_T("Failed to convert image to BLOB format."));
            return false; // ��ȯ ���н� ó��
        }

        // ���� �غ� �� ����
        // ���������� image ���̺� id, idcard, phnumber �� ����
        std::string query = "REPLACE INTO image (id, idcard, phnumber) VALUES (?, ?, ?)";
        sql::PreparedStatement* stmt = m_conn->prepareStatement(query);

        // ������ �� ����
        stmt->setInt(1, imageID); // �̹��� ID
        std::istringstream blobStream(std::string(img_Blob.begin(), img_Blob.end())); // BLOB ������ ��Ʈ��
        stmt->setBlob(2, &blobStream); // BLOB ������ ����
        stmt->setString(3, InputPhoneNum); // ��ȭ��ȣ �Է�

        // ���� ����
        stmt->executeUpdate();
        delete stmt; // ���� �� PreparedStatement ��ü ����

        OutputDebugString(L"Image data successfully inserted into the database.");
        return true; // ���������� �����ͺ��̽��� ����
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


// �̹��� �����͸� BLOB �������� ��ȯ
bool CCamDB::ConvertBlob(cv::Mat& img, std::vector<BYTE>& imageBlob) {
    try {
        // OpenCV�� imencode �Լ��� �̹����� BLOB �������� ���ڵ� (JPEG �������� ���ڵ�)
        // .jpg Ȯ���ڸ� ����Ͽ� JPEG �������� ���ڵ��ϰ�, ����� imageBlob ���Ϳ� ����
        if (!cv::imencode(".jpg", img, imageBlob)) {
            // ���ڵ� ���� �� false ��ȯ
            return false;
        }
        // ���ڵ��� ���������� true ��ȯ
        return true;
    }
    catch (cv::Exception& e) {
        // OpenCV ���� ó��
        CString error;
        error.Format(L"OpenCV Error: %s", CString(e.what()));
        AfxMessageBox(error); // ���� �޽��� ���
    }
    // ���� �߻� �� false ��ȯ
    return false;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// �̹��� ���� ���� Ȯ��
bool CCamDB::CheckImageExists(int imageID) {
    try {
        // �����ͺ��̽����� �ش� �̹��� ID�� �˻��� ���� �غ�
        std::string query = "SELECT * FROM image WHERE id = ?";
        sql::PreparedStatement* stmt = m_conn->prepareStatement(query);
        stmt->setInt(1, imageID); // ���� �Ķ���Ϳ� �̹��� ID ����

        // ���� ���� �� ��� ���� ���
        sql::ResultSet* result = stmt->executeQuery();

        // ����� �����ϸ� true ��ȯ (�̹����� �����ϸ�)
        bool exists = result->next();

        // ����� ���ҽ� ����
        delete result;
        delete stmt;

        return exists;  // �̹����� �����ϸ� true, �ƴϸ� false ��ȯ
    }
    catch (sql::SQLException& e) {
        // SQL ���ܰ� �߻��ϸ� ���� �޽��� ���
        CString error;
        error.Format(L"SQL Error: %s", CString(e.what()));
        AfxMessageBox(error);
    }
    return false;  // ���� �߻� �� false ��ȯ
}
