// CTimeOffOrderDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "KhOpenApiTest.h"
#include "afxdialogex.h"
#include "CTimeOffOrderDlg.h"


// CTimeOffOrderDlg 대화 상자

IMPLEMENT_DYNAMIC(CTimeOffOrderDlg, CDialogEx)

CTimeOffOrderDlg::CTimeOffOrderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TIMEOFFODER, pParent)
{
	m_pParent = pParent;
	m_bAutoOrderActive = FALSE;
	m_nTimerID = 0;
	m_dwLastOrderTime = 0;
	m_nOrderAttempts = 0;
	m_nTargetHour = 8;
	m_nTargetMinute = 30;
	m_bMarketClosed = FALSE;
	m_nLastOrderQty = 0;
	m_bOrderProcessing = FALSE;
	m_nOrderRetryCount = 0;
}

CTimeOffOrderDlg::~CTimeOffOrderDlg()
{
}

void CTimeOffOrderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTimeOffOrderDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CTimeOffOrderDlg::OnBtnOrder)
	ON_BN_CLICKED(IDC_CHECKAUTO, &CTimeOffOrderDlg::OnBnClickedCheckAuto)
	ON_CBN_SELCHANGE(IDC_COMBOACOUNT, &CTimeOffOrderDlg::OnCbnSelchangeComboacount)
	ON_CBN_SELCHANGE(IDC_COMBOSELRECTTIME, &CTimeOffOrderDlg::OnCbnSelchangeComboselrecttime)
	ON_CBN_SELCHANGE(IDC_COMBYNSELL, &CTimeOffOrderDlg::OnCbnSelchangeCombynsell)
	ON_EN_CHANGE(IDC_EDITJONGCODE, &CTimeOffOrderDlg::OnEnChangeEditjongcode)
	ON_EN_CHANGE(IDC_EDITNUMBER, &CTimeOffOrderDlg::OnEnChangeEditnumber)
END_MESSAGE_MAP()


// CTimeOffOrderDlg 메시지 처리기

//*******************************************************************/
//! Function Name : OnInitDialog
//! Function      : 다이얼로그 초기화
//! Param         : void
//! Return        : BOOL
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
BOOL CTimeOffOrderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 다이얼로그 타이틀 설정
	SetWindowText(_T("시간외주문"));

	// 콤보박스 초기화
	CComboBox* pComboAccount = (CComboBox*)GetDlgItem(IDC_COMBOACOUNT);
	CComboBox* pComboTime = (CComboBox*)GetDlgItem(IDC_COMBOSELRECTTIME);

	if (pComboAccount)
	{
		// 실제 계좌번호 불러오기
		LoadAccountList(pComboAccount);
	}

	if (pComboTime)
	{
		// 시간외 구분 추가
		pComboTime->AddString(_T("장전시간외"));
		pComboTime->AddString(_T("장후시간외"));
		pComboTime->SetCurSel(0);
	}

	// 매수/매도 콤보박스 초기화
	CComboBox* pComboBuySell = (CComboBox*)GetDlgItem(IDC_COMBYNSELL);
	if (pComboBuySell)
	{
		pComboBuySell->ResetContent(); // 기존 항목들 모두 삭제
		pComboBuySell->AddString(_T("매수"));
		pComboBuySell->AddString(_T("매도"));
		pComboBuySell->SetCurSel(1); // 기본값: 매수 (0번째)
	}

	// 체크박스 기본값 설정
	CButton* pCheckAuto = (CButton*)GetDlgItem(IDC_CHECKAUTO);
	if (pCheckAuto)
	{
		pCheckAuto->SetCheck(BST_UNCHECKED);
	}
	
	// 상태 메시지 초기화
	UpdateStateMessage(_T("대기중"));
	
	// AppData에서 마지막 주문 정보 불러오기
	LoadConfigFromFile();

	return TRUE;
}

