// TractorClient.h : main header file for the TRACTORCLIENT application
//

#if !defined(AFX_TRACTORCLIENT_H__B7926C8B_D2A1_46C4_A61B_50D149734AE9__INCLUDED_)
#define AFX_TRACTORCLIENT_H__B7926C8B_D2A1_46C4_A61B_50D149734AE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTractorClientApp:
// See TractorClient.cpp for the implementation of this class
//

class CTractorClientApp : public CWinApp
{
public:
	CTractorClientApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTractorClientApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTractorClientApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACTORCLIENT_H__B7926C8B_D2A1_46C4_A61B_50D149734AE9__INCLUDED_)
