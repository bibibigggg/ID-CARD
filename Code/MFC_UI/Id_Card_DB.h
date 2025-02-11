#pragma once

#include "CDB.h"
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV ��� �߰�
#include "MainDlg.h"

class CId_Card_DB : public CLoginDB { 
public:
    CId_Card_DB();  // ������
    ~CId_Card_DB(); // �Ҹ���

    // �����ͺ��̽����� ���� �ֱ� ���� �������� �Լ�
    void fetchRecentRow(CMainDlg* pDlg);

};
