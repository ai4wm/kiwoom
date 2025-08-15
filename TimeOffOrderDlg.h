//******************************************************************/
//******************************************************************/
//! All Rights Reserved. Copyright(c)   2014 (주)한국소프트웨어개발        /
//******************************************************************/
//! File Name     : TimeOffOrderDlg.h
//! Function      : 시간외주문 다이얼로그
//! System Name   : 키움 오픈API 테스트
//! Create        : , 2025/01/14
//! Update        : 
//! Comment       : 
//******************************************************************/
#pragma once

// CTimeOffOrderDlg 대화 상자
class CTimeOffOrderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTimeOffOrderDlg)

public:
	CTimeOffOrderDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTimeOffOrderDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TIMEOFFODER };

public:
	CString m_strScrNo;
	CWnd* m_pParent;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBtnOrder();
	afx_msg void OnBtnAutoOrder();
};