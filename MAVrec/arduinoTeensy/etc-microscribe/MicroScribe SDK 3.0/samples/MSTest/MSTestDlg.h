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

  FILE:		MSTestDlg.h : Dialog header file.

  PURPOSE:	Example program shows the proper use of the Armdll32 
			functions for communicating with the MicroScribe digitizer.
   
  By Valentino Felipe, May 2002
     Tom Welsh, Dec 2011

**********************************************************************/

#if !defined(AFX_MSTESTDLG_H__E13F4CFA_529C_4661_B015_E14D6A93D5D0__INCLUDED_)
#define AFX_MSTESTDLG_H__E13F4CFA_529C_4661_B015_E14D6A93D5D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ArmCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMSTestDlg dialog

class CMSTestDlg : public CDialog
{
// Construction
public:
	CMSTestDlg(CWnd* pParent = NULL);	// standard constructor
	void UpdateDlgData();
	void GetProductInfo();

// Dialog Data
	//{{AFX_DATA(CMSTestDlg)
	enum { IDD = IDD_MSTEST_DIALOG };
	CEdit	m_editStatus;
	CEdit	m_editSerialNum;
	CEdit	m_editProtocol;
	CEdit	m_editProductName;
	CEdit	m_editModelName;
	CEdit	m_editFirmwareVersion;
	CEdit	m_editDLLVersion;
	CEdit	m_editButton2;
	CEdit	m_editButton1;
	CButton	m_buttonConnect;
	float	m_fPositionX;
	float	m_fPositionY;
	float	m_fPositionZ;
	int		m_nDOF;
	DWORD	m_dwElbow;
	DWORD	m_dwShPitch;
	DWORD	m_dwShYaw;
	DWORD	m_dwStRoll;
	DWORD	m_dwWrPitch;
	DWORD	m_dwWrRoll;
	float	m_fVectorX;
	float	m_fVectorY;
	float	m_fVectorZ;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSTestDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CArmCtrl* m_acInstance;
	arm_rec m_armData;
	angle m_arJointAngles[NUM_DOF];
	length_3D m_l3TipPosition;
	angle_3D m_a3TipUnitVector;
	DWORD m_dwButtonsState;

	// Generated message map functions
	//{{AFX_MSG(CMSTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonConnect();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSTESTDLG_H__E13F4CFA_529C_4661_B015_E14D6A93D5D0__INCLUDED_)