//*******************************************************************/
//! Function Name : OnClose
//! Function      : 다이얼로그 닫기
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnClose()
{
	// 자동주문 중지
	StopAutoOrder();
	
	// 부모 다이얼로그에 화면 닫힘 메시지 전송
	if (m_pParent)
	{
		int nLen = m_strScrNo.GetLength();
		char* cScrNo = new char[nLen + 1];
		memset(cScrNo, 0x00, nLen + 1);
		memcpy(cScrNo, m_strScrNo, nLen);
		m_pParent->PostMessage(UM_SCRENN_CLOSE, 0U, (LPARAM)cScrNo);
	}

	CDialogEx::OnClose();
}

//*******************************************************************/
//! Function Name : OnBtnOrder
//! Function      : 주문 버튼 클릭
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnBtnOrder()
{
	// 종목코드 가져오기
	CString strJongCode;
	GetDlgItemText(IDC_EDITJONGCODE, strJongCode);

	if (strJongCode.IsEmpty())
	{
		UpdateStateMessage(_T("종목코드를 입력하세요."));
		return;
	}

	// 계좌 정보 가져오기
	CComboBox* pComboAccount = (CComboBox*)GetDlgItem(IDC_COMBOACOUNT);
	CString strAccount;
	if (pComboAccount)
	{
		int nSel = pComboAccount->GetCurSel();
		if (nSel != CB_ERR)
		{
			pComboAccount->GetLBText(nSel, strAccount);
		}
	}

	// 수동 주문도 SendTimeOffOrder 호출
	SendTimeOffOrder();
}

//*******************************************************************/
//! Function Name : OnBnClickedCheckAuto
//! Function      : 자동주문 체크박스 클릭
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnBnClickedCheckAuto()
{
	CButton* pCheckAuto = (CButton*)GetDlgItem(IDC_CHECKAUTO);
	if (pCheckAuto)
	{
		int nCheck = pCheckAuto->GetCheck();
		if (nCheck == BST_CHECKED)
		{
			StartAutoOrder();
			UpdateStateMessage(_T("자동주문이 활성화되었습니다."));
		}
		else
		{
			StopAutoOrder();
			UpdateStateMessage(_T("자동주문이 비활성화되었습니다."));
		}
	}
}

//*******************************************************************/
//! Function Name : LoadAccountList
//! Function      : 계좌번호 목록 불러오기
//! Param         : CComboBox* pComboAccount
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::LoadAccountList(CComboBox* pComboAccount)
{
	if (!pComboAccount) return;
	
	pComboAccount->ResetContent();
	
	// 키움 OpenAPI에서 계좌 목록 가져오기
	CString strAccountList = theApp.m_khOpenApi.GetLoginInfo(_T("ACCLIST"));
	
	if (strAccountList.IsEmpty()) return;
	
	// ';'로 구분된 계좌번호들을 분리하여 콤보박스에 추가
	int nStart = 0;
	CString strAccount;
	
	while (nStart < strAccountList.GetLength())
	{
		int nEnd = strAccountList.Find(_T(';'), nStart);
		
		if (nEnd == -1)
		{
			strAccount = strAccountList.Mid(nStart);
		}
		else
		{
			strAccount = strAccountList.Mid(nStart, nEnd - nStart);
			nStart = nEnd + 1;
		}
		
		strAccount.Trim();
		if (!strAccount.IsEmpty())
		{
			pComboAccount->AddString(strAccount);
		}
		
		if (nEnd == -1) break;
	}
	
	// 첫 번째 계좌 선택
	if (pComboAccount->GetCount() > 0)
	{
		pComboAccount->SetCurSel(0);
	}
}

//*******************************************************************/
//! Function Name : UpdateStateMessage
//! Function      : 상태 메시지 업데이트
//! Param         : const CString& strMessage
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::UpdateStateMessage(const CString& strMessage)
{
	SetDlgItemText(IDC_STATE, strMessage);
}

