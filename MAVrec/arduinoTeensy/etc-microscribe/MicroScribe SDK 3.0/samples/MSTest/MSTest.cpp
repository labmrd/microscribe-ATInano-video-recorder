/**********************************************************************
	Copyright (c) 2002-2011 Revware, Inc. All Rights Reserved.

	Confidential property not for distribution except under the
	expressed written consent of Revware, Inc.
	PO Box 90786, Raleigh, NC 27675-0786 USA  Phone 919-790-0000

	REVWARE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
	INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
	IN NO EVENT SHALL REVWARE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
	CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
	LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
	NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
	CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

	This source code is provided "As Is" with no warranty whatsoever
	as example MicroScribe interface code for Revware customers.
	Usage is covered under the terms of the Revware Non-Disclosure
	Agreement (NDA) and the MicroScribe SDK license.

  FILE:		MSTest.cpp : Defines the class behaviors for the application.

  PURPOSE:	Example program shows the proper use of the Armdll32 
			functions for communicating with the MicroScribe digitizer.
   
  By Valentino Felipe, May 2002
     Tom Welsh, Dec 2011

**********************************************************************/

#include "stdafx.h"
#include "MSTest.h"
#include "MSTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSTestApp

BEGIN_MESSAGE_MAP(CMSTestApp, CWinApp)
	//{{AFX_MSG_MAP(CMSTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSTestApp construction

CMSTestApp::CMSTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMSTestApp object

CMSTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMSTestApp initialization

BOOL CMSTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	CMSTestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
