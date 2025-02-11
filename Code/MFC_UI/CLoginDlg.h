#pragma once
#include "afxdialogex.h"
#include "CDB.h"

// CLoginDlg 대화 상자

class CLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CLoginDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	CString InputID, InputPW;	// CString 형식 문자열 변수(ID & PW)

private:
	CLoginDB* m_pdb;
	CString imagePath = _T("BCImage\\Login.png");  // 프로젝트 디렉토리에 PNG 파일 저장
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonJoin();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	virtual BOOL OnInitDialog();	

};
