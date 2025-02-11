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

// �Էµ� ���ڰ� ������ �� �����
void CSearchDB::Search_In_DB(const CString& str, SearchDlg* sd) {
    std::string query;
    sql::PreparedStatement* stmt = nullptr;  // SQL ������ �غ��� PreparedStatement ��ü
    std::unique_ptr<sql::ResultSet> result;  // ���� ���� ����� ���� ResultSet ��ü
    std::string str_utf8;  // UTF-8 �������� ��ȯ�� �˻���

    try {
        // �����ͺ��̽� ������ �Ǿ� �ִ��� Ȯ��
        if (!m_conn) {
            OutputDebugString(L"Database connection is not established.\n");
            return;
        }

        if (!str.IsEmpty()) {
            // �˻�� ��� ���� ������ �̸� �Ǵ� ��ȭ��ȣ�� �˻�
            query = "SELECT name, pnumber, address, passnumber, phnumber FROM idcard WHERE name LIKE ? OR phnumber LIKE ?";
            stmt = m_conn->prepareStatement(query);

            // �Էµ� �˻�� UTF-8�� ��ȯ�Ͽ� ���ʿ� '%'�� �߰� (LIKE �������� ���)
            str_utf8 = "%" + std::string(CT2A(str, CP_UTF8)) + "%";

            stmt->setString(1, str_utf8);  // ù ��° �Ķ���Ϳ� �̸��� LIKE �˻�
            stmt->setString(2, str_utf8);  // �� ��° �Ķ���Ϳ� ��ȭ��ȣ�� LIKE �˻�

            result.reset(stmt->executeQuery());  // ���� ���� �� ��� �ޱ�
        }
        else {
            // �˻�� ��� ������ ��� �����͸� ������
            query = "SELECT name, pnumber, address, passnumber, phnumber FROM idcard";
            stmt = m_conn->prepareStatement(query);
            result.reset(stmt->executeQuery());  // ��ü �����͸� �������� ���� ����
        }

        // ���� ����� SearchDlg�� DisplayResults �޼��带 ���� ǥ��
        // *** SearchDlg.cpp 147 ���� ���� ***
        sd->DisplayResults(result);
    }
    catch (sql::SQLException& e) {
        // SQL ���ܰ� �߻��� ��� ���� �޽��� ���
        CString errorMsg;
        errorMsg.Format(L"SQL Error occurred: %S", e.what());
        OutputDebugString(errorMsg);
    }
    catch (std::exception& e) {
        // �Ϲ����� ���� �߻� �� ���� �޽��� ���
        CString errorMsg;
        errorMsg.Format(L"Standard Exception occurred: %S", e.what());
        OutputDebugString(errorMsg);
    }
    catch (...) {
        // �� �� ���� ���� �߻� �� ���� �޽��� ���
        OutputDebugString(L"Unknown error occurred.\n");
    }
}



