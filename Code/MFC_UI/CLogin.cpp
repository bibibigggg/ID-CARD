// �α��� ���
// ���̵�� ��й�ȣ�� ��ġ���� �ƴ��� ����� ���ԵǾ� ����

// ���
#include "pch.h"
#include "CLogin.h"
#include "CDB.h"

// ������
CLogin::CLogin() {}

// �Ҹ���
CLogin::~CLogin() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// �α��� üũ �Լ�
bool CLogin::CheckLogin(CString InputID, CString InputPW)
{
	// CLoginDB ��ü ���� (�����ͺ��̽� ������ ���� ���)
	CLoginDB db;

	// DB ����
	if (!db.Connect())
	{
		// DB ���� ���� �� false ��ȯ
		AfxMessageBox(_T("�����ͺ��̽� ���ῡ �����߽��ϴ�."));
		return false;
	}

	// �Էµ� ���̵�� ��й�ȣ�� std::string���� ��ȯ (CLoginDB������ std::string ���)
	std::string userID = CT2A(InputID);
	std::string password = CT2A(InputPW);

	// DB���� ���̵�� ��й�ȣ üũ
	// *** CLoginDB.cpp 123 ���� ���� ***
	if (db.CheckLogin(userID, password))
	{
		// �α��� ����
		return true;
	}
	else
	{
		// �α��� ����
		return false;
	}
}
