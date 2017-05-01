// DialogPrintView.cpp : implementation file
//

#include "stdafx.h"
#include "GEditPro.h"
#include "DialogPrintView.h"
#include "Globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDialogPrintView dialog


CDialogPrintView::CDialogPrintView(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPrintView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogPrintView)
	m_btnTopView = FALSE;
	m_btnFrontView = FALSE;
	m_btnSideView = FALSE;
	m_btnTexturedView = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogPrintView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPrintView)
	DDX_Check(pDX, IDC_BTN_PRINT_TOP, m_btnTopView);
	DDX_Check(pDX, IDC_BTN_PRINT_FRONT, m_btnFrontView);
	DDX_Check(pDX, IDC_BTN_PRINT_SIDE, m_btnSideView);
	DDX_Check(pDX, IDC_BTN_PRINT_TEXTURED, m_btnTexturedView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPrintView, CDialog)
	//{{AFX_MSG_MAP(CDialogPrintView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDialogPrintView message handlers

BOOL CDialogPrintView::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDialogPrintView::OnOK()
{
	UpdateData();
/*	CWnd	*pWnd;
	CGEditProDoc *pDoc = NULL;
	pDoc = CGlobals::GetActiveDocument();

	if (pDoc)
	{

	if (this->m_btnTopView)
	{

	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView *pView = pDoc->GetNextView(pos);
		if(pView->GetParent()->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
		{
			CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, pView->GetParent());
			int row = TOP_VIEW_ROW;
			int col = TOP_VIEW_COL;
			
			VERIFY(pParent->IsChildPane(pView, row, col));
			
			pWnd = pParent->GetPane(row,col);
				//	if the active pane is not the pane for this view
			if (pParent->GetActivePane(&row, &col) != pWnd)
			{
					//	make the pane for this view the active pane
				pParent->SetActivePane(row,col, pWnd);
			}
			pView->SendMessage(WM_PRINT);
		}
	}


/*

		CWnd *pMainWnd = NULL;
		pMainWnd = AfxGetMainWnd();
		

		CView *pSplitterView = static_cast<CFrameWnd*>(pMainWnd)->GetActiveFrame()->GetActiveView();
		int row = TOP_VIEW_ROW;
		int col = TOP_VIEW_COL;
		if(!pSplitterView->IsKindOf(RUNTIME_CLASS(CSplitterWnd)))
		MessageBeep(0);
		
		
	//	CSplitterWnd *pParent = STATIC_DOWNCAST(CSplitterWnd, GetParent());
			
//			int row = TOP_VIEW_ROW;
//			int col = TOP_VIEW_COL;
			
//			VERIFY(pParent->IsChildPane(this, row, col));
			
			pWnd = pSplitterView->GetPane(row,col);
				//	if the active pane is not the pane for this view
			if (pSplitterView->GetActivePane(&row, &col) != pWnd)
			{
					//	make the pane for this view the active pane
				pSplitterView->SetActivePane(row,col, pWnd);
			}
	}

	if (this->m_btnFrontView)
	{
		MessageBeep(0);
	}

	if (this->m_btnSideView)
	{
		MessageBeep(0);
	}

	if (this->m_btnTexturedView)
	{
		MessageBeep(0);
	}

	}
	*/
//this->DestroyWindow();
//this->m_hWnd = NULL;
	CDialog::OnOK();
}
