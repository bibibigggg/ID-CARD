#pragma once
#include "afxwin.h"
#include "CDB.h"  // CLoginDB�� �����Ͽ� �����ͺ��̽� ����

class CLogin
{
public:
	CLogin();
	~CLogin();

	bool CheckLogin(CString InputID, CString InputPW);

private:
	CString m_CorrectID, m_CorrectPW;
};
