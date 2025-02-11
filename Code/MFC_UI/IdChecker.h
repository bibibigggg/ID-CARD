#pragma once
#include <afxwin.h>
#include <string>
#include "CDB.h" // DB ���� Ŭ���� ����

class IdChecker {
public:
    IdChecker(CWnd* parent, int idInputID); // ������
    void NotifyUser(); // ����� �˸� �Լ�

private:
    CWnd* m_parent; // �θ� â
    int m_idInputID; // ���̵� �Է� �ʵ� ID

    bool CheckIdUniqueness(const CString& id); // ���̵� ������ Ȯ��
};
