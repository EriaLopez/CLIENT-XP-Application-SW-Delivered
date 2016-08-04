// BasePCGUI.h : main header file for the BASEPCGUI application
//

#if !defined(AFX_BASEPCGUI_H__ADDCA510_0A64_4823_A0E6_7E497D23ED1A__INCLUDED_)
#define AFX_BASEPCGUI_H__ADDCA510_0A64_4823_A0E6_7E497D23ED1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "..\resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBasePCGUIApp:
// See BasePCGUI.cpp for the implementation of this class
//

class CBasePCGUIApp : public CWinApp
{
public:
	CBasePCGUIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasePCGUIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBasePCGUIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASEPCGUI_H__ADDCA510_0A64_4823_A0E6_7E497D23ED1A__INCLUDED_)