//*******************************************************************/
//! Function Name : StartAutoOrder
//! Function      : 자동주문 시작
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::StartAutoOrder()
{
	if (m_bAutoOrderActive) return;
	
	m_bAutoOrderActive = TRUE;
	m_nOrderAttempts = 0;
	m_dwLastOrderTime = 0;
	
	// 즉시 첫 번째 주문 시도 (맨 앞 줄 서기 시작!)
	UpdateStateMessage(_T("맨 앞 줄 서기 시작! 접수될 때까지 계속 시도합니다."));
	SendTimeOffOrder();
	
	// 접수될 때까지 계속 시도 (1초마다 체크)
	m_nTimerID = SetTimer(1, 1000, NULL);
}

//*******************************************************************/
//! Function Name : StopAutoOrder
//! Function      : 자동주문 중지
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::StopAutoOrder()
{
	if (!m_bAutoOrderActive) return;
	
	m_bAutoOrderActive = FALSE;
	
	// 타이머 종료
	if (m_nTimerID != 0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
	}
	
	UpdateStateMessage(_T("자동주문이 중지되었습니다."));
}

//*******************************************************************/
//! Function Name : OnTimer
//! Function      : 타이머 이벤트 처리
//! Param         : UINT_PTR nIDEvent
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerID && m_bAutoOrderActive)
	{
		ProcessAutoOrder();
	}
	
	CDialogEx::OnTimer(nIDEvent);
}

//*******************************************************************/
//! Function Name : ProcessAutoOrder
//! Function      : 자동주문 처리
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::ProcessAutoOrder()
{
	// 이미 주문 처리 중이면 타임아웃 체크
	if (m_bOrderProcessing)
	{
		DWORD dwCurrentTime = GetTickCount();
		// 200ms 이상 응답 없으면 타임아웃 처리 (테스트용 초고속)
		if ((dwCurrentTime - m_dwLastOrderTime) > 200)
		{
			CString strTimeout;
			strTimeout.Format(_T("[%d회] 응답 타임아웃, 재시도"), m_nOrderAttempts);
			UpdateStateMessage(strTimeout);
			m_bOrderProcessing = FALSE; // 타임아웃시 플래그 해제
		}
		else
		{
			UpdateStateMessage(_T("주문 응답 대기 중..."));
			return;
		}
	}
	
	// API 호출 제한 체크
	if (!CanSendOrder())
	{
		UpdateStateMessage(_T("API 호출 제한으로 대기 중..."));
		return;
	}
	
	// 지속적 주문 시도 (맨 앞 줄 서기 위해)
	CString strMsg;
	SYSTEMTIME st;
	GetLocalTime(&st);
	strMsg.Format(_T("[%02d:%02d:%02d] 맨 앞 줄 서기 시도 (%d번째)"), 
				  st.wHour, st.wMinute, st.wSecond, ++m_nOrderAttempts);
	UpdateStateMessage(strMsg);
	
	SendTimeOffOrder();
}

//*******************************************************************/
//! Function Name : IsOrderTime
//! Function      : 주문 시간 확인
//! Param         : void
//! Return        : BOOL
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
BOOL CTimeOffOrderDlg::IsOrderTime()
{
	// 체크박스가 체크된 순간부터 바로 주문 시도
	return TRUE;
}

//*******************************************************************/
//! Function Name : CanSendOrder
//! Function      : API 호출 제한 체크
//! Param         : void
//! Return        : BOOL
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
BOOL CTimeOffOrderDlg::CanSendOrder()
{
	DWORD dwCurrentTime = GetTickCount();
	
	// 키움 제한인 200ms 간격으로 API 호출
	if (m_dwLastOrderTime != 0 && (dwCurrentTime - m_dwLastOrderTime) < 200)
	{
		return FALSE;
	}
	
	return TRUE;
}

