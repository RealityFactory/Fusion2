// Export3dsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fusion.h"
#include "Export3dsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExport3dsDialog dialog


CExport3dsDialog::CExport3dsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CExport3dsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExport3dsDialog)
	m_ExportAll = 0;
	m_ExportLights = FALSE;
	m_GroupFile = FALSE;
	//}}AFX_DATA_INIT
}


void CExport3dsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExport3dsDialog)
	DDX_Radio(pDX, IDC_EXPORTALL, m_ExportAll);
	DDX_Check(pDX, IDC_EXPORTLIGHTS, m_ExportLights);
	DDX_Check(pDX, IDC_GROUPFILE, m_GroupFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExport3dsDialog, CDialog)
	//{{AFX_MSG_MAP(CExport3dsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExport3dsDialog message handlers
