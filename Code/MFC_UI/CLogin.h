#pragma once
#include "afxwin.h"
#include "CDB.h"  // CLoginDB를 포함하여 데이터베이스 연동

class CLogin
{
public:
	CLogin();
	~CLogin();

	bool CheckLogin(CString InputID, CString InputPW);

private:
	CString m_CorrectID, m_CorrectPW;
};