//*******************************************************************/
//! Function Name : SendTimeOffOrder
//! Function      : 시간외주문 전송
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::SendTimeOffOrder()
{
	// 종목코드 가져오기
	CString strJongCode;
	GetDlgItemText(IDC_EDITJONGCODE, strJongCode);
	
	if (strJongCode.IsEmpty())
	{
		UpdateStateMessage(_T("종목코드를 입력하세요."));
		return;
	}
	
	// 계좌 정보 가져오기
	CComboBox* pComboAccount = (CComboBox*)GetDlgItem(IDC_COMBOACOUNT);
	CString strAccount;
	if (pComboAccount)
	{
		int nSel = pComboAccount->GetCurSel();
		if (nSel != CB_ERR)
		{
			pComboAccount->GetLBText(nSel, strAccount);
		}
	}
	
	// 시간외 구분 가져오기
	CComboBox* pComboTime = (CComboBox*)GetDlgItem(IDC_COMBOSELRECTTIME);
	CString strTimeType;
	if (pComboTime)
	{
		int nSel = pComboTime->GetCurSel();
		if (nSel != CB_ERR)
		{
			pComboTime->GetLBText(nSel, strTimeType);
		}
	}
	
	// 수량 입력 필드에서 가져오기
	CString strQty;
	GetDlgItemText(IDC_EDITNUMBER, strQty);
	if (strQty.IsEmpty())
	{
		strQty = _T("1"); // 기본값 1주
	}
	
	// 매수/매도 선택 가져오기
	CComboBox* pComboBuySell = (CComboBox*)GetDlgItem(IDC_COMBYNSELL);
	int nBuySellType = 1; // 기본값: 매수
	if (pComboBuySell)
	{
		int nSel = pComboBuySell->GetCurSel();
		if (nSel == 1) 
			nBuySellType = 1; // 매수 (1번째 항목)
		else 
			nBuySellType = 2; // 매도 (0번째 항목)
	}
	
	CString strPrice = _T("0"); // 시간외주문은 0으로 설정 (장전=전일종가, 장후=당일종가)
	
	// 시간외 주문 구분 코드 결정
	CString strOrderType;
	if (strTimeType == _T("장전시간외"))
	{
		strOrderType = _T("61"); // 장전시간외
	}
	else
	{
		strOrderType = _T("81"); // 장후시간외
	}
	
	try
	{
		// 디버깅: 실제 전송되는 값 확인
		CString strDebugMsg;
		strDebugMsg.Format(_T("주문타입: %d (1=매수, 2=매도)"), nBuySellType);
		UpdateStateMessage(strDebugMsg);
		
		// 마지막 주문 정보 실시간 저장
		m_strLastOrderCode = strJongCode;
		m_nLastOrderQty = _ttoi(strQty);
		
		// 저장 디버깅 메시지
		CString strSaveDebug;
		strSaveDebug.Format(_T("저장됨: %s, %d주"), m_strLastOrderCode, m_nLastOrderQty);
		UpdateStateMessage(strSaveDebug);
		
		// 주문 처리 중 플래그 설정
		m_bOrderProcessing = TRUE;
		m_strCurrentOrderNo = _T(""); // 주문번호는 응답에서 받음
		
		// SendOrder API 호출
		long lRet = theApp.m_khOpenApi.SendOrder(
			_T("시간외주문"),     // sRQName
			m_strScrNo,          // sScreenNo - 할당받은 화면번호 사용
			strAccount,          // sAccNo
			nBuySellType,        // nOrderType (1: 매수, 2: 매도)
			strJongCode,         // sCode
			_ttoi(strQty),       // nQty
			_ttoi(strPrice),     // nPrice
			strOrderType,        // sHogaGb (시간외 주문 구분)
			_T("")              // sOrgOrderNo
		);
		
		m_dwLastOrderTime = GetTickCount();
		
		CString strMsg;
		if (lRet == 0)
		{
			strMsg.Format(_T("[%d회] 주문 전송중... (%s, %s주)"), m_nOrderAttempts, strJongCode, strQty);
			UpdateStateMessage(strMsg);
			// OnReceiveMsgKhopenapictrl에서 접수 여부 확인 대기
		}
		else
		{
			m_bOrderProcessing = FALSE; // 실패시 플래그 해제
			strMsg.Format(_T("[%d회] 전송 실패 (에러:%ld), 재시도 예정"), m_nOrderAttempts, lRet);
			UpdateStateMessage(strMsg);
			
			// API 레벨 에러도 타이머에서 재시도
			// 단, 심각한 에러는 중지 (예: 계좌 오류, 권한 오류 등)
			if (lRet == -100 || lRet == -200) // 심각한 에러 예시
			{
				StopAutoOrder();
				CButton* pCheckAuto = (CButton*)GetDlgItem(IDC_CHECKAUTO);
				if (pCheckAuto)
				{
					pCheckAuto->SetCheck(BST_UNCHECKED);
				}
			}
			// 기타 에러는 계속 재시도 (네트워크 오류, 일시적 오류 등)
		}
	}
	catch (...)
	{
		m_bOrderProcessing = FALSE; // 예외 발생시 플래그 해제
		UpdateStateMessage(_T("주문 전송 중 오류 발생"));
	}
}

