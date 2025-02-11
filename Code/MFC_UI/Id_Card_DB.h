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
#include <opencv2/opencv.hpp>  // OpenCV 헤더 추가
#include "MainDlg.h"

class CId_Card_DB : public CLoginDB { 
public:
    CId_Card_DB();  // 생성자
    ~CId_Card_DB(); // 소멸자

    // 데이터베이스에서 가장 최근 행을 가져오는 함수
    void fetchRecentRow(CMainDlg* pDlg);

};
