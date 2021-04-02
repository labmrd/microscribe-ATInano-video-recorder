/*/////////////////////////////////////////////////////////////////////////

	Copyright (c) 19922-2011 Revware, Inc. All Rights Reserved.

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

  FILE:		Probe_MgrDlg.h : Main program interface class definition.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#pragma once

#include "armdll32.h"
#include <vector>
#include "FilterScribe.h"
#include "afxwin.h"

// CProbe_MgrDlg dialog
class CProbe_MgrDlg : public CDialogEx, public CFilterScribe
{
// Construction
public:
	CProbe_MgrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROBE_MGR_DIALOG };
	// MicroScribe group
	CStatic		m_ctlModelLblVal;
	CStatic		m_ctlSerialNumLblVal;
	CStatic		m_ctlDriverVersionLblVal;
	CStatic		m_ctlFirmwareVersionLblVal;
	CString		m_sModelVal;
	CString		m_sSerialNumVal;
	CString		m_sDriverVersionVal;
	CString		m_sFirmwareVersionVal;
	CButton		m_btnInches;
	CButton		m_btnMillimeters;
	CButton		m_ctlConnectionBtn;
	CButton		m_ctlConnectToSNChk;
	BOOL		m_bConnectToSN;
	CEdit		m_editTargetSN;
	CString		m_sTargetSN;
	CStatic		m_ctlConnectMsg;
	CStatic		m_ctlConnectChk;

	// Tip Type group
	CStatic		m_ctlTipTypeGroup;
	CComboBox	m_tipName;
	CEdit		m_editOffset;
	CString		m_sTipOffset;
	CButton		m_btnSetOffset;
	CStatic		m_ctlUnits; 
	CButton     m_btnDefaultTip;
	CButton		m_btnFixedOffsetTip;
	CButton		m_btnCustomTip;
	CButton		m_btnDefineTip;
	CButton		m_btnEnter_xyz;
 
	// Custom Tip group
    CStatic		m_ctlCustomTipGroup;
    CStatic		m_ctlStep0;
    CStatic		m_ctlStep1;
    CStatic		m_ctlStep2;
    CStatic		m_ctlStep2A;
    CStatic		m_ctlStep3;
    CStatic		m_ctlStep4;
    CEdit		m_editCustomTip;
	CButton		m_btnDoneHome;
    CButton		m_btnDoneSwitch;
	CButton		m_btnResetPts;
    CButton     m_btnSaveAll;
	CProgressCtrl  m_ctlProgressPts;

	// Position group
	CStatic		m_ctlPosX;
	CStatic		m_ctlPosY;
	CStatic		m_ctlPosZ;
	CStatic		m_ctlPosXval;
	CStatic		m_ctlPosYval;
	CStatic		m_ctlPosZval;
	CString		m_sPosXval;
	CString		m_sPosYval;
	CString		m_sPosZval;
	CStatic		m_ctlPosI;
	CStatic		m_ctlPosJ;
	CStatic		m_ctlPosK;
	CStatic		m_ctlPosIval;
	CStatic		m_ctlPosJval;
	CStatic		m_ctlPosKval;
	CString		m_sPosIval;
	CString		m_sPosJval;
	CString		m_sPosKval;
	CStatic		m_ctlButtonsLbl;
	CStatic		m_ctlBtnChk1;
	CStatic		m_ctlBtnChk2;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

private:
	int m_enumState;
	int m_auxState;

	int m_homeEncoder[NUM_ENCODERS];
	int m_encoderDelta[NUM_ENCODERS];

	float m_lengthOffset[4];
	int m_currentTip;
	int m_prevTip;
	bool m_userEntersOffsets;
	int m_prevSel;

	void IntDeviceInfo();
	bool SetSN();
	bool ValidateTipOnExit();
	void SetVisibleWidgets();

	std::vector<length_3D> m_pts;			// XYZ
	std::vector<angle_3D> m_orient;			// IJK
	std::vector<angle_3D> m_unit_orient;	// IJK (unit vector)

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnSave();
	afx_msg void OnDefaultTip();
    afx_msg void OnFixedOffset();
	afx_msg void OnBnClickedSetOffset();
	afx_msg void OnCustomTip();
     
    afx_msg void OnNewTip();
	afx_msg void OnDefaultHome();
    afx_msg void OnNewTipHome();
	afx_msg void OnSwitchTipName();
	afx_msg void OnResetPoints();
	afx_msg void OnBnClickedEnterXyz();
	afx_msg void OnBnClickedConnection();
	afx_msg void OnBnClickedConnectToSn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

public:
	afx_msg bool OnData();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedInches();
	afx_msg void OnBnClickedMilimeters();
};
