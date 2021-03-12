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

  FILE:		ArmCtrl.h : description of a class that encapsulates
			Armdll32 functions.

  PURPOSE:	Example program shows the proper use of the Armdll32 
			functions for communicating with the MicroScribe digitizer.
   
  By Valentino Felipe, May 2002
     Tom Welsh, Dec 2011

**********************************************************************/

#if !defined(AFX_ARMWINDOW_H__0775857C_843F_427E_A67E_B6B86418143F__INCLUDED_)
#define AFX_ARMWINDOW_H__0775857C_843F_427E_A67E_B6B86418143F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CArmCtrl.h : header file
//

#include <armdll32.h>
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CArmCtrl window

class CArmCtrl : public CWnd
{
// Construction
public:
	CArmCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CArmCtrl)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CArmCtrl();
	arm_rec GetArmData();
	void GetJointAngles(angle* pAngles);
	length_3D GetTipPosition();
	angle_3D GetTipOrientation();
	DWORD GetButtonsState();
	BOOL Connect();
	void Disconnect();

	friend hci_result _cdecl CANT_OPEN_error_handler(hci_rec *hci, hci_result condition);
	friend hci_result _cdecl NO_HCI_error_handler(hci_rec *hci, hci_result condition);
	friend hci_result _cdecl BAD_PORT_error_handler(hci_rec *hci, hci_result condition);
	friend hci_result _cdecl CANT_BEGIN_error_handler(hci_rec *hci, hci_result condition);

	// Generated message map functions
protected:
	arm_rec m_armData;
	angle m_arJointAngles[NUM_DOF];
	length_3D m_l3TipPosition;
	angle_3D m_a3TipUnitVector;
	DWORD m_dwButtonsState;

	//{{AFX_MSG(CArmCtrl)
	//}}AFX_MSG
	afx_msg LRESULT UpdateArm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ConnectionLost(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARMWINDOW_H__0775857C_843F_427E_A67E_B6B86418143F__INCLUDED_)
