/*************************************************************************/
/*				Copyright (c) 2000-2010 NT Kernel Resources.		     */
/*                           All Rights Reserved.                        */
/*                          http://www.ntkernel.com                      */
/*                           ndisrd@ntkernel.com                         */
/*																		 */
/* Description: include file for standard system include files,          */
/*  or project specific include files that are used frequently, but      */
/*      are changed infrequently										 */
/*************************************************************************/

#if !defined(AFX_STDAFX_H__E249799D_B7A4_4E88_A85C_9FBD855E854F__INCLUDED_)
#define AFX_STDAFX_H__E249799D_B7A4_4E88_A85C_9FBD855E854F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif // WIN32_LEAN_AND_MEAN

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif //_CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <tchar.h>
#include <stdlib.h>

#include "common.h"
#include "ndisapi.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E249799D_B7A4_4E88_A85C_9FBD855E854F__INCLUDED_)
