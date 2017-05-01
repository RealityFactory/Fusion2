// RotateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fusion.h"
#include "RotateDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <assert.h>
#include "units.h"

/////////////////////////////////////////////////////////////////////////////
// CRotateDialog dialog


CRotateDialog::CRotateDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRotateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRotateDialog)
	m_DegreesType = 6;
	m_Direction = 0;
	m_FreeDegrees = 90.0f;
	//}}AFX_DATA_INIT
}


void CRotateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotateDialog)
	DDX_Radio(pDX, IDC_DEGREES, m_DegreesType);
	DDX_Radio(pDX, IDC_DIRECTION, m_Direction);
	DDX_Text(pDX, IDC_EDITFREE, m_FreeDegrees);
	DDV_MinMaxFloat(pDX, m_FreeDegrees, 0.f, 360.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRotateDialog, CDialog)
	//{{AFX_MSG_MAP(CRotateDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotateDialog message handlers

void CRotateDialog::OnOK() 
{
	if (!UpdateData(TRUE))
		return;

	geFloat Multiplier = 0;
	switch (m_Direction)
	{
		case 0:
			Multiplier = -1;
			break;

		case 1:
			Multiplier = 1;
			break;

		default:
			assert(0);
	}
	
	geFloat Radians = 0;
	switch (m_DegreesType)
	{
		case 0:
			Radians = (M_PI / 6);
			break;

		case 1:
			Radians = (M_PI / 4);
			break;

		case 2:
			Radians = (M_PI / 3);
			break;

		case 3:
			Radians = (M_PI / 2);
			break;

		case 4:
			Radians = M_PI;
			break;

		case 5:
			Radians = (3 * M_PI) / 2;
			break;

		case 6:
			Radians = Units_DegreesToRadians(m_FreeDegrees);
			break;

		default:
			assert(0);
	}

	(*FinalRotation) = Multiplier * Radians;

	CDialog::OnOK();
}

int CRotateDialog::DoModal(geFloat *RotateRadians) 
{
	FinalRotation = RotateRadians;

	return CDialog::DoModal();
}