//*******************************************************************/
//! Function Name : OnOrderComplete
//! Function      : 주문 완료 처리
//! Param         : const CString& strMsg
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 외부에서 호출되어 자동주문을 중지시킴
//******************************************************************/
void CTimeOffOrderDlg::OnOrderComplete(const CString& strMsg)
{
	if (m_bAutoOrderActive)
	{
		UpdateStateMessage(strMsg);
		
		// 자동주문 중지
		StopAutoOrder();
		
		// 체크박스 해제
		CButton* pCheckAuto = (CButton*)GetDlgItem(IDC_CHECKAUTO);
		if (pCheckAuto)
		{
			pCheckAuto->SetCheck(BST_UNCHECKED);
		}
	}
}

//*******************************************************************/
//! Function Name : OnCbnSelchangeComboacount
//! Function      : 계좌 콤보박스 선택 변경
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnCbnSelchangeComboacount()
{
	// 계좌 선택 변경 시 처리
	CComboBox* pComboAccount = (CComboBox*)GetDlgItem(IDC_COMBOACOUNT);
	if (pComboAccount)
	{
		int nSel = pComboAccount->GetCurSel();
		if (nSel != CB_ERR)
		{
			CString strAccount;
			pComboAccount->GetLBText(nSel, strAccount);
			CString strMsg;
			strMsg.Format(_T("계좌 선택: %s"), strAccount);
			UpdateStateMessage(strMsg);
			
			// 계좌 선택 변경시 즉시 저장
			SaveConfigToFile();
		}
	}
}

//*******************************************************************/
//! Function Name : OnCbnSelchangeComboselrecttime
//! Function      : 시간 콤보박스 선택 변경
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnCbnSelchangeComboselrecttime()
{
	// 시간외 구분 선택 변경 시 처리
	CComboBox* pComboTime = (CComboBox*)GetDlgItem(IDC_COMBOSELRECTTIME);
	if (pComboTime)
	{
		int nSel = pComboTime->GetCurSel();
		if (nSel != CB_ERR)
		{
			CString strTimeType;
			pComboTime->GetLBText(nSel, strTimeType);
			
			// 목표 시간 업데이트
			if (strTimeType == _T("장전시간외"))
			{
				m_nTargetHour = 8;
				m_nTargetMinute = 30;
			}
			else if (strTimeType == _T("장후시간외"))
			{
				m_nTargetHour = 15;
				m_nTargetMinute = 30;
			}
			
			CString strMsg;
			strMsg.Format(_T("시간외 구분 선택: %s"), strTimeType);
			UpdateStateMessage(strMsg);
			
			// 시간외 구분 변경시 즉시 저장
			SaveConfigToFile();
		}
	}
}

