// ac3info.h : main header file for the AC3INFO application
//

#if !defined(AFX_AC3INFO_H__E363EF07_AFCB_11D2_A379_00805FE32B78__INCLUDED_)
#define AFX_AC3INFO_H__E363EF07_AFCB_11D2_A379_00805FE32B78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAc3infoApp:
// See ac3info.cpp for the implementation of this class
//

class CAc3infoApp : public CWinApp
{
public:
	CAc3infoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAc3infoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAc3infoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AC3INFO_H__E363EF07_AFCB_11D2_A379_00805FE32B78__INCLUDED_)
