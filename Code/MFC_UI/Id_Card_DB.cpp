// ����� �� ��ϵ� �ź��� ������ ���
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

// �Է¹��� �ź��� �����͸� MFC�� �°� ��ȯ�ϴ� �Լ�
void CId_Card_DB::fetchRecentRow(CMainDlg* pDlg)
{
    try {
        // �����ͺ��̽� ������ ����� �ȉ���� Ȯ��
        if (!m_conn) {
            OutputDebugString(L"Database connection is not established.\n");
            return;
        }

        // �ֽ� �����͸� �������� ���� SQL ����
        // `idcard` ���̺��� �ֱ� 1���� ���� ���������� ��û
        std::string query = "SELECT name, pnumber, address, face, phnumber FROM idcard ORDER BY id DESC LIMIT 1";
        sql::PreparedStatement* stmt = m_conn->prepareStatement(query); // ���� �غ�
        std::unique_ptr<sql::ResultSet> result(stmt->executeQuery()); // ���� ���� �� ��� ����

        // ����� �ִ��� Ȯ��
        if (result->next()) {
            // �������� ������ �����͸� ������ ����
            std::string name = result->getString("name");           // �̸�
            std::string pnumber = result->getString("pnumber");     // �ֹι�ȣ
            std::string address = result->getString("address");     // �ּ�
            std::string phnumber = result->getString("phnumber");   // �ڵ��� ��ȣ

            // �̹���(BLOB) ó��
            // `face` ������ �̹����� BLOB �������� �о��
            std::istream* imageBuffer = result->getBlob("face");
            // BLOB �����͸� `std::vector<uchar>`�� ��ȯ
            std::vector<uchar> db_image_data((std::istreambuf_iterator<char>(*imageBuffer)), std::istreambuf_iterator<char>());
            // OpenCV�� ����Ͽ� �̹����� �÷��� ���ڵ�
            cv::Mat db_image = cv::imdecode(db_image_data, cv::IMREAD_COLOR);

            // CString���� ��ȯ (MFC������ CString�� ����ؾ� ��)
            CString m_name, m_address, m_pnumber, m_phnumber;
            MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, m_name.GetBuffer(name.length()), name.length());
            m_name.ReleaseBuffer();  // CString�� ���� ����
            MultiByteToWideChar(CP_UTF8, 0, address.c_str(), -1, m_address.GetBuffer(address.length()), address.length());
            m_address.ReleaseBuffer();
            MultiByteToWideChar(CP_UTF8, 0, pnumber.c_str(), -1, m_pnumber.GetBuffer(pnumber.length()), pnumber.length());
            m_pnumber.ReleaseBuffer();
            MultiByteToWideChar(CP_UTF8, 0, phnumber.c_str(), -1, m_phnumber.GetBuffer(phnumber.length()), phnumber.length());
            m_phnumber.ReleaseBuffer();

            // MainDlg�� �޼��� ȣ���Ͽ� UI ������Ʈ
            // *** MainDlg.cpp 354 ���� ���� ***
            if (pDlg != nullptr) {
                pDlg->SetDataFromIdCard(m_name, m_pnumber, m_address, m_phnumber, db_image);
            }
        }
        else 
        {
            // �����ͺ��̽����� �ֱ� �����Ͱ� ���� ���
            OutputDebugString(L"No recent row found in the database. Update failed.\n");  // ����� �޽��� ���
            if (pDlg != nullptr) {
                AfxMessageBox(L"�ȵ�");  // �޽��� �ڽ��� ��� ����ڿ��� �˸�
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

