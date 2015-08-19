// SockProxy.h : main header file for the SOCKPROXY application
//

#if !defined(AFX_SOCKPROXY_H__F1A524A5_55AE_11D5_B587_00E04C39B036__INCLUDED_)
#define AFX_SOCKPROXY_H__F1A524A5_55AE_11D5_B587_00E04C39B036__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSockProxyApp:
// See SockProxy.cpp for the implementation of this class
//

class CSockProxyApp : public CWinApp
{
public:
	CSockProxyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSockProxyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSockProxyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOCKPROXY_H__F1A524A5_55AE_11D5_B587_00E04C39B036__INCLUDED_)