void CTimeOffOrderDlg::OnCbnSelchangeCombynsell()
{
	// 매수/매도 선택 변경시 즉시 저장
	SaveConfigToFile();
}

//*******************************************************************/
//! Function Name : OnReceiveMsgKhopenapictrl
//! Function      : 키움 API 메시지 수신 처리
//! Param         : sScrNo, sRQName, sTrCode, sMsg
//! Return        : void
//! Create        : , 2025/01/15
//! Comment       : 주문 관련 메시지 처리
//******************************************************************/
void CTimeOffOrderDlg::OnReceiveMsgKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
	CString strScrNo(sScrNo);
	CString strRQName(sRQName);
	CString strMessage(sMsg);
	
	// 현재 화면번호의 주문 관련 메시지인지 확인
	if (strScrNo == m_strScrNo && strRQName == _T("시간외주문"))
	{
		if (m_bOrderProcessing)
		{
			m_bOrderProcessing = FALSE;
			
			// 성공 케이스만 명확히 확인 (나머지는 모두 실패로 처리)
			BOOL bOrderSuccess = FALSE;
			
			// 1. 메시지가 비어있는 경우 (정상 접수시 메시지 없음)
			if (strMessage.IsEmpty())
			{
				bOrderSuccess = TRUE;
			}
			// 2. 성공 관련 키워드 확인
			else if (strMessage.Find(_T("정상")) != -1 || 
					 strMessage.Find(_T("접수")) != -1 ||
					 strMessage.Find(_T("성공")) != -1 ||
					 strMessage.Find(_T("완료")) != -1)
			{
				bOrderSuccess = TRUE;
			}
			
			if (bOrderSuccess)
			{
				// 주문 접수 성공 - 목표 달성! 자동주문 중지
				UpdateStateMessage(_T("주문 접수 완료! 맨 앞 줄 확보!"));
				
				// 성공시에만 자동주문 중지
				if (m_bAutoOrderActive)
				{
					StopAutoOrder();
					CButton* pCheckAuto = (CButton*)GetDlgItem(IDC_CHECKAUTO);
					if (pCheckAuto)
					{
						pCheckAuto->SetCheck(BST_UNCHECKED);
					}
				}
			}
			else
			{
				// 접수 실패 - 계속 시도해야 함 (맨 앞 줄 서기 위해)
				CString strError;
				strError.Format(_T("[%d회] 접수 실패, 재시도 중: %s"), m_nOrderAttempts, strMessage);
				UpdateStateMessage(strError);
				
				// 실패해도 자동주문은 계속 진행 (타이머가 재시도)
				// 단, 사용자가 명시적으로 중지하기 전까지 계속 시도
				
				// 주문 처리 플래그만 해제 (다음 타이머에서 재시도 가능하게)
				m_bOrderProcessing = FALSE;
			}
		}
	}
}

//*******************************************************************/
//! Function Name : OnMarketClose
//! Function      : 장종료 처리 (빈 함수)
//! Param         : const CString& strMsg
//! Return        : void
//! Create        : , 2025/01/14
//! Comment       : 
//******************************************************************/

void CTimeOffOrderDlg::OnMarketClose(const CString& strMsg)
{
	if (m_bAutoOrderActive)
	{
		UpdateStateMessage(strMsg);
		
		// 자동주문 중지
		StopAutoOrder();
		
		// 체크박스 해제
		CButton* pCheckAuto = (CButton*)GetDlgItem(IDC_CHECKAUTO);
		if (pCheckAuto)
		{
			pCheckAuto->SetCheck(BST_UNCHECKED);
		}
	}
	else
	{
		// 자동주문이 활성화되지 않은 경우에도 메시지 표시
		UpdateStateMessage(strMsg);
	}
}

