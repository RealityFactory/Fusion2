// ViewMgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GEditPro.h"
#include "ViewMgrDlg.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewMgrDlg dialog


CViewMgrDlg::CViewMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CViewMgrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CViewMgrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CViewMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewMgrDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewMgrDlg, CDialog)
	//{{AFX_MSG_MAP(CViewMgrDlg)
	ON_BN_CLICKED(IDC_BTN_VIEW_UL, OnBtnViewUL)
	ON_BN_CLICKED(IDC_BTN_VIEW_UR, OnBtnViewUR)
	ON_BN_CLICKED(IDC_BTN_VIEW_LL, OnBtnViewLL)
	ON_BN_CLICKED(IDC_BTN_VIEW_LR, OnBtnViewLR)
	ON_BN_CLICKED(IDC_BTN_VIEW_RESTORE, OnBtnViewRestore)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CViewMgrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}






/////////////////////////////////////////////////////////////////////////////
// CViewMgrDlg message handlers



void CViewMgrDlg::OnBtnViewUL() 
{
	//	only expand if the button is ACTUALLY clicked
	//	otherwise. just having focus will activate this message
	CButton	*ptempButton = NULL;
	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_UL);
	if (ptempButton->GetCheck() == 1)
	{
		CMainFrame *pMainFrame = NULL;
		pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
		{
			CChildFrame *pChildFrame = NULL;
			
			pChildFrame = (CChildFrame*)pMainFrame->MDIGetActive();
			if (pChildFrame)
			{
				ASSERT( pChildFrame->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) );
				
				pChildFrame->MaximizeUpperLeftPane();
			}
		}
	}
}




void CViewMgrDlg::OnBtnViewUR() 
{
	//	only expand if the button is ACTUALLY clicked
	//	otherwise. just having focus will activate this message
	CButton	*ptempButton = NULL;
	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_UR);
	if (ptempButton->GetCheck() == 1)
	{
		CMainFrame *pMainFrame = NULL;
		pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
		{
			CChildFrame *pChildFrame = NULL;
			
			pChildFrame = (CChildFrame*)pMainFrame->MDIGetActive();
			if (pChildFrame)
			{
				ASSERT( pChildFrame->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) );
				
				pChildFrame->MaximizeUpperRightPane();
			}
		}
	}
}




void CViewMgrDlg::OnBtnViewLL() 
{
	//	only expand if the button is ACTUALLY clicked
	//	otherwise. just having focus will activate this message
	CButton	*ptempButton = NULL;
	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_LL);
	if (ptempButton->GetCheck() == 1)
	{
		CMainFrame *pMainFrame = NULL;
		pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
		{
			CChildFrame *pChildFrame = NULL;
			
			pChildFrame = (CChildFrame*)pMainFrame->MDIGetActive();
			if (pChildFrame)
			{
				ASSERT( pChildFrame->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) );
				
				pChildFrame->MaximizeLowerLeftPane();
			}
		}
	}
}




void CViewMgrDlg::OnBtnViewLR() 
{
	//	only expand if the button is ACTUALLY clicked
	//	otherwise. just having focus will activate this message
	CButton	*ptempButton = NULL;
	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_LR);
	if (ptempButton->GetCheck() == 1)
	{
		CMainFrame *pMainFrame = NULL;
		pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
		{
			CChildFrame *pChildFrame = NULL;
			
			pChildFrame = (CChildFrame*)pMainFrame->MDIGetActive();
			if (pChildFrame)
			{
				ASSERT( pChildFrame->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) );
				
				pChildFrame->MaximizeLowerRightPane();
			}
		}
	}
}




void CViewMgrDlg::OnBtnViewRestore() 
{
	//	only restore if the button is ACTUALLY clicked
	//	otherwise. just having focus will activate this message
	CButton	*ptempButton = NULL;
	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_RESTORE);
	if (ptempButton->GetCheck() == 1)
	{
		CMainFrame *pMainFrame = NULL;
		pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		if (pMainFrame)
		{
			CChildFrame *pChildFrame = NULL;
			
			pChildFrame = (CChildFrame*)pMainFrame->MDIGetActive();
			if (pChildFrame)
			{
				ASSERT( pChildFrame->IsKindOf( RUNTIME_CLASS( CChildFrame ) ) );
				
				pChildFrame->RestoreAllPanes();
			}
		}
	}
}



bool CViewMgrDlg::ResetButtons()
{
	CButton	*ptempButton = NULL;

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_UL);
	if (ptempButton)
	{
		ptempButton->SetCheck(0);
		ptempButton = NULL;
	}

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_UR);
	if (ptempButton)
	{
		ptempButton->SetCheck(0);
		ptempButton = NULL;
	}

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_LL);
	if (ptempButton)
	{
		ptempButton->SetCheck(0);
		ptempButton = NULL;
	}

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_LR);
	if (ptempButton)
	{
		ptempButton->SetCheck(0);
		ptempButton = NULL;
	}

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_RESTORE);
	if (ptempButton)
	{
		ptempButton->SetCheck(0);
		ptempButton = NULL;
	}

	return true;
}



bool CViewMgrDlg::EnableButtons(BOOL bWhat)
{
	CButton	*ptempButton = NULL;

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_UL);
	if (ptempButton)
	{
		ptempButton->EnableWindow(bWhat);
		ptempButton = NULL;
	}

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_UR);
	if (ptempButton)
	{
		ptempButton->EnableWindow(bWhat);
		ptempButton = NULL;
	}

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_LL);
	if (ptempButton)
	{
		ptempButton->EnableWindow(bWhat);
		ptempButton = NULL;
	}

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_LR);
	if (ptempButton)
	{
		ptempButton->EnableWindow(bWhat);
		ptempButton = NULL;
	}

	ptempButton = (CButton*)GetDlgItem(IDC_BTN_VIEW_RESTORE);
	if (ptempButton)
	{
		ptempButton->EnableWindow(bWhat);
		ptempButton = NULL;
	}

	return true;
}
