#include "pch.h"
#include "IdChecker.h"

// ������ ȸ������ ���̵� Ȯ�� ��ư

// IdChecker ������
IdChecker::IdChecker(CWnd* parent, int idInputID)
    : m_parent(parent), m_idInputID(idInputID) // �θ� â�� ���̵� �Է� �ʵ� ID �ʱ�ȭ
{
}

// ���̵� ������ üũ �Լ�
bool IdChecker::CheckIdUniqueness(const CString& id)
{
    // DB�� �����ϱ� ���� CLoginDB ��ü ����
    CLoginDB db;
    if (!db.Connect()) {
        AfxMessageBox(L"�����ͺ��̽� ���ῡ �����߽��ϴ�.");
        return false; // ���� ���� �� �ߺ����� ����
    }

    // CString -> std::string ��ȯ
    std::string userId = CT2A(id);

    // �ߺ� üũ ���� ����
    std::string query = "SELECT id FROM Manager WHERE id = '" + userId + "';";
    if (!db.ExecuteQuery(query)) {
        AfxMessageBox(L"���� ���࿡ �����߽��ϴ�.");
        return false; // ���� �� �ߺ����� ����
    }

    // ���� ��� Ȯ��: �����ϸ� true, ������ false
    if (db.GetResult()->next()) {
        return false; // �ߺ��� ���̵� ����
    }
    return true; // �ߺ��� ���̵� ����
}

// ���̵� Ȯ�� ��ư�� Ŭ������ �� ȣ��Ǵ� �Լ�
void IdChecker::NotifyUser()
{
    CString id;
    // �θ� â���� ���̵� �ؽ�Ʈ ��������
    m_parent->GetDlgItemText(m_idInputID, id);

    // ���̵� �ߺ� üũ �� ����ڿ��� �˸�
    if (CheckIdUniqueness(id)) {
        AfxMessageBox(L"���̵� ��� �����մϴ�."); // ��� ������ ���̵��� ���
    } else {
        AfxMessageBox(L"���̵� �̹� ��� ���Դϴ�."); // �̹� ��� ���� ���̵��� ���
    }
}
