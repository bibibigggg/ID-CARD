#pragma once
#include "pch.h"
#include "CDB.h"
#include "SearchDlg.h"
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

class CSearchDB : public CLoginDB {
public:
	CSearchDB();
	~CSearchDB();

	void Print_DB(SearchDlg* dlg, const CString& str);
	CString ConvertUTF8ToCString(const std::string& utf8Str);
	void Search_In_DB(const CString& str, SearchDlg* sd);

	std::string name;
	std::string pnumber;
	std::string address;
	int passnumber;
	std::string phnumber;
	

	CString searchstr;


};