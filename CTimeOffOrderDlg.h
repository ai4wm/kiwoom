#pragma once
#include "afxdialogex.h"


// CTimeOffOrderDlg 대화 상자

class CTimeOffOrderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTimeOffOrderDlg)

public:
	CTimeOffOrderDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTimeOffOrderDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TIMEOFFODER };
#endif

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
	afx_msg void OnBnClickedCheckAuto();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCbnSelchangeComboacount();
	afx_msg void OnCbnSelchangeComboselrecttime();
	afx_msg void OnEnChangeEditjongcode();
	afx_msg void OnEnChangeEditnumber();
	void OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	void OnMarketClose(const CString& strMsg);
	void OnOrderComplete(const CString& strMsg);
	void OnOrderFailed(const CString& strMsg);
	void OnOrderResult(const CString& strOrderNo, const CString& strOrderStatus, const CString& strChejanQty);
	
private:
	void LoadAccountList(CComboBox* pComboAccount);
	void UpdateStateMessage(const CString& strMessage);
	void StartAutoOrder();
	void StopAutoOrder();
	void ProcessAutoOrder();
	BOOL IsOrderTime();
	BOOL CanSendOrder();
	void SendTimeOffOrder();
	
	// AppData 파일 저장/로드 관련 함수
	CString GetConfigFilePath();
	void SaveConfigToFile();
	void LoadConfigFromFile();
	
	// 자동주문 관련 변수
	BOOL m_bAutoOrderActive;
	UINT_PTR m_nTimerID;
	DWORD m_dwLastOrderTime;
	int m_nOrderAttempts;
	int m_nTargetHour;
	int m_nTargetMinute;
	BOOL m_bMarketClosed;
	
	// 마지막 주문 정보 저장
	CString m_strLastOrderCode;  // 마지막 주문 종목코드
	int m_nLastOrderQty;         // 마지막 주문 수량
	
	// 주문 상태 추적
	CString m_strCurrentOrderNo; // 현재 주문번호
	BOOL m_bOrderProcessing;     // 주문 처리 중 플래그
	int m_nOrderRetryCount;      // 주문 재시도 횟수
public:
	afx_msg void OnCbnSelchangeCombynsell();
};