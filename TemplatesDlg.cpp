/****************************************************************************************/
/*  TemplatesDlg.cpp																	*/
/*                                                                                      */
/*  Author:       Tom Morris															*/
/*  Description:  Template selection													*/
/*                                                                                      */
/*  The contents of this file are subject to the Genesis3D Public License               */
/*  Version 1.01 (the "License"); you may not use this file except in                   */
/*  compliance with the License. You may obtain a copy of the License at                */
/*  http://www.genesis3d.com                                                            */
/*                                                                                      */
/*  Software distributed under the License is distributed on an "AS IS"                 */
/*  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See                */
/*  the License for the specific language governing rights and limitations              */
/*  under the License.                                                                  */
/*                                                                                      */
/*  The Original Code is Genesis3D, released March 25, 1999.                            */
/*	Genesis3D Version 1.1 released November 15, 1999				                    */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved							*/
/*                                                                                      */
/*  Modified by Tom Morris for GEditPro ver. 0.7, Nov. 2, 2002							*/
/****************************************************************************************/

#include "stdafx.h"
#include "Globals.h"
#include "GEditPro.h"
#include "TemplatesDlg.h"
#include "FilePath.h"
//#include "SkyDialog.h"		//	commented out post 0.5 release

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplatesDlg dialog


CTemplatesDlg::CTemplatesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplatesDlg::IDD, pParent)
	, m_strCurrentItem(_T(""))
{
	m_pDoc = NULL;
	//{{AFX_DATA_INIT(CTemplatesDlg)
	//}}AFX_DATA_INIT
}


void CTemplatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplatesDlg)
	DDX_Control(pDX, IDC_TEMPLATE_APPLY, m_templateApplyBtn);
	DDX_Control(pDX, IDC_SKYBUTTON, m_skyBtn);
	DDX_Control(pDX, IDC_ENTITY_BTN, m_entitiesButton);
	DDX_Control(pDX, IDC_ENTITIES_COMBO, m_entitiesCombo);
	DDX_Control(pDX, IDC_CURRENT_PREFAB, m_currentPrefabName);
	DDX_Control(pDX, IDC_PREFAB_LIST, m_prefabList);
	DDX_Control(pDX, IDC_BUTTON_SPHERE, m_sphere_template_btn);
	DDX_Control(pDX, IDC_BUTTON_CUBE, m_cube_template_btn);
	DDX_Control(pDX, IDC_BUTTON_CYLINDER, m_cylinder_template_btn);
	DDX_Control(pDX, IDC_BUTTON_STAIRS, m_stairs_template_btn);
	DDX_Control(pDX, IDC_BUTTON_CONE, m_cone_template_btn);
	DDX_Control(pDX, IDC_BUTTON_ARCH, m_arch_template_btn);
	DDX_Control(pDX, IDC_PREFAB_BTN, m_prefabButton);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_CURRENT_ITEM, m_strCurrentItem);
}


BEGIN_MESSAGE_MAP(CTemplatesDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplatesDlg)
	ON_BN_CLICKED(IDC_PREFAB_BTN, OnPrefabBtn)
	ON_BN_CLICKED(IDC_ENTITY_BTN, OnEntityBtn)
	ON_BN_CLICKED(IDC_SKYBUTTON, OnSkybutton)
	ON_BN_CLICKED(IDC_TEMPLATE_APPLY, OnTemplateApply)
	ON_LBN_DBLCLK(IDC_PREFAB_LIST, OnDblclkPrefabList)
	ON_BN_CLICKED(IDC_BUTTON_SPHERE, OnButtonSphere)
	ON_BN_CLICKED(IDC_BUTTON_CUBE, OnButtonCube)
	ON_BN_CLICKED(IDC_BUTTON_CYLINDER, OnButtonCylinder)
	ON_BN_CLICKED(IDC_BUTTON_STAIRS, OnButtonStairs)
	ON_BN_CLICKED(IDC_BUTTON_CONE, OnButtonCone)
	ON_BN_CLICKED(IDC_BUTTON_ARCH, OnButtonArch)
	ON_LBN_SELCHANGE(IDC_PREFAB_LIST, OnSelchangePrefabList)
	ON_CBN_SELCHANGE(IDC_ENTITIES_COMBO, OnSelchangeEntitiesCombo)
	//}}AFX_MSG_MAP
	ON_CBN_CLOSEUP(IDC_ENTITIES_COMBO, OnCbnCloseupEntitiesCombo)
END_MESSAGE_MAP()



BOOL CTemplatesDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	return CDialog::PreCreateWindow(cs);
}




/////////////////////////////////////////////////////////////////////////////
// CTemplatesDlg message handlers

BOOL CTemplatesDlg::OnInitDialog() 
{
	//	position this dialog box on the command panel tab
	CGlobals::PositionDialogUnderTabs(this);

	((CButton*)GetDlgItem(IDC_BUTTON_CUBE))->SetIcon(AfxGetApp()->LoadIcon(IDI_CUBE_ICON));

	((CButton*)GetDlgItem(IDC_BUTTON_SPHERE))->SetIcon(AfxGetApp()->LoadIcon(IDI_SPHERE_ICON));

	((CButton*)GetDlgItem(IDC_BUTTON_CYLINDER))->SetIcon(AfxGetApp()->LoadIcon(IDI_CYLINDER_ICON));

	((CButton*)GetDlgItem(IDC_BUTTON_STAIRS))->SetIcon(AfxGetApp()->LoadIcon(IDI_STAIRS_ICON));

	((CButton*)GetDlgItem(IDC_BUTTON_CONE))->SetIcon(AfxGetApp()->LoadIcon(IDI_CONE_ICON));

	((CButton*)GetDlgItem(IDC_BUTTON_ARCH))->SetIcon(AfxGetApp()->LoadIcon(IDI_ARCH_ICON));

	//	initially set the cube template button
	((CButton*)GetDlgItem(IDC_BUTTON_CUBE))->SetCheck(1);	
	m_strCurrentItem.LoadString(IDS_CUBE);

	//	put an icon on the sky button	
	((CButton*)GetDlgItem(IDC_SKYBUTTON))->SetIcon(AfxGetApp()->LoadIcon(IDI_SKY_ICON));

	m_bEntitiesInitialized = false;
	m_bPrefabsInitialized = false;

	UpdateData(FALSE);

	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



bool CTemplatesDlg::InitializeEntitiesCombo(CGEditProDoc* pDoc)
{
	m_entitiesCombo.ResetContent();

	// here we need to add the entity types from the CDialog info to the combo box...
	EntityTypeList *pList = NULL;
	const EntityTable	*pEntityTable = NULL;
	CtLevel *pLevel = NULL;
	pLevel = pDoc->GetLevel();
	if (pLevel)
	{
		pEntityTable = pDoc->DocVarsGetPointer()->m_Level.m_pEntityDefs;
//        pEntityTable = pDoc->m_pLevelMgr->GetEntityDefs (pLevel);
		if (pEntityTable)
		{

			pList = EntityTable_GetAvailableEntityTypes(pEntityTable);
			if (pList != NULL)
			{
				for (int i = 0; i < pList->nTypes; i++)
				{
					if( strcmp( pList->TypeNames[i], "Camera" ) )	// Exclude Camera entity

					m_entitiesCombo.AddString(pList->TypeNames[i]);
				}
				EntityTable_ReleaseEntityTypes (pList);
			}
		}
		}

	if (m_iCurrentEntity >0)
		m_entitiesCombo.SetCurSel(m_iCurrentEntity);
	else
		m_entitiesCombo.SetCurSel(0);

		m_bEntitiesInitialized = true;

	return true;
}



bool	CTemplatesDlg::InitializePrefabList(CGEditProDoc* pDoc)
{
	char			ObjectsDir[MAX_PATH];
	const char		*pObjDir = NULL;
	CString			strObjectsDir;
	BOOL			FileWasFound;
	char			Name[MAX_PATH];

	WIN32_FIND_DATA FindData;
	HANDLE			FindHandle;
	const Prefs		*pPrefs = pDoc->GetPrefs();

	// get Objects directory from INI
	pObjDir = Prefs_GetObjectsDir (pPrefs);
	::FilePath_AppendName (pObjDir, "*.3dt", ObjectsDir);

	m_prefabList.ResetContent();
	FindHandle = ::FindFirstFile (ObjectsDir, &FindData);

	FileWasFound = (FindHandle != INVALID_HANDLE_VALUE);

	while (FileWasFound)
	{
		::FilePath_GetName (FindData.cFileName, Name);
		m_prefabList.AddString (Name);
		FileWasFound = ::FindNextFile (FindHandle, &FindData);
	}

	::FindClose (FindHandle);
					//	keep the previously-selected item after this Update()
	if (m_iCurrentPrefab > 0)
		m_prefabList.SetCurSel (m_iCurrentPrefab);
	else
		if	(m_prefabList.SetCurSel (0) == CB_ERR)
	{
		// couldn't set it...probably nothing in there
//		EnablePlaceObjectButton (FALSE);
	}
	
	m_bPrefabsInitialized = true;
	return true;
}




bool CTemplatesDlg::UpdateTemplates( CGEditProDoc* pDoc )
{

	m_pDoc = pDoc;

//	if (!m_bEntitiesInitialized)
//	{
		if (!InitializeEntitiesCombo(pDoc))
		{
			AfxMessageBox("Update Failed to initialize Entities Combo");
		}
//	}
	
	if (!m_bPrefabsInitialized)
	{
		if (!InitializePrefabList(pDoc))
		{
			AfxMessageBox("Update Failed to initialize Prefab list");
		}
	}

	if(m_pDoc->m_iModeTool==ID_TOOLS_TEMPLATE)
	{
		//enable all
		m_cube_template_btn.EnableWindow(TRUE);
		m_sphere_template_btn.EnableWindow(TRUE);
		m_cylinder_template_btn.EnableWindow(TRUE);
		m_stairs_template_btn.EnableWindow(TRUE);
		m_arch_template_btn.EnableWindow(TRUE);
		m_cone_template_btn.EnableWindow(TRUE);
		m_prefabButton.EnableWindow(TRUE);
		m_prefabList.EnableWindow(TRUE);
		m_currentPrefabName.EnableWindow(TRUE);
		m_entitiesButton.EnableWindow (TRUE);
		m_entitiesCombo.EnableWindow(TRUE);
		m_templateApplyBtn.EnableWindow(TRUE);
		m_skyBtn.EnableWindow(TRUE);
		// enable the object placement button
//		EnablePlaceObjectButton( TRUE );
	} 
	else 
	{
		//grey all
		m_cube_template_btn.EnableWindow(FALSE);
		m_sphere_template_btn.EnableWindow(FALSE);
		m_cylinder_template_btn.EnableWindow(FALSE);
		m_stairs_template_btn.EnableWindow(FALSE);
		m_arch_template_btn.EnableWindow(FALSE);
		m_cone_template_btn.EnableWindow(FALSE);
		m_prefabButton.EnableWindow(FALSE);
		m_prefabList.EnableWindow(FALSE);
		m_currentPrefabName.EnableWindow(FALSE);
		m_entitiesButton.EnableWindow (FALSE);
		m_entitiesCombo.EnableWindow(FALSE);
		m_templateApplyBtn.EnableWindow(FALSE);
		m_skyBtn.EnableWindow(FALSE);

		// disable object placement button
//		m_PlaceObjectButton.EnableWindow( FALSE );
	}

	//	Update the template buttons after switching between open documents
	bool	bSearch = true;
	
	while (bSearch)
	{
		if (pDoc->m_currentTemplateName == _T("Arch") && m_arch_template_btn.GetCheck() == 0)
		{
			m_arch_template_btn.SetCheck(1);
			m_strCurrentItem.LoadString(IDS_ARCH);
			m_cube_template_btn.SetCheck(0);
			m_cone_template_btn.SetCheck(0);
			m_cylinder_template_btn.SetCheck(0);
			m_sphere_template_btn.SetCheck(0);
			m_stairs_template_btn.SetCheck(0);
			bSearch = false;	//	break out of loop
		}
		
		if (pDoc->m_currentTemplateName == _T("Cube") && m_cube_template_btn.GetCheck() == 0)
		{
			m_arch_template_btn.SetCheck(0);
			m_cube_template_btn.SetCheck(1);
			m_strCurrentItem.LoadString(IDS_CUBE);
			m_cone_template_btn.SetCheck(0);
			m_cylinder_template_btn.SetCheck(0);
			m_sphere_template_btn.SetCheck(0);
			m_stairs_template_btn.SetCheck(0);
			bSearch = false;
		}
		if (pDoc->m_currentTemplateName == _T("Cone") && m_cone_template_btn.GetCheck() == 0)
		{
			m_arch_template_btn.SetCheck(0);
			m_cube_template_btn.SetCheck(0);
			m_cone_template_btn.SetCheck(1);
			m_strCurrentItem.LoadString(IDS_CONE);
			m_cylinder_template_btn.SetCheck(0);
			m_sphere_template_btn.SetCheck(0);
			m_stairs_template_btn.SetCheck(0);
			bSearch = false;
		}
		if (pDoc->m_currentTemplateName == _T("Cylinder") && m_cylinder_template_btn.GetCheck() == 0)
		{
			m_arch_template_btn.SetCheck(0);
			m_cube_template_btn.SetCheck(0);
			m_cone_template_btn.SetCheck(0);
			m_cylinder_template_btn.SetCheck(1);
			m_strCurrentItem.LoadString(IDS_CYLINDER);
			m_sphere_template_btn.SetCheck(0);
			m_stairs_template_btn.SetCheck(0);
			bSearch = false;
		}
		if (pDoc->m_currentTemplateName == _T("Sphere") && m_sphere_template_btn.GetCheck() == 0)
		{
			m_arch_template_btn.SetCheck(0);
			m_cube_template_btn.SetCheck(0);
			m_cone_template_btn.SetCheck(0);
			m_cylinder_template_btn.SetCheck(0);
			m_sphere_template_btn.SetCheck(1);
			m_strCurrentItem.LoadString(IDS_SPHERE);
			m_stairs_template_btn.SetCheck(0);
			bSearch = false;
		}
		if (pDoc->m_currentTemplateName == _T("Stairs") && m_stairs_template_btn.GetCheck() == 0)
		{
			m_arch_template_btn.SetCheck(0);
			m_cube_template_btn.SetCheck(0);
			m_cone_template_btn.SetCheck(0);
			m_cylinder_template_btn.SetCheck(0);
			m_sphere_template_btn.SetCheck(0);
			m_stairs_template_btn.SetCheck(1);
			m_strCurrentItem.LoadString(IDS_STAIRS);
			
		}
		else	//	break out of the loop if nothing yet
			bSearch = false;
	}


			//	Restore focus to active view
	CMDIChildWnd *pMDIChild	=(CMDIChildWnd *)pDoc->m_pMainFrame->MDIGetActive();
	if(pMDIChild)
	{
		CView	*cv	=(CView *)pMDIChild->GetActiveView();
		if( cv)
			cv->SetFocus();
	}

	return true;
}







void CTemplatesDlg::OnButtonSphere() 
{
	if (m_pDoc->m_pMainFrame->m_createSpheroidDialog.m_hWnd == NULL)	//	post 0.55
		m_pDoc->CreateSpheroid();

	m_strCurrentItem.LoadString(IDS_SPHERE);
	UpdateData(FALSE);
}


void CTemplatesDlg::OnButtonCube() 
{
	if (m_pDoc->m_pMainFrame->m_createBoxDialog.m_hWnd == NULL)	//	post 0.55
		m_pDoc->CreateCube();

	m_strCurrentItem.LoadString(IDS_CUBE);
	UpdateData(FALSE);
}


void CTemplatesDlg::OnButtonCylinder() 
{
	if (m_pDoc->m_pMainFrame->m_createCylDialog.m_hWnd == NULL)	//	post 0.55
		m_pDoc->CreateCylinder();

	m_strCurrentItem.LoadString(IDS_CYLINDER);
	UpdateData(FALSE);
}
void CTemplatesDlg::OnButtonStairs() 
{
	if (m_pDoc->m_pMainFrame->m_createStaircaseDialog.m_hWnd == NULL)	//	post 0.55
		m_pDoc->CreateStaircase();	

	m_strCurrentItem.LoadString(IDS_STAIRS);

	UpdateData(FALSE);
}


void CTemplatesDlg::OnButtonCone() 
{
	if (m_pDoc->m_pMainFrame->m_createConeDialog.m_hWnd == NULL)	//	post 0.55
		m_pDoc->CreateCone();

	m_strCurrentItem.LoadString(IDS_CONE);
	UpdateData(FALSE);
}


void CTemplatesDlg::OnButtonArch() 
{
	if (m_pDoc->m_pMainFrame->m_createArchDialog.m_hWnd == NULL)	//	post 0.55
		m_pDoc->CreateArch();

	m_strCurrentItem.LoadString(IDS_ARCH);
	UpdateData(FALSE);
}

/*
void CTemplatesDlg::SetupPrefabList()
{
	// add object names to the object combo box
//	int NumObjectsAdded = 0;	// counts the number of library objects that were added to 
								// the combo box
	char			ObjectsDir[MAX_PATH];
	const char		*pObjDir;
	CString			strObjectsDir;
	BOOL			FileWasFound;
	char			Name[MAX_PATH];

	WIN32_FIND_DATA FindData;
	HANDLE			FindHandle;
	const Prefs		*pPrefs = (CGlobals::GetActiveDocument())->GetPrefs();
	
//	m_pDoc = CGlobals::GetActiveDocument;
//	Prefs = m_pDoc->GetPrefs ();

	// get Objects directory from INI
	pObjDir = Prefs_GetObjectsDir (pPrefs);
	::FilePath_AppendName (pObjDir, "*.3dt", ObjectsDir);

	m_prefabList.ResetContent();
	FindHandle = ::FindFirstFile (ObjectsDir, &FindData);

	FileWasFound = (FindHandle != INVALID_HANDLE_VALUE);

	while (FileWasFound)
	{
		::FilePath_GetName (FindData.cFileName, Name);
		m_prefabList.AddString (Name);
		FileWasFound = ::FindNextFile (FindHandle, &FindData);
	}

	::FindClose (FindHandle);
	if (m_prefabList.SetCurSel (0) == CB_ERR)
	{
		// couldn't set it...probably nothing in there
//		EnablePlaceObjectButton (FALSE);
	}
}
*/

void CTemplatesDlg::OnSelchangePrefabList() 
{
	m_iCurrentPrefab = m_prefabList.GetCurSel();
	OnPrefabBtn();
}



void CTemplatesDlg::OnDblclkPrefabList() 
{
	OnPrefabBtn();
	OnTemplateApply();
}


void CTemplatesDlg::OnPrefabBtn() 
{
	m_iCurrentPrefab = m_prefabList.GetCurSel();

	if (m_iCurrentPrefab != LB_ERR)
	{
		m_prefabList.GetText(m_iCurrentPrefab, m_sCurrentTemplateName);
		m_strCurrentItem = m_sCurrentTemplateName;
	}

		//	set this so the normal brush templates will know that they
		//	were not the most recent template tool chosen
	m_pDoc->m_currentTemplateName = "PreFab";


	CWnd *mainWnd = AfxGetApp()->GetMainWnd();
	ASSERT( mainWnd );
		// tell the GEditPro doc to create a template entity for the user to specify the location
	// of the objects with
//	m_pDoc = CGlobals::GetActiveDocument();	
	m_pDoc->CreateObjectTemplate();

							//	clear the Properties Panel
	m_pDoc->m_pMainFrame->HideAllPropDialogsBut(this);
	
	UnCheckAllTemplateButtons();

}


geBoolean CTemplatesDlg::GetCurrentTemplateName (char *pTemplateName)
{
	m_iCurrentPrefab = m_prefabList.GetCurSel ();

	if (m_iCurrentPrefab != LB_ERR)
	{
		m_prefabList.GetText(m_iCurrentPrefab, pTemplateName);

		return GE_TRUE;
	}
	return GE_FALSE;
}



void CTemplatesDlg::OnEntityBtn() 
{
	// Entity button was clicked
	// Get current entity type from combo box and create one...
	m_iCurrentEntity = m_entitiesCombo.GetCurSel();
	if (m_iCurrentEntity != LB_ERR)
	{
		m_pDoc->m_currentTemplateName = "Entity";		

		CString		strEntityName = _T("light");

		m_entitiesCombo.GetLBText(m_iCurrentEntity, m_strCurrentItem);
		
		//  We create a "light" entity as the template entity.
		//  In future, we'll have a "template" entity.
		m_pDoc->CreateEntity (strEntityName.GetBuffer(strEntityName.GetLength()));

					//	clear the Properties Panel
		m_pDoc->m_pMainFrame->HideAllPropDialogsBut(this);

		UnCheckAllTemplateButtons();
  }	
}

void CTemplatesDlg::OnSelchangeEntitiesCombo() 
{
	m_iCurrentEntity = m_entitiesCombo.GetCurSel ();
//	m_pDoc->m_currentTemplateName = "Prefab";
	m_pDoc->m_currentTemplateName = "Entity";
}


void CTemplatesDlg::OnCbnCloseupEntitiesCombo()
{
	OnEntityBtn();

}


void	CTemplatesDlg::UnCheckAllTemplateButtons()
{
	m_arch_template_btn.SetCheck(0);
	m_cube_template_btn.SetCheck(0);
	m_cone_template_btn.SetCheck(0);
	m_cylinder_template_btn.SetCheck(0);
	m_sphere_template_btn.SetCheck(0);
	m_stairs_template_btn.SetCheck(0);

	UpdateData(FALSE);
}




geBoolean CTemplatesDlg::GetCurrentEntityName(char *pEntityName)
{
	m_iCurrentEntity = m_entitiesCombo.GetCurSel ();

	if (m_iCurrentEntity != LB_ERR)
	{
		m_entitiesCombo.GetLBText (m_iCurrentEntity, pEntityName);
		return GE_TRUE;
	}
	return GE_FALSE;
}

void CTemplatesDlg::OnSkybutton() 
{
				//	post 0.5 release
				//	sets command panel tab to Textures, so we can browse textures
				//	while we set our sky
	if (m_pDoc->m_pMainFrame->m_CommandDialogBar)
	{
		CTabCtrl*	pCommandPanelTabCtrl = 
		(CTabCtrl*)m_pDoc->m_pMainFrame->m_CommandDialogBar.GetDlgItem(IDC_COMMAND_TAB);
		pCommandPanelTabCtrl->SetCurSel((MAINFRM_COMMANDPANEL_TAB)MAINFRM_COMMANDPANEL_TEXTURES);
		m_pDoc->m_pMainFrame->SetCommandPanelTab(MAINFRM_COMMANDPANEL_TEXTURES);
	}
	
//	CSkyDialog m_dSky;	//	post 0.5 release. now classwide
//	m_dSky.DoModal();	//	post 0.5 release. now modeless
						//	post 0.5 release. Create modeless sky dialog box
	if (!m_dSky)
	{
		m_dSky.Create(IDD_SKYDIALOG, m_pDoc->m_pMainFrame);
		m_dSky.BringWindowToTop();
		m_dSky.ShowWindow(SW_SHOW);
	}
						//	post 0.5 release. sky dialog's internal OnOK() destroys
						//	the sky dialog
}


void CTemplatesDlg::OnTemplateApply() 
{
//	m_pDoc = CGlobals::GetActiveDocument();	
	m_pDoc->AddBrushToWorld();	
}


void CTemplatesDlg::OnCancel()
{
	return;
}

