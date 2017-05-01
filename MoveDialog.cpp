// MoveDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fusion.h"
#include "MoveDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveDialog dialog


CMoveDialog::CMoveDialog(CWnd* pParent)
	: CDialog(CMoveDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMoveDialog)
	m_MoveType = 0;
	m_XFinal = 0.0f;
	m_YFinal = 0.0f;
	m_ZFinal = 0.0f;
	m_X = 0.0f;
	m_Y = 0.0f;
	m_Z = 0.0f;
	//}}AFX_DATA_INIT
}


void CMoveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveDialog)
	DDX_Radio(pDX, IDC_ABSOLUTE, m_MoveType);
	DDX_Text(pDX, IDC_FINALX, m_XFinal);
	DDX_Text(pDX, IDC_FINALY, m_YFinal);
	DDX_Text(pDX, IDC_FINALZ, m_ZFinal);
	DDX_Text(pDX, IDC_XEDITBOX, m_X);
	DDX_Text(pDX, IDC_YEDITBOX, m_Y);
	DDX_Text(pDX, IDC_ZEDITBOX, m_Z);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoveDialog, CDialog)
	//{{AFX_MSG_MAP(CMoveDialog)
	ON_EN_CHANGE(IDC_XEDITBOX, OnChangeXedit)
	ON_EN_CHANGE(IDC_YEDITBOX, OnChangeYedit)
	ON_EN_CHANGE(IDC_ZEDITBOX, OnChangeZedit)
	ON_BN_CLICKED(IDC_ABSOLUTE, OnAbsolute)
	ON_BN_CLICKED(IDC_RELATIVE, OnRelative)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveDialog message handlers

void CMoveDialog::OnOK() 
{
	UpdateXValue();
	UpdateYValue();
	UpdateZValue();

	(*Position).X = m_XFinal;
	(*Position).Y = m_YFinal;
	(*Position).Z = m_ZFinal;

	CDialog::OnOK();
}

void CMoveDialog::OnCancel() 
{
	CDialog::OnCancel();
}

void CMoveDialog::OnChangeXedit() 
{
	UpdateData(TRUE);
	UpdateXValue();
	UpdateData(FALSE);
}

void CMoveDialog::OnChangeYedit() 
{
	UpdateData(TRUE);
	UpdateYValue();
	UpdateData(FALSE);
}

void CMoveDialog::OnChangeZedit() 
{
	UpdateData(TRUE);
	UpdateZValue();
	UpdateData(FALSE);
}

void CMoveDialog::OnAbsolute() 
{
	UpdateData(TRUE);
	m_X = (*Position).X;
	m_Y = (*Position).Y;
	m_Z = (*Position).Z;
	UpdateXValue();
	UpdateYValue();
	UpdateZValue();
	UpdateData(FALSE);
}

void CMoveDialog::OnRelative() 
{
	UpdateData(TRUE);
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
	UpdateXValue();
	UpdateYValue();
	UpdateZValue();
	UpdateData(FALSE);
}

void CMoveDialog::UpdateXValue()
{

	switch (m_MoveType)
	{
		case 0 :
			m_XFinal = m_X;
			break;

		case 1 :
			m_XFinal = (*Position).X + m_X;
			break;

		default :
	    assert(0);
	}
}

void CMoveDialog::UpdateYValue()
{

	switch (m_MoveType)
	{
		case 0 :
			m_YFinal = m_Y;
			break;

		case 1 :
			m_YFinal = (*Position).Y + m_Y;
			break;

		default :
	    assert(0);
	}
}

void CMoveDialog::UpdateZValue()
{

	switch (m_MoveType)
	{
		case 0 :
			m_ZFinal = m_Z;
			break;

		case 1 :
			m_ZFinal = (*Position).Z + m_Z;
			break;

		default :
	    assert(0);
	}
}

int CMoveDialog::DoModal(geVec3d *PassedPosition, geBoolean ShowX, geBoolean ShowY, geBoolean ShowZ) 
{
	assert(Position);

	Position = PassedPosition;

	m_MoveType = 0;
	m_XFinal = (*Position).X;
	m_YFinal = (*Position).Y;
	m_ZFinal = (*Position).Z;
	m_X = (*Position).X;
	m_Y = (*Position).Y;
	m_Z = (*Position).Z;
	XVisible = ShowX;
	YVisible = ShowY;
	ZVisible = ShowZ;

	return CDialog::DoModal();
}

BOOL CMoveDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	(CEdit*)GetDlgItem(IDC_XEDITBOX)->ShowWindow(XVisible);
	(CEdit*)GetDlgItem(IDC_FINALX)->ShowWindow(XVisible);

	(CEdit*)GetDlgItem(IDC_YEDITBOX)->ShowWindow(YVisible);
	(CEdit*)GetDlgItem(IDC_FINALY)->ShowWindow(YVisible);

	(CEdit*)GetDlgItem(IDC_ZEDITBOX)->ShowWindow(ZVisible);
	(CEdit*)GetDlgItem(IDC_FINALZ)->ShowWindow(ZVisible);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