//*******************************************************************/
//! Function Name : GetConfigFilePath
//! Function      : AppData 폴더의 설정 파일 경로 반환
//! Param         : void
//! Return        : CString
//! Create        : , 2025/01/15
//! Comment       : 
//******************************************************************/
CString CTimeOffOrderDlg::GetConfigFilePath()
{
	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
	{
		CString strAppPath(szPath);
		strAppPath += _T("\\KhOpenApiTest");
		
		// 폴더가 없으면 생성
		CreateDirectory(strAppPath, NULL);
		
		return strAppPath + _T("\\timeoff_order.ini");
	}
	
	// 실패시 실행파일 폴더 사용
	TCHAR szExePath[MAX_PATH];
	GetModuleFileName(NULL, szExePath, MAX_PATH);
	CString strExePath(szExePath);
	int nPos = strExePath.ReverseFind(_T('\\'));
	return strExePath.Left(nPos) + _T("\\timeoff_order.ini");
}

//*******************************************************************/
//! Function Name : SaveConfigToFile
//! Function      : 현재 설정을 파일에 저장
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/15
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::SaveConfigToFile()
{
	CString strConfigPath = GetConfigFilePath();
	
	// 종목코드 저장
	CString strCurrentCode;
	GetDlgItemText(IDC_EDITJONGCODE, strCurrentCode);
	WritePrivateProfileString(_T("LastOrder"), _T("Code"), 
							 strCurrentCode, strConfigPath);
	
	// 수량 저장
	CString strCurrentQty;
	GetDlgItemText(IDC_EDITNUMBER, strCurrentQty);
	WritePrivateProfileString(_T("LastOrder"), _T("Quantity"), 
							 strCurrentQty, strConfigPath);
	
	// 매수/매도 저장
	CComboBox* pComboBuySell = (CComboBox*)GetDlgItem(IDC_COMBYNSELL);
	if (pComboBuySell)
	{
		int nSel = pComboBuySell->GetCurSel();
		CString strBuySell;
		strBuySell.Format(_T("%d"), nSel);
		WritePrivateProfileString(_T("LastOrder"), _T("BuySellType"), 
								 strBuySell, strConfigPath);
	}
	
	// 시간외 구분 저장
	CComboBox* pComboTime = (CComboBox*)GetDlgItem(IDC_COMBOSELRECTTIME);
	if (pComboTime)
	{
		int nSel = pComboTime->GetCurSel();
		CString strTimeType;
		strTimeType.Format(_T("%d"), nSel);
		WritePrivateProfileString(_T("LastOrder"), _T("TimeType"), 
								 strTimeType, strConfigPath);
	}
	
	// 계좌 선택 저장
	CComboBox* pComboAccount = (CComboBox*)GetDlgItem(IDC_COMBOACOUNT);
	if (pComboAccount)
	{
		int nSel = pComboAccount->GetCurSel();
		CString strAccountSel;
		strAccountSel.Format(_T("%d"), nSel);
		WritePrivateProfileString(_T("LastOrder"), _T("AccountIndex"), 
								 strAccountSel, strConfigPath);
	}
}

//*******************************************************************/
//! Function Name : LoadConfigFromFile
//! Function      : 파일에서 설정을 불러와 UI에 적용
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/15
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::LoadConfigFromFile()
{
	CString strConfigPath = GetConfigFilePath();
	TCHAR buffer[256];
	
	// 종목코드 불러오기
	GetPrivateProfileString(_T("LastOrder"), _T("Code"), 
						   _T(""), buffer, 256, strConfigPath);
	CString strLastCode(buffer);
	
	// 수량 불러오기
	GetPrivateProfileString(_T("LastOrder"), _T("Quantity"), 
						   _T("1"), buffer, 256, strConfigPath);
	CString strLastQty(buffer);
	
	// 매수/매도 불러오기
	int nBuySellType = GetPrivateProfileInt(_T("LastOrder"), _T("BuySellType"), 
										   0, strConfigPath);
	
	// 시간외 구분 불러오기
	int nTimeType = GetPrivateProfileInt(_T("LastOrder"), _T("TimeType"), 
										0, strConfigPath);
	
	// 계좌 선택 불러오기
	int nAccountIndex = GetPrivateProfileInt(_T("LastOrder"), _T("AccountIndex"), 
											0, strConfigPath);
	
	// UI에 설정
	if (!strLastCode.IsEmpty())
	{
		SetDlgItemText(IDC_EDITJONGCODE, strLastCode);
		m_strLastOrderCode = strLastCode;
		UpdateStateMessage(_T("종목코드 복원완료"));
	}
	
	if (!strLastQty.IsEmpty() && _ttoi(strLastQty) > 0)
	{
		SetDlgItemText(IDC_EDITNUMBER, strLastQty);
		m_nLastOrderQty = _ttoi(strLastQty);
		UpdateStateMessage(_T("수량 복원완료"));
	}
	
	// 콤보박스 설정 (콤보박스가 이미 초기화된 후에 설정)
	CComboBox* pComboBuySell = (CComboBox*)GetDlgItem(IDC_COMBYNSELL);
	if (pComboBuySell && nBuySellType >= 0 && nBuySellType < pComboBuySell->GetCount())
	{
		pComboBuySell->SetCurSel(nBuySellType);
	}
	
	CComboBox* pComboTime = (CComboBox*)GetDlgItem(IDC_COMBOSELRECTTIME);
	if (pComboTime && nTimeType >= 0 && nTimeType < pComboTime->GetCount())
	{
		pComboTime->SetCurSel(nTimeType);
	}
	
	CComboBox* pComboAccount = (CComboBox*)GetDlgItem(IDC_COMBOACOUNT);
	if (pComboAccount && nAccountIndex >= 0 && nAccountIndex < pComboAccount->GetCount())
	{
		pComboAccount->SetCurSel(nAccountIndex);
	}
}

//*******************************************************************/
//! Function Name : OnEnChangeEditjongcode
//! Function      : 종목코드 입력 변경시 즉시 저장
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/15
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnEnChangeEditjongcode()
{
	SaveConfigToFile();
}

//*******************************************************************/
//! Function Name : OnEnChangeEditnumber
//! Function      : 수량 입력 변경시 즉시 저장
//! Param         : void
//! Return        : void
//! Create        : , 2025/01/15
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnEnChangeEditnumber()
{
	SaveConfigToFile();
}

//*******************************************************************/
//! Function Name : OnOrderFailed
//! Function      : 주문 실패 처리
//! Param         : const CString& strMsg
//! Return        : void
//! Create        : , 2025/01/15
//! Comment       : 
//******************************************************************/
void CTimeOffOrderDlg::OnOrderFailed(const CString& strMsg)
{
	m_bOrderProcessing = FALSE;
	UpdateStateMessage(strMsg);
	
	// 자동주문 중지
	if (m_bAutoOrderActive)
	{
		StopAutoOrder();
		CButton* pCheckAuto = (CButton*)GetDlgItem(IDC_CHECKAUTO);
		if (pCheckAuto)
		{
			pCheckAuto->SetCheck(BST_UNCHECKED);
		}
	}
}

//*******************************************************************/
//! Function Name : OnOrderResult
//! Function      : 주문 결과 처리 (체결 정보)
//! Param         : const CString& strOrderNo, strOrderStatus, strChejanQty
//! Return        : void
//! Create        : , 2025/01/15
//! Comment       : 체결 데이터 수신시 호출 (참고용)
//******************************************************************/
void CTimeOffOrderDlg::OnOrderResult(const CString& strOrderNo, const CString& strOrderStatus, const CString& strChejanQty)
{
	// 주문 접수만 확인하므로 체결 정보는 참고용으로만 사용
	CString strMsg;
	strMsg.Format(_T("주문번호: %s, 상태: %s"), strOrderNo, strOrderStatus);
	
	// 로그성 정보로만 사용
	if (!strOrderNo.IsEmpty())
	{
		m_strCurrentOrderNo = strOrderNo;
	}
}
