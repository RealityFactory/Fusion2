// ScaleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fusion.h"
#include "ScaleDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleDialog dialog


CScaleDialog::CScaleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CScaleDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScaleDialog)
	m_X = 0.0f;
	m_Y = 0.0f;
	m_Z = 0.0f;
	//}}AFX_DATA_INIT
}


void CScaleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScaleDialog)
	DDX_Text(pDX, IDC_XEDITBOX, m_X);
	DDX_Text(pDX, IDC_YEDITBOX, m_Y);
	DDX_Text(pDX, IDC_ZEDITBOX, m_Z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScaleDialog, CDialog)
	//{{AFX_MSG_MAP(CScaleDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleDialog message handlers

void CScaleDialog::OnOK() 
{
	if (!UpdateData(TRUE))
		return;

	if ((m_X < 0) || (m_Y < 0) || (m_Z < 0))
	{
		AfxMessageBox(IDS_SCALEFACTORZERO);
		return;
	}

	(*ScaleVector).X = m_X;
	(*ScaleVector).Y = m_Y;
	(*ScaleVector).Z = m_Z;

	CDialog::OnOK();
}

int CScaleDialog::DoModal(geVec3d *PassedScaleVector) 
{
	ScaleVector = PassedScaleVector;
	m_X = (*ScaleVector).X;
	m_Y = (*ScaleVector).Y;
	m_Z = (*ScaleVector).Z;
	
	return CDialog::DoModal();
}
