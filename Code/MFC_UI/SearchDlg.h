#pragma once
#include "afxdialogex.h"
#include <memory>
#include <string>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

// SearchDlg 대화 상자
class SearchDlg : public CDialogEx
{
    DECLARE_DYNAMIC(SearchDlg)

private:
    CString m_searchstr;    // 검색어

public:
    SearchDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
    virtual ~SearchDlg();
    void SetdataFromMainDlgSearch(const CString& searchMstr);
    CString Getsearchstr() const { return m_searchstr; }
    void Setstr(const CString& searchstr) { m_searchstr = searchstr; }

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SEARCH_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg BOOL PreTranslateMessage(MSG* pMsg);
    void OnBnClickedButtonSearch();
    CListCtrl m_List;  // 리스트 박스 컨트롤
    void DrawListBox();  // 리스트 박스 초기화 함수
    void DisplayResults(std::unique_ptr<sql::ResultSet>& result);  // 검색 결과 표시
};