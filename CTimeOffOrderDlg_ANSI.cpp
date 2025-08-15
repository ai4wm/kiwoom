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
	ON_BN_CLICKED(IDOK, &CTimeOffOrderDlg::OnBtnOrder)
	ON_BN_CLICKED(IDC_CHECKAUTO, &CTimeOffOrderDlg::OnBnClickedCheckAuto)
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
		// 계좌번호 샘플 데이터 추가
		pComboAccount->AddString(_T("계좌1"));
		pComboAccount->AddString(_T("계좌2"));
		pComboAccount->SetCurSel(0);
	}

	if (pComboTime)
	{
		// 시간대 샘플 데이터 추가
		pComboTime->AddString(_T("08:00"));
		pComboTime->AddString(_T("18:00"));
		pComboTime->AddString(_T("19:00"));
		pComboTime->SetCurSel(0);
	}

	// 체크박스 기본값 설정
	CButton* pCheckAuto = (CButton*)GetDlgItem(IDC_CHECKAUTO);
	if (pCheckAuto)
	{
		pCheckAuto->SetCheck(BST_UNCHECKED);
	}

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
		AfxMessageBox(_T("종목코드를 입력하세요."));
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

	// 시간외주문 로직 구현
	CString strMsg;
	strMsg.Format(_T("시간외주문이 접수되었습니다.\n종목코드: %s\n계좌: %s"), strJongCode, strAccount);
	AfxMessageBox(strMsg);
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
			AfxMessageBox(_T("자동주문이 활성화되었습니다."));
		}
		else
		{
			AfxMessageBox(_T("자동주문이 비활성화되었습니다."));
		}
	}
}