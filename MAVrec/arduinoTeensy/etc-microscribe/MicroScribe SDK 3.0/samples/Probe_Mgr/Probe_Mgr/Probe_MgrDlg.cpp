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

  FILE:		Probe_MgrDlg.cpp : Main program interface class definition
			implementation.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#include "stdafx.h"
#include "Probe_Mgr.h"
#include "Probe_MgrDlg.h"
#include "afxdialogex.h"
#include "ArmDll32.h"
#include "FilterScribe.h"
#include "Preferences.h"

///////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange( CDataExchange* pDX );

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx( CAboutDlg::IDD )
{
}

void CAboutDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialogEx::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CAboutDlg, CDialogEx )
END_MESSAGE_MAP()


///////////////////////////////////////////////////////
// CProbe_MgrDlg dialog

// States
enum STATES {INIT_STATE=0, USE_NEW_TIP , SWITCH_TIP, USE_DEFAULT_TIP, USE_FIXED_OFFSET, USE_CUSTOM_TIP, DONE_PTS};

// Aux states
enum AUX_STATES {UNDEF_STATE= -1, DISCONNECTED = 0, INFO_STATE, DONE_HOME, DONE_SWITCH};

// Raddio button choices
enum SELECTION {DEFAULT_TIP = 0, OFFSET_TIP, CUSTOM_TIP};

#define SERIAL_DEVICE	1
#define USB_DEVICE		2

CProbe_MgrDlg::CProbe_MgrDlg( CWnd* pParent /*=NULL*/ )
	: CDialogEx( CProbe_MgrDlg::IDD, pParent )
	, CFilterScribe()
	, m_sPosXval( _T("") )
	, m_sPosYval( _T("") )
	, m_sPosZval( _T("") )
	, m_sPosIval( _T("") )
	, m_sPosJval( _T("") )
	, m_sPosKval( _T("") )
	, m_sModelVal( _T("") )
	, m_sSerialNumVal( _T("") )
	, m_sDriverVersionVal( _T("") )
	, m_sFirmwareVersionVal( _T("") )
	, m_bConnectToSN( FALSE )
	, m_sTargetSN( _T("") )
	, m_sTipOffset( _T("") )
	, m_userEntersOffsets( false )
	, m_currentTip( 0 )
	, m_prevTip( 0 )
	, m_prevSel( -1 )
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CProbe_MgrDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialogEx::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_DEFAULT_TIP, m_btnDefaultTip );
	DDX_Control( pDX, IDC_FIXED_OFFSET_TIP, m_btnFixedOffsetTip );
	DDX_Control( pDX, IDC_EDIT_OFFSET, m_editOffset );
	DDX_Control( pDX, IDC_UNITS, m_ctlUnits );
	DDX_Control( pDX, IDC_CUSTOM_TIP, m_btnCustomTip );
	DDX_Control( pDX, IDC_TIPNAME, m_tipName );
	DDX_Control( pDX, IDC_REDEFINE, m_btnDefineTip );
	DDX_Control( pDX, IDC_ENTER_XYZ, m_btnEnter_xyz );
	DDX_Control( pDX, IDC_CUSTOM_TIP_GROUP, m_ctlCustomTipGroup );
	DDX_Control( pDX, IDC_STEP0, m_ctlStep0 );
	DDX_Control( pDX, IDC_EDIT_TIPNAME, m_editCustomTip );
	DDX_Control( pDX, IDC_STEP1, m_ctlStep1 );
	DDX_Control( pDX, IDC_DONE_HOME, m_btnDoneHome );
	DDX_Control( pDX, IDC_STEP2, m_ctlStep2 );
	DDX_Control( pDX, IDC_STEP2A, m_ctlStep2A );
	DDX_Control( pDX, IDC_DONE_SWITCH, m_btnDoneSwitch );
	DDX_Control( pDX, IDC_STEP3, m_ctlStep3 );
	DDX_Control( pDX, IDC_RESET_PTS, m_btnResetPts );
	DDX_Control( pDX, IDC_PROGRESS_PTS, m_ctlProgressPts );
	DDX_Control( pDX, IDC_STEP4, m_ctlStep4 );
	DDX_Control( pDX, IDC_SAVE_ALL, m_btnSaveAll );
	DDX_Control( pDX, IDC_POS_X, m_ctlPosX );
	DDX_Control( pDX, IDC_POS_Y, m_ctlPosY );
	DDX_Control( pDX, IDC_POS_Z, m_ctlPosZ );
	DDX_Control( pDX, IDC_POS_X_VAL, m_ctlPosXval );
	DDX_Control( pDX, IDC_POS_Y_VAL, m_ctlPosYval );
	DDX_Control( pDX, IDC_POS_Z_VAL, m_ctlPosZval );
	DDX_Text( pDX, IDC_POS_X_VAL, m_sPosXval );
	DDX_Text( pDX, IDC_POS_Y_VAL, m_sPosYval );
	DDX_Text( pDX, IDC_POS_Z_VAL, m_sPosZval );
	DDX_Control( pDX, IDC_POS_I, m_ctlPosI );
	DDX_Control( pDX, IDC_POS_J, m_ctlPosJ );
	DDX_Control( pDX, IDC_POS_K, m_ctlPosK );
	DDX_Control( pDX, IDC_POS_I_VAL, m_ctlPosIval );
	DDX_Control( pDX, IDC_POS_J_VAL, m_ctlPosJval );
	DDX_Control( pDX, IDC_POS_K_VAL, m_ctlPosKval );
	DDX_Text( pDX, IDC_POS_I_VAL, m_sPosIval );
	DDX_Text( pDX, IDC_POS_J_VAL, m_sPosJval );
	DDX_Text( pDX, IDC_POS_K_VAL, m_sPosKval );
	DDX_Control( pDX, IDC_CONNECT_TO_SN, m_ctlConnectToSNChk );
	DDX_Check( pDX, IDC_CONNECT_TO_SN, m_bConnectToSN );
	DDX_Control( pDX, IDC_TARGETSN, m_editTargetSN );
	DDX_Text( pDX, IDC_TARGETSN, m_sTargetSN );
	DDV_MaxChars( pDX, m_sTargetSN, 5 );
	DDX_Control( pDX, IDC_MODEL_VAL, m_ctlModelLblVal );
	DDX_Control( pDX, IDC_SERIALNUMBER_VAL, m_ctlSerialNumLblVal );
	DDX_Control( pDX, IDC_DRIVERVERSION_VAL, m_ctlDriverVersionLblVal );
	DDX_Control( pDX, IDC_FIRMWAREVERSION_VAL, m_ctlFirmwareVersionLblVal );	
	DDX_Text( pDX, IDC_MODEL_VAL, m_sModelVal );
	DDX_Text( pDX, IDC_SERIALNUMBER_VAL, m_sSerialNumVal );
	DDX_Text( pDX, IDC_DRIVERVERSION_VAL, m_sDriverVersionVal );
	DDX_Text( pDX, IDC_FIRMWAREVERSION_VAL, m_sFirmwareVersionVal );
	DDX_Control( pDX, IDC_TIP_TYPE_GROUP, m_ctlTipTypeGroup );
	DDX_Control( pDX, IDC_CONNECTION, m_ctlConnectionBtn );
	DDX_Control( pDX, IDC_STATIC_CONNECTED, m_ctlConnectMsg );
	DDX_Control( pDX, IDC_CHECK6, m_ctlConnectChk );
	DDX_Control( pDX, IDC_BUTTONS_LBL, m_ctlButtonsLbl );
	DDX_Control( pDX, IDC_CHECK_BTN1, m_ctlBtnChk1 );
	DDX_Control( pDX, IDC_CHECK_BTN2, m_ctlBtnChk2 );
	DDX_Control( pDX, IDC_SETOFFSET, m_btnSetOffset );
	DDX_Text( pDX, IDC_EDIT_OFFSET, m_sTipOffset );
	DDX_Control( pDX, IDC_INCHES, m_btnInches );
	DDX_Control( pDX, IDC_MILIMETERS, m_btnMillimeters );
}

BEGIN_MESSAGE_MAP( CProbe_MgrDlg, CDialogEx )
	ON_BN_CLICKED( IDC_DEFAULT_TIP, &CProbe_MgrDlg::OnDefaultTip )
	ON_BN_CLICKED( IDC_FIXED_OFFSET_TIP, &CProbe_MgrDlg::OnFixedOffset )
	ON_BN_CLICKED( IDC_CUSTOM_TIP, &CProbe_MgrDlg::OnCustomTip )
	ON_BN_CLICKED( IDC_REDEFINE, &CProbe_MgrDlg::OnNewTip )
	ON_BN_CLICKED( IDC_DONE_HOME, &CProbe_MgrDlg::OnDefaultHome )
	ON_BN_CLICKED( IDC_DONE_SWITCH, &CProbe_MgrDlg::OnNewTipHome )
	ON_BN_CLICKED( IDC_SAVE_ALL, &CProbe_MgrDlg::OnSave )
	ON_BN_CLICKED( IDC_RESET_PTS, &CProbe_MgrDlg::OnResetPoints )
	ON_CBN_SELCHANGE( IDC_TIPNAME, &CProbe_MgrDlg::OnSwitchTipName )
	ON_BN_CLICKED( IDC_ENTER_XYZ, &CProbe_MgrDlg::OnBnClickedEnterXyz )
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED( IDC_CONNECTION, &CProbe_MgrDlg::OnBnClickedConnection )
	ON_BN_CLICKED( IDC_CONNECT_TO_SN, &CProbe_MgrDlg::OnBnClickedConnectToSn )
	ON_BN_CLICKED( IDC_SETOFFSET, &CProbe_MgrDlg::OnBnClickedSetOffset )
	ON_BN_CLICKED( IDOK, &CProbe_MgrDlg::OnBnClickedOk )
	ON_BN_CLICKED( IDCANCEL, &CProbe_MgrDlg::OnBnClickedCancel )
	ON_BN_CLICKED(IDC_INCHES, &CProbe_MgrDlg::OnBnClickedInches)
	ON_BN_CLICKED(IDC_MILIMETERS, &CProbe_MgrDlg::OnBnClickedMilimeters)
END_MESSAGE_MAP()


// CProbe_MgrDlg message handlers

BOOL CProbe_MgrDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT( (IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX );
	ASSERT( IDM_ABOUTBOX < 0xF000 );

	CMenu* pSysMenu = GetSystemMenu( FALSE );
	if ( pSysMenu != NULL )
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString( IDS_ABOUTBOX );
		ASSERT( bNameValid );
		if ( !strAboutMenu.IsEmpty() )
		{
			pSysMenu->AppendMenu( MF_SEPARATOR );
			pSysMenu->AppendMenu( MF_STRING, IDM_ABOUTBOX, strAboutMenu );
		}
	}

	// Set the icon for this dialog. The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon( m_hIcon, TRUE );	// Set big icon
	SetIcon( m_hIcon, FALSE );	// Set small icon

	// Needed for access back to the dialog window for messages display.
	m_pCWnd = this;

	m_enumState = INIT_STATE;
	m_auxState = DISCONNECTED;

	// Set the initial state of the information display widgets.
	SetVisibleWidgets();

	return TRUE;	// return TRUE unless you set the focus to a control
}


void CProbe_MgrDlg::OnSysCommand( UINT nID, LPARAM lParam )
{
	if ( (nID & 0xFFF0) == IDM_ABOUTBOX )	// Trap the AboutBox message.
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand( nID, lParam );
	}
}

// If you add a minimize button to your dialog, you will need the code below
// to draw the icon. For MFC applications using the document/view model,
// this is automatically done for you by the framework.

void CProbe_MgrDlg::OnPaint()
{
	if ( IsIconic() )
	{
		CPaintDC dc( this );	// Device context for painting.

		SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0 );

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics( SM_CXICON );
		int cyIcon = GetSystemMetrics( SM_CYICON );
		CRect rect;
		GetClientRect( &rect );
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon( x, y, m_hIcon );
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CProbe_MgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// For generic cleanup at the time that the dialog is destroyed.
void CProbe_MgrDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}

void CProbe_MgrDlg::OnSave()
{
	CString tipName,x,y,z;

	m_editCustomTip.GetWindowText( tipName );
	tipName.Trim();	// Trim leading and trailing whitespace.
	if ( tipName.GetLength() > 31 )
		tipName.Truncate( 31 );	// Truncate to a max of 31 characters.

	if ( m_userEntersOffsets )
	{
		bool offsetsEntered = true;
		GetDlgItem( IDC_EDIT_X )->GetWindowText( x );
		if ( x.IsEmpty() )
			offsetsEntered = false;
		else
			m_lengthOffset[0] = (float)_ttof( x );

		GetDlgItem( IDC_EDIT_Y )->GetWindowText( y );
		if ( y.IsEmpty() )
			offsetsEntered = false;
		else
			m_lengthOffset[1] = (float)_ttof( y );

		GetDlgItem( IDC_EDIT_Z )->GetWindowText( z );
		if ( z.IsEmpty() )
			offsetsEntered = false;
		else
			m_lengthOffset[2] = (float)_ttof( z );

		if ( !offsetsEntered )
		{
			MessageBox( _T("Enter your offsets first, then press Save"), _T("MicroScribe Probe Manager"), MB_OK );

			return;
		}
	}

	if( GetIsMetric() )
	{
		float scaleFactor = 25.4f;
		m_lengthOffset[0] /= scaleFactor;
		m_lengthOffset[1] /= scaleFactor;
		m_lengthOffset[2] /= scaleFactor;
	}

	CT2CA cName( tipName );
	if ( ArmSaveTipProfile( m_currentTip, m_encoderDelta, m_lengthOffset, cName ) != ARM_SUCCESS )
		MessageBox( _T("New tip has not been saved properly"), MB_OK );

	m_enumState = USE_CUSTOM_TIP;
	m_auxState = INFO_STATE;
	m_userEntersOffsets = false;
	Sleep( 500 );

	m_ctlProgressPts.SetPos( 0 );
	m_tipName.DeleteString( m_currentTip -1 );
	m_tipName.InsertString( m_currentTip -1 , tipName );
	m_tipName.SetCurSel( m_currentTip -1 );
	
	SetVisibleWidgets();
}

void CProbe_MgrDlg::OnDefaultTip()
{
	if ( m_prevSel == DEFAULT_TIP )
		return;	// Nothing to do. Reselected the current selection.

	m_prevSel = DEFAULT_TIP;
	m_enumState = USE_DEFAULT_TIP;
	m_btnDefaultTip.SetCheck( true );
	m_btnFixedOffsetTip.SetCheck( false );
	m_btnCustomTip.SetCheck( false );
	
	// If going back from fixed offset to default tip, no need to pop dialog box
	// but need to 0 the tip offset.
	if ( m_currentTip == 0 )
	{
		m_auxState = INFO_STATE;
		PutOffsetTipDelta( 0.0 );
	}

	// If click back here while defining the tip, get rid of the checkmarks.
	if ( m_auxState > INFO_STATE ) 
		m_auxState = UNDEF_STATE;
	
	m_tipName.EnableWindow( false );
	m_editOffset.EnableWindow( false );
	m_btnSetOffset.EnableWindow( false );
	m_btnDefineTip.EnableWindow( false );
	m_btnEnter_xyz.EnableWindow( false );
	
	SetVisibleWidgets();
}

void CProbe_MgrDlg::OnFixedOffset()
{
	if ( m_prevSel == OFFSET_TIP )
		return;	// Nothing to do. Reselected the current selection.

	m_prevSel = OFFSET_TIP;
	m_enumState = USE_FIXED_OFFSET;
	m_btnDefaultTip.SetCheck( false );
	m_btnFixedOffsetTip.SetCheck( true );
	m_btnCustomTip.SetCheck( false );
	
	// If prev state was custom tip, need to re-home, else not.
	if ( m_currentTip == 0 )
		m_auxState = INFO_STATE;

	// If click back here while defining the tip, get rid of the checkmarks.
	if ( m_auxState > INFO_STATE ) 
		m_auxState = UNDEF_STATE;
	
	m_tipName.EnableWindow( false );
	m_editOffset.EnableWindow( true );
	m_btnSetOffset.EnableWindow( true );
	m_btnDefineTip.EnableWindow( false );
	m_btnEnter_xyz.EnableWindow( false );
	
	OnBnClickedSetOffset();
	SetVisibleWidgets();
}

void CProbe_MgrDlg::OnCustomTip()
{
	if ( m_prevSel == CUSTOM_TIP )
		return;	// Nothing to do. Reselected the current selection.

	m_prevSel = CUSTOM_TIP;
	m_enumState = USE_CUSTOM_TIP;
	m_btnFixedOffsetTip.SetCheck( false );
	m_btnDefaultTip.SetCheck( false );
	m_btnCustomTip.SetCheck( true );
	m_tipName.EnableWindow( true );
	m_editOffset.EnableWindow( false );
	m_btnSetOffset.EnableWindow( false );
	m_btnDefineTip.EnableWindow( true );

	// If the current tip selection is the default tip or an offset tip
	// this will reset the specified tip offset back to 0.0 as if PutOffsetTipDelta(0.0) were called.
	(void)ArmSetTipProfile(m_currentTip);

	if ( m_currentTip == 0 ) 
		OnSwitchTipName();
	else
		SetVisibleWidgets();		
}

void CProbe_MgrDlg::OnSwitchTipName()
{
	int newTip = m_tipName.GetCurSel();

	if ( newTip != m_currentTip - 1 && m_auxState < DONE_HOME )
		m_enumState = SWITCH_TIP;

	SetVisibleWidgets();
}

void CProbe_MgrDlg::OnNewTip()
{
	if ( m_enumState == USE_NEW_TIP )
	{
		int userChoice = MessageBox( _T("Your custom tip offsets have not been defined. \nPress OK to continue to define your tip \nor CANCEL to go back to the Default Tip"), _T("MicroScribe Probe Manager"), MB_OKCANCEL );
		if ( userChoice == IDOK )
		{
			SetVisibleWidgets();

			return;
		}
		else
		{
			m_auxState = UNDEF_STATE;
		
			m_currentTip = 0;
			m_prevTip = 0;
			ArmSetTipProfile( m_currentTip );
			OnDefaultTip();
			MessageBox( _T("Place the device in home position with the Default tip \nand press the Home button before acquiring data"), _T("MicroScribe Probe Manager"), MB_OK );

			return;
		}
	}

	m_enumState = USE_NEW_TIP ? (m_enumState == SWITCH_TIP || m_enumState == USE_CUSTOM_TIP) : SWITCH_TIP;

	if ( m_enumState == SWITCH_TIP )
	{
		OnSwitchTipName();

		return;
	}

	if ( m_currentTip && m_enumState == USE_NEW_TIP )
	{
		int userChoice = MessageBox( _T("Please replace the currently installed tip with the Default Tip.\nPress OK once you have installed the Default Tip.\nPress Cancel to continue using the currently installed tip."),
						 _T("MicroScribe Probe Manager"), MB_OKCANCEL );

		// This assumes they did actually change to the default tip.
		// Catch OK and cancel.
		if ( userChoice == IDOK )
		{
			m_currentTip = 0;
			m_prevTip = m_currentTip;
			ArmSetTipProfile( m_currentTip );
		}
		else
		{
			m_enumState = USE_CUSTOM_TIP;
			OnSwitchTipName();
		}
	}

	SetVisibleWidgets();
}

void CProbe_MgrDlg::OnDefaultHome() 
{	
	ArmSetSoftHome();

	int ret = SERIAL_DEVICE;

	ret = GetDeviceStatus();

	if ( ret == USB_DEVICE )
		GetHomePosition( m_homeEncoder );
	else
		int result = ArmGetEncoderCount( m_homeEncoder );

	m_btnDoneSwitch.EnableWindow( true );
	m_btnDoneHome.EnableWindow( false );
	m_auxState = DONE_HOME;

	SetVisibleWidgets();	
}

// This method is common to defining a new tip and switching to an already defined tip.
void CProbe_MgrDlg::OnNewTipHome() 
{
 	if ( m_enumState == USE_NEW_TIP )
	{
		int currEncoder[NUM_ENCODERS];
		int result = ArmGetEncoderCount( currEncoder );

		// Ignore differences in base encoder.
		m_encoderDelta[0] = 0;
		for ( int i=1; i< NUM_ENCODERS; ++i )
			m_encoderDelta[i] = currEncoder[i] - m_homeEncoder[i];
	
		ArmSetSoftHome();

		m_ctlProgressPts.EnableWindow( true );
		m_ctlProgressPts.SetRange( 0,20 );
		m_ctlProgressPts.SetPos( 0 );
		m_ctlProgressPts.SetStep( 1 );	
		m_currentTip = m_tipName.GetCurSel() + 1;
		m_auxState = DONE_SWITCH;

		if ( m_userEntersOffsets )
			m_enumState = DONE_PTS;

		SetVisibleWidgets();
		ArmSetHomeEncoderOffset( m_encoderDelta );
	}
	else
	{	
		if ( m_enumState == USE_DEFAULT_TIP || m_enumState == USE_FIXED_OFFSET ) 
			m_currentTip = 0;

		if ( m_enumState == SWITCH_TIP )
			m_currentTip = m_tipName.GetCurSel() + 1;

		// If we are switcing from a Cuwstom Tip to an Offset Tip, this is the second of two steps
		// and we do not want to home the MicroScribe again after user has switched to the Offset Tip.
		if ( m_enumState != USE_FIXED_OFFSET )
			ArmSetSoftHome();
		
		// Start using new the tip (encoder +tip offset ), but do not actually do SetTipProfile until
		// OK is pressed.
		int homeOffset[NUM_ENCODERS];
		ArmGetTipProfileHomeOffset( m_currentTip, homeOffset );
		homeOffset[6] = 0;	// Reserved encoder.
		ArmSetHomeEncoderOffset( homeOffset );

		// If we are switching to a Default Tip or Offset Tip, the values for tip 0 are all zero values.
		float tipOffset[3];
		ArmGetTipProfilePositionOffset( m_currentTip, tipOffset );
		ArmSetTipPositionOffset( ARM_INCHES, tipOffset[0], tipOffset[1], tipOffset[2] );

		// If we are switching to an Offset Tip, we want to set the offset length.
		if ( m_enumState == USE_FIXED_OFFSET )
			OnBnClickedSetOffset();
		
		m_auxState = INFO_STATE;
		SetVisibleWidgets();
	}

	m_btnDoneSwitch.EnableWindow( false );	
}

void CProbe_MgrDlg::OnResetPoints() 
{
	m_pts.clear();
	m_orient.clear();
	m_unit_orient.clear();
	m_ctlProgressPts.SetPos( 0 );
	m_enumState = USE_NEW_TIP;
	SetVisibleWidgets();
}

bool CProbe_MgrDlg::OnData()
{
	static int minNumPts=20;

	m_sPosXval.Format( _T("%2.4f"), m_x );
	m_sPosYval.Format( _T("%2.4f"), m_y );
	m_sPosZval.Format( _T("%2.4f"), m_z );
	m_sPosIval.Format( _T("%2.4f"), m_i );
	m_sPosJval.Format( _T("%2.4f"), m_j );
	m_sPosKval.Format( _T("%2.4f"), m_k );

	m_ctlPosXval.SetWindowText( m_sPosXval );
	m_ctlPosYval.SetWindowText( m_sPosYval );
	m_ctlPosZval.SetWindowText( m_sPosZval );
	m_ctlPosIval.SetWindowText( m_sPosIval );
	m_ctlPosJval.SetWindowText( m_sPosJval );
	m_ctlPosKval.SetWindowText( m_sPosKval );

	// Display which buttons are pressed.
	m_ctlBtnChk1.ShowWindow( (m_buttons & ARM_BUTTON_1 ) ? true : false );
	m_ctlBtnChk2.ShowWindow( (m_buttons & ARM_BUTTON_2 ) ? true : false );

	if( !m_ButtonFilter.DoDataFilter( m_buttons ) )
		return false;

	if ( m_pts.size() == minNumPts )
		return false;

	if ( m_auxState == DONE_SWITCH ) 
	{	
		length_3D new_pt;
		angle_3D new_angle;
		angle_3D new_unit_orientation;
	
		ArmGetTipPosition( &new_pt );
		ArmGetTipOrientation( &new_angle );
		ArmGetTipOrientationUnitVector( &new_unit_orientation );

		m_pts.push_back( new_pt );
		m_orient.push_back( new_angle );
		m_unit_orient.push_back( new_unit_orientation );

		m_ctlProgressPts.StepIt();

		if ( m_pts.size() == minNumPts )		
		{	
			length_3D posOffset;
			int result = ArmGenerateTipPositionOffsetEx( &m_pts.front(), &m_orient.front(), minNumPts, posOffset, &m_unit_orient.front() );
			// Altrnate legacy version
			//int result = ArmGenerateTipPositionOffset( &pts.front(), &orient.front(), minNumPts, posOffset );

			if ( result == PTS_TOO_CLOSE )
			{
				m_enumState = DONE_PTS;	
				int userChoice = MessageBox( _T("Calibration points or angles too close to one another. Press OK to accept 0 offsets, Cancel to go back to Default Tip"), _T("MicroScribe Probe Manager"), MB_OKCANCEL );
				
				if ( userChoice == IDOK )
				{
					m_lengthOffset[0] = 0.0;
					m_lengthOffset[1] = 0.0;
					m_lengthOffset[2] = 0.0;
				}
				else 
				{
					m_currentTip =0;
					m_prevTip = m_currentTip ;
					ArmSetTipProfile( m_currentTip );
					OnDefaultTip();
					MessageBox( _T("Place the device in home position with the Default tip \nand press the Home button before acquiring data"), _T("MicroScribe Probe Manager"), MB_OK );
				}
			}
			else if ( result != ARM_SUCCESS ) 
			{
				MessageBox( _T("Calibration incomplete. Please collect 20 additional points"),_T("MicroScribe Probe Manager") );
				m_ctlProgressPts.SetPos( 0 );
				m_pts.clear();
				m_orient.clear();
				m_unit_orient.clear();
			}		 
	 		else 
			{
				m_enumState = DONE_PTS;

				double radius = sqrt( posOffset.x * posOffset.x + 
									  posOffset.y * posOffset.y +
									  posOffset.z * posOffset.z );

				m_lengthOffset[0] = posOffset.x;
				m_lengthOffset[1] = posOffset.y;
				m_lengthOffset[2] = posOffset.z;

				// This calculation is not necessary, but it gives yo ua value that can be used to define an Offset Tip.
				if ( posOffset.z !=0 )
					m_lengthOffset[3] = (float)(radius * posOffset.z / fabs( posOffset.z ));
				else
					m_lengthOffset[3] = 0.;

				// Cannot call ArmSetTipProfile( m_currentTip ) until after ArmSaveTipProfile() is called with the new information.
				// ArmSaveTipProfile() causes the new information to be written to the registry by ArmDLLxx. Subsequent calls to
				// ArmSetTipProfile() cause ArmDLLxx to pull the information from the registry and set the MicroScribe.
				// ArmSaveTipProfile() is called within OnSave which is triggered when the Save Custom Tip Definition button is pressed by the user.
				// Yet, we want the displayed data stream to start showing accurate values form the newly calibrated probe before the user chooses to save.
				// We can do this by directly doing the work of ArmSetTipProfile() here.
				// ArmSetHomeEncoderOffset() has already been called earlier in the calibration process.  We only need to call ArmSetTipPositionOffset()
				// to get the data displaying correctly for the newly calibrated probe.
				// Lastly, all calibration information is expected to be stored in inches. If the data collected here is in MM, we can send the
				// offsets to the MicroScribe in MM, but when we save the data it is first converted to inchesw.  See OnSave().
				(void)ArmSetTipPositionOffset( GetIsMetric() ? ARM_MM : ARM_INCHES, m_lengthOffset[0], m_lengthOffset[1], m_lengthOffset[2] );
			}
			SetVisibleWidgets();
		}

	}
	return true;
}


// Basic device information.
void CProbe_MgrDlg::IntDeviceInfo()
{
	BSTR sTemp;
	BSTR sTemp2;
	(void)GetModelName( &sTemp );
	m_sModelVal = sTemp;

	(void)GetSerialNumber( &sTemp );
	m_sSerialNumVal = sTemp;

	(void)GetVersion( &sTemp, &sTemp2 );	// ArmDLL, and MicroScribe firmware versions.
	m_sDriverVersionVal = sTemp;
	m_sFirmwareVersionVal = sTemp2;

	m_ctlModelLblVal.SetWindowText( m_sModelVal );
	m_ctlSerialNumLblVal.SetWindowText( m_sSerialNumVal );
	m_ctlDriverVersionLblVal.SetWindowText( m_sDriverVersionVal );
	m_ctlFirmwareVersionLblVal.SetWindowText( m_sFirmwareVersionVal );

	m_tipName.Clear();

	int numTips = 5;
	char tipName[32];
	CString sTipName;

	// Start indexing at 1 to keep default tip separate.
	for ( int i=1; i<numTips; ++i )
	{
		ArmGetTipNameData( i, tipName );
		sTipName = tipName;
		m_tipName.AddString( sTipName );
	}

	ArmGetTipProfile( &m_currentTip );
	m_prevTip = m_currentTip;
	m_prevSel = -1;	// No previous selection.

	// Populate the Offset Tip length edit field with the last value saved.
	CString strTmp;

	float fLength = thePrefs.m_fOffsetTipLength;
	if ( GetIsMetric() )
		fLength = fLength * 25.4f;

	strTmp.Format( _T("%2.4f"), fLength );
	m_editOffset.SetWindowText( LPCTSTR( strTmp ) );

	if ( thePrefs.m_bOffsetTipFlag )
	{
		m_tipName.SetCurSel( 0 );
		OnFixedOffset();
	}
	else if ( m_currentTip == 0 )
	{
		m_tipName.SetCurSel( 0 );
		OnDefaultTip();
	}
	else
	{
		m_tipName.SetCurSel( m_currentTip - 1 );
		OnCustomTip();
	}
}

bool CProbe_MgrDlg::SetSN()
{
	// Must update the dialog data to assure retrieval of the current SN entry.
	UpdateData();

	// Make sure checkbox is unchecked if SN is empty.
	if ( m_sTargetSN.IsEmpty() )
		m_ctlConnectToSNChk.SetCheck( 0 );

	// If the SN checkbox is checked, use it.
	if ( m_bConnectToSN )
		return PutSN( m_sTargetSN );	// Send value, even if empty.

	// Otherwise, make sure that the SN is cleared out to avoid using previously set values.
	(void)PutSN( _T("") );

	return true;
}

void CProbe_MgrDlg::SetVisibleWidgets()
{
	// Tip calibration widgets.
	bool switchTip = (m_enumState == SWITCH_TIP);
	bool useDefaultTip = (m_enumState == USE_DEFAULT_TIP);
	bool useFixedOffset = (m_enumState == USE_FIXED_OFFSET);
	bool useNewTip = (m_enumState == USE_NEW_TIP || m_enumState == DONE_PTS);
	bool donePts = (m_enumState == DONE_PTS);

	bool bConnected = GetIsConnected();
	m_ctlModelLblVal.EnableWindow( bConnected );
	m_ctlSerialNumLblVal.EnableWindow( bConnected );
	m_ctlDriverVersionLblVal.EnableWindow( bConnected );
	m_ctlFirmwareVersionLblVal.EnableWindow( bConnected );
	m_btnInches.SetCheck( !thePrefs.m_bMetric );
	m_btnMillimeters.SetCheck( thePrefs.m_bMetric );

	if ( !bConnected )
	{
		// Everything is turned off.
		m_ctlTipTypeGroup.ShowWindow( false );
		m_btnDefaultTip.ShowWindow( false );
		m_btnFixedOffsetTip.ShowWindow( false );
		m_btnCustomTip.ShowWindow( false );
		m_editOffset.ShowWindow( false );
		m_btnSetOffset.ShowWindow( false );
		m_ctlUnits.ShowWindow( false );
		GetDlgItem(IDC_WARNING )->ShowWindow( false );
		m_tipName.ShowWindow( false );
		m_btnDefineTip.ShowWindow( false );
		m_btnEnter_xyz.ShowWindow( false );
		m_ctlCustomTipGroup.ShowWindow( false );
		m_ctlStep0.ShowWindow( false );
		m_ctlStep1.ShowWindow( false );
		m_ctlStep2.ShowWindow( false );
		m_ctlStep2A.ShowWindow( false );	
		m_ctlStep3.ShowWindow( false );
		m_ctlStep4.ShowWindow( false );	
		m_editCustomTip.ShowWindow( false );
		m_btnDoneHome.ShowWindow( false );
		m_btnDoneSwitch.ShowWindow( false );
		m_ctlProgressPts.ShowWindow( false );
		m_btnSaveAll.ShowWindow( false );
		m_btnResetPts.ShowWindow( false );
		m_ctlProgressPts.ShowWindow( false );
		m_btnDefineTip.ShowWindow( false );
		m_btnEnter_xyz.ShowWindow( false );
		GetDlgItem( IDC_EDIT_X )->ShowWindow( false );
		GetDlgItem( IDC_EDIT_Y )->ShowWindow( false );
		GetDlgItem( IDC_EDIT_Z )->ShowWindow( false );	
		GetDlgItem( IDC_OFFSET_X2 )->ShowWindow( false );
		GetDlgItem( IDC_OFFSET_Y2 )->ShowWindow( false );
		GetDlgItem( IDC_OFFSET_Z2 )->ShowWindow( false );	
		GetDlgItem( IDC_CHECK3 )->ShowWindow( false );
		GetDlgItem( IDC_CHECK4 )->ShowWindow( false );
		GetDlgItem( IDC_CHECK1 )->ShowWindow( false );
		GetDlgItem( IDC_CHECK2 )->ShowWindow( false );
		GetDlgItem( IDC_OFFSET_X )->ShowWindow( false );
		GetDlgItem( IDC_OFFSET_Y )->ShowWindow( false );
		GetDlgItem( IDC_OFFSET_Z )->ShowWindow( false );
		GetDlgItem( IDC_X_VAL )->ShowWindow( false );
		GetDlgItem( IDC_Y_VAL )->ShowWindow( false );
		GetDlgItem( IDC_Z_VAL )->ShowWindow( false );
		GetDlgItem( IDC_POSITION_GROUP )->ShowWindow( false );	
		m_ctlPosX.ShowWindow( false );
		m_ctlPosY.ShowWindow( false );
		m_ctlPosZ.ShowWindow( false );
		m_ctlPosXval.ShowWindow( false );
		m_ctlPosYval.ShowWindow( false );
		m_ctlPosZval.ShowWindow( false );
		m_ctlPosI.ShowWindow( false );
		m_ctlPosJ.ShowWindow( false );
		m_ctlPosK.ShowWindow( false );
		m_ctlPosIval.ShowWindow( false );
		m_ctlPosJval.ShowWindow( false );
		m_ctlPosKval.ShowWindow( false );
		m_ctlButtonsLbl.ShowWindow( false );
		m_ctlBtnChk1.ShowWindow( false );
		m_ctlBtnChk2.ShowWindow( false );

		// Clear the device information.
		m_sModelVal.Empty();
		m_sSerialNumVal.Empty();
		m_sDriverVersionVal.Empty();
		m_sFirmwareVersionVal.Empty();
		m_ctlModelLblVal.SetWindowTextW( m_sModelVal );
		m_ctlSerialNumLblVal.SetWindowTextW( m_sSerialNumVal );
		m_ctlDriverVersionLblVal.SetWindowTextW( m_sDriverVersionVal );
		m_ctlFirmwareVersionLblVal.SetWindowTextW( m_sFirmwareVersionVal );
	}
	else
	{
		m_ctlTipTypeGroup.ShowWindow( true );
		m_btnDefaultTip.ShowWindow( true );
		m_btnFixedOffsetTip.ShowWindow( true );
		m_btnCustomTip.ShowWindow( true );
		m_editOffset.ShowWindow( true );
		m_btnSetOffset.ShowWindow( true );
		m_ctlUnits.ShowWindow( true );
		GetDlgItem(IDC_WARNING )->ShowWindow( true );
		m_tipName.ShowWindow( true );
		m_btnDefineTip.ShowWindow( true );
		m_btnEnter_xyz.ShowWindow( true );

		bool isClicked = (switchTip || useDefaultTip || useFixedOffset) && ((m_auxState == UNDEF_STATE) || (m_auxState > INFO_STATE));
		m_ctlCustomTipGroup.ShowWindow( useNewTip || isClicked );
		m_ctlCustomTipGroup.EnableWindow( useNewTip || isClicked );
		m_ctlStep0.ShowWindow( useNewTip );
		m_ctlStep1.ShowWindow( useNewTip || (useFixedOffset && ((m_auxState == UNDEF_STATE) || (m_auxState > INFO_STATE))) );
		m_ctlStep2.ShowWindow( useNewTip || isClicked );
		m_ctlStep2A.ShowWindow( useNewTip || (isClicked && !useFixedOffset) );

		m_ctlStep3.ShowWindow( useNewTip );
		if ( useNewTip && m_userEntersOffsets )
			m_ctlStep3.SetWindowText( _T("Step 4 of 5: Enter the known offsets") );
		else
			m_ctlStep3.SetWindowText( _T("Step 4 of 5: Collect 20 points on calibration fixture") );
	
		m_ctlStep4.ShowWindow( useNewTip );
	
		m_editCustomTip.ShowWindow( useNewTip );
		if ( useNewTip && m_auxState < DONE_HOME )
		{
			CString strTmp;
			m_tipName.GetLBText( m_tipName.GetCurSel(),strTmp );
			m_editCustomTip.SetWindowText( (LPCTSTR)strTmp );
		}
		m_btnDoneHome.ShowWindow( useNewTip || (useFixedOffset && ((m_auxState == UNDEF_STATE) || (m_auxState > INFO_STATE))) );
		m_btnDoneHome.EnableWindow( (useNewTip || useFixedOffset) && (m_auxState < DONE_HOME) );
		m_btnDoneSwitch.ShowWindow( useNewTip || isClicked );
		m_btnDoneSwitch.EnableWindow( useNewTip || isClicked );
		m_ctlProgressPts.ShowWindow( useNewTip && !m_userEntersOffsets );
		m_btnSaveAll.ShowWindow( useNewTip );
		m_btnSaveAll.EnableWindow( m_enumState == DONE_PTS );
		m_btnResetPts.ShowWindow( useNewTip && !m_userEntersOffsets );
		m_ctlProgressPts.ShowWindow( useNewTip && !m_userEntersOffsets );
		m_btnSaveAll.EnableWindow( false );	
	
		if ( (useDefaultTip || useFixedOffset || switchTip) && ((m_auxState == UNDEF_STATE) || (m_auxState > INFO_STATE)) )
		{
			m_ctlCustomTipGroup.SetWindowText( _T("Complete these steps to switch your tip") );
			if ( useDefaultTip )
			{
				m_ctlStep2.SetWindowText( _T("Step 1 of 1: Switch to your Default Tip") );
				m_btnDoneSwitch.EnableWindow( true );
			}
			else if ( useFixedOffset )
			{
				m_ctlStep1.SetWindowText( _T("Step 1 of 2: Place in home position with Default Tip") );
				m_ctlStep2.SetWindowText( _T("Step 2 of 2: Switch to your Offset Tip") );
				m_btnDoneSwitch.EnableWindow( m_auxState == DONE_HOME );
			}
			else
			{
				m_ctlStep2.SetWindowText( _T("Step 1 of 1: Switch to your Custom Tip") );
				m_btnDoneSwitch.EnableWindow( true );
			}
		}

		if ( useNewTip && ((m_auxState == UNDEF_STATE) || (m_auxState > INFO_STATE)) )
		{
			m_ctlCustomTipGroup.SetWindowText( _T("Complete these steps to (re)define your tip") );
			m_ctlStep1.SetWindowText( _T("Step 2 of 5: Place in home position with Default Tip") );
			m_ctlStep2.SetWindowText( _T("Step 3 of 5: Switch to your custom tip") );
			m_btnDoneSwitch.EnableWindow( m_auxState == DONE_HOME );
		}

		m_btnInches.EnableWindow(  !(m_auxState == DONE_SWITCH) );
		m_btnMillimeters.EnableWindow(  !(m_auxState == DONE_SWITCH) );

		if ( GetIsMetric() ) 
			m_ctlUnits.SetWindowText( _T("mm") );
		else
			m_ctlUnits.SetWindowText( _T("inches") );

		if ( !((m_auxState == UNDEF_STATE) || (m_auxState > INFO_STATE)) )
			m_auxState = UNDEF_STATE;

		m_btnDefineTip.ShowWindow( true );
		m_btnDefineTip.SetCheck( useNewTip );
		m_btnEnter_xyz.ShowWindow( true );
		m_btnEnter_xyz.EnableWindow( this->IsDlgButtonChecked( IDC_REDEFINE ) );
		m_btnEnter_xyz.SetCheck( (int) m_userEntersOffsets );
	
		bool bRes = (m_auxState == DONE_SWITCH) && m_userEntersOffsets;
		GetDlgItem( IDC_EDIT_X )->EnableWindow( bRes );
		GetDlgItem( IDC_EDIT_Y )->EnableWindow( bRes );
		GetDlgItem( IDC_EDIT_Z )->EnableWindow( bRes );
	
		GetDlgItem( IDC_OFFSET_X2 )->EnableWindow( bRes );
		GetDlgItem( IDC_OFFSET_Y2 )->EnableWindow( bRes );
		GetDlgItem( IDC_OFFSET_Z2 )->EnableWindow( bRes );
	
		GetDlgItem( IDC_EDIT_X )->ShowWindow( m_userEntersOffsets );
		GetDlgItem( IDC_EDIT_Y )->ShowWindow( m_userEntersOffsets );
		GetDlgItem( IDC_EDIT_Z )->ShowWindow( m_userEntersOffsets );
	
		GetDlgItem( IDC_OFFSET_X2 )->ShowWindow( m_userEntersOffsets );
		GetDlgItem( IDC_OFFSET_Y2 )->ShowWindow( m_userEntersOffsets );
		GetDlgItem( IDC_OFFSET_Z2 )->ShowWindow( m_userEntersOffsets );
	
		GetDlgItem( IDC_CHECK3 )->ShowWindow( m_auxState >= DONE_SWITCH );
		GetDlgItem( IDC_CHECK4 )->ShowWindow( m_enumState == DONE_PTS );
		GetDlgItem( IDC_CHECK1 )->ShowWindow( m_auxState >= DONE_HOME && !useFixedOffset );
		GetDlgItem( IDC_CHECK2 )->ShowWindow( m_auxState >= DONE_HOME );
		m_btnSaveAll.EnableWindow( donePts );
	
		m_btnResetPts.EnableWindow( m_auxState >= DONE_SWITCH );
	
		bRes = (m_enumState == DONE_PTS) && !m_userEntersOffsets;
		GetDlgItem( IDC_OFFSET_X )->ShowWindow( bRes );
		GetDlgItem( IDC_OFFSET_Y )->ShowWindow( bRes );
		GetDlgItem( IDC_OFFSET_Z )->ShowWindow( bRes );
		GetDlgItem( IDC_X_VAL )->ShowWindow( bRes );
		GetDlgItem( IDC_Y_VAL )->ShowWindow( bRes );
		GetDlgItem( IDC_Z_VAL )->ShowWindow( bRes );
	
		if ( bRes )
		{
			CString x_offset, y_offset, z_offset;
			x_offset.Format( _T("%2.4f"),m_lengthOffset[0] );
			y_offset.Format( _T("%2.4f"),m_lengthOffset[1] );
			z_offset.Format( _T("%2.4f  Offset: %2.4f"), m_lengthOffset[2], m_lengthOffset[3] );
			// Alternate legacy method
			//z_offset.Format( _T("%2.4f"),m_lengthOffset[2] );
			GetDlgItem( IDC_X_VAL )->SetWindowText( x_offset );
			GetDlgItem( IDC_Y_VAL )->SetWindowText( y_offset );
			GetDlgItem( IDC_Z_VAL )->SetWindowText( z_offset );
		}
		GetDlgItem( IDC_POSITION_GROUP )->ShowWindow( true );	
		m_ctlPosX.ShowWindow( true );
		m_ctlPosY.ShowWindow( true );
		m_ctlPosZ.ShowWindow( true );
		m_ctlPosXval.ShowWindow( true );
		m_ctlPosYval.ShowWindow( true );
		m_ctlPosZval.ShowWindow( true );
		m_ctlPosI.ShowWindow( true );
		m_ctlPosJ.ShowWindow( true );
		m_ctlPosK.ShowWindow( true );
		m_ctlPosIval.ShowWindow( true );
		m_ctlPosJval.ShowWindow( true );
		m_ctlPosKval.ShowWindow( true );
		m_ctlButtonsLbl.ShowWindow( true );
	}
}

void CProbe_MgrDlg::OnBnClickedEnterXyz()
{
	// Don't allow state change in the middle of point collection.
	if ( m_auxState < DONE_SWITCH )
		m_userEntersOffsets = !m_userEntersOffsets;

	m_btnEnter_xyz.SetCheck( (int)m_userEntersOffsets );

	SetVisibleWidgets();
}


// Toggles active connection to the MicroScribe.
// In this implementation we create and destroy the process thread as well as connect and disconnect to the MicroScribe.
// Alternatively, you could create and maintain the process thread and cycle through Connect and Disconnect actions.
void CProbe_MgrDlg::OnBnClickedConnection()
{
	// Default state unless we connect.
	m_auxState = DISCONNECTED;

	if ( GetIsConnected() )
	{
		if ( ValidateTipOnExit() )
			return;

		// Disconnect.
		TerminateThread();

		m_ctlConnectionBtn.SetWindowTextW( _T("Connect") );
		m_ctlConnectMsg.SetWindowText( _T("MicroScribe not connected") );
		m_ctlConnectChk.ShowWindow( false );
		m_ctlConnectToSNChk.EnableWindow( true );
		m_editTargetSN.EnableWindow( true );

		// A small delay to allow the thread to terminate.
		// Otherwise the thread and device connection status will have not changed and widgets will not display properly.
		int i = 0;
		do
		{
			// Break the moment we detect that the connection has been dropped.
			if ( !GetIsConnected() )
				break;

			Sleep( 100 );
			i++;
		} while ( i < 10 );	// Up to a 1 second delay.
	}
	else
	{
		// Initialize connection options.
		// (void)PutInterval( 50 );	// Data interval in ms. Check the return value to confirm specified value is within allowed limits.

		// Set the serial number to use if one is specified.
		if ( !SetSN() )
		{
			MessageBox( _T("Invalid serial number."), _T("Error"), MB_OK );
		}
		else if ( StartMonitor() )	// Setup and start device communications.
		{
			// Since MicroScribe communications are started on a thread, we do not get a direct response here that the start of communications succeeded.
			// We must give the thread some time to start and establish communications and then we can query to see if we were successful
			// Otherwise the thread status and device data is not likely to have been updated and widgets will not display properly.
			// Alternatively you could choose to not display the widgets until after receiving a message from the active thread.

			// Sleep for up to 1 second.
			bool bConnected = false;
			int i = 0;
			do
			{
				if ( GetIsConnected() )
				{
					bConnected = true;
					break;
				}

				Sleep( 100 );
				i++;
			} while ( i < 10 );

			if ( bConnected )
			{
				IntDeviceInfo();	
				SetDoApplyFilters( true );
				m_userEntersOffsets = false;

				// Set MicroScribe to display the desired units.
				PutIsMetric( thePrefs.m_bMetric );

#pragma message(__FILE__" ToDo: The data trigger button is hardwired to 1")
//ToDo: We can set this to a user selectable value.
				m_ButtonFilter.PutActivePedal( 1 );
				m_ButtonFilter.PutActiveOnDown( true );
				m_ButtonFilter.PutActiveOnToggle( true );
				SetDoApplyFilters( false );

				m_auxState = INFO_STATE;

				m_ctlConnectionBtn.SetWindowTextW( _T("Disconnect") );
				m_ctlConnectMsg.SetWindowText( _T("MicroScribe connected") );
				m_ctlConnectChk.ShowWindow( true );
				m_ctlConnectToSNChk.EnableWindow( false );
				m_editTargetSN.EnableWindow( false );
			}
			else
			{
				// Make sure that the thread is closed.
				TerminateThread();

				MessageBox( _T("Unable to connect to a MicroScribe."), _T("Error"), MB_OK );
			}
		}
		else
		{
			MessageBox( _T("Unable to initiate MicroScribe connection."), _T("Error"), MB_OK );
		}
	}

	SetVisibleWidgets();
}


void CProbe_MgrDlg::OnBnClickedConnectToSn()
{
	(void)UpdateData();

	m_editTargetSN.EnableWindow( m_bConnectToSN );
}


void CProbe_MgrDlg::OnBnClickedSetOffset()
{
	UpdateData();
	float fVal = (float)_ttof( m_sTipOffset );

	// Ultimately calls ArmCustomTip().
	// The units are determined by the most recent call to ArmSetLengthUnits().
	// Since the offset length edit field is maintained in the current units
	// we can use that value without conversion.
	PutOffsetTipDelta( fVal );
}


bool CProbe_MgrDlg::ValidateTipOnExit()
{
	if ( (m_enumState == USE_NEW_TIP || m_enumState == DONE_PTS) )
	{		
		int userChoice = MessageBox( _T("Your custom tip offsets have not been defined. \nPress OK to continue to define your tip \nor CANCEL to go back to the Default Tip"), _T("MicroScribe Probe Manager"), MB_OKCANCEL );
		if ( userChoice == IDOK )
		{
			SetVisibleWidgets();

			return true;
		}
		else
		{
			m_currentTip = 0;
			MessageBox( _T("Place the device in home position with the Default tip \nand press the Home button before acquiring data"), _T("MicroScribe Probe Manager"), MB_OK );
		}
	}

	if ( ArmSetTipProfile( m_currentTip ) != ARM_SUCCESS )
		MessageBox( _T("New tip has not been set properly"), _T("MicroScribe Probe Manager"), MB_OK );

	// We save what ever value was set for the Offset tip length.
	CString s;		
	m_editOffset.GetWindowText( s );
	float fLength = (float)_ttof(LPCTSTR(s));
	if ( GetIsMetric() )
		fLength = fLength / 25.4f;
	thePrefs.m_fOffsetTipLength = fLength;

	if ( m_enumState == USE_FIXED_OFFSET )
	{
		// Make sure that the offset length was sent to the MicroScribe.
		// Cannot assume that the user pressed the Set button.

		thePrefs.m_bOffsetTipFlag = true;
		PutOffsetTipDelta( thePrefs.m_fOffsetTipLength );
	}
	else
	{
		thePrefs.m_bOffsetTipFlag = false;
	}

	return false;
}

void CProbe_MgrDlg::OnBnClickedOk()
{
	if ( GetIsConnected() )
		if ( ValidateTipOnExit() )
			return;

	m_pts.clear();
	m_orient.clear();
	m_unit_orient.clear();

	SetDoApplyFilters( true );
	TerminateThread();

	CDialogEx::OnOK();
}


void CProbe_MgrDlg::OnBnClickedCancel()
{
	if ( m_enumState == USE_NEW_TIP || m_enumState == DONE_PTS )
	{
		int userChoice = MessageBox( _T("Your custom tip offsets have not been defined. \nPress OK to continue to define your tip \nor CANCEL to go back to the Default Tip"), _T("MicroScribe Probe Manager"), MB_OKCANCEL );
		if ( userChoice == IDCANCEL )
		{
			ArmSetTipProfile( 0 );
			MessageBox( _T("Place the device in home position with the Default tip \nand press the Home button before acquiring data."), _T("MicroScribe Probe Manager"), MB_OK );
		}
		else
		{
			SetVisibleWidgets();

			return;
		}
	}
	else if ( m_prevTip != m_currentTip )
	{
		char tipName[32];
		ArmGetTipNameData( m_prevTip, tipName );
		CString sName( tipName );
		CString str;
		str.Format( _T("Place the device in home position with %s tip \nand press the Home button before acquiring data."), sName );
		MessageBox( str, _T("MicroScribe Probe Manager"), MB_OK );
		ArmSetTipProfile( m_prevTip );
	}

	SetDoApplyFilters( true );
	TerminateThread();

	m_pts.clear();
	m_orient.clear();
	m_unit_orient.clear();

	CDialogEx::OnCancel();
}


void CProbe_MgrDlg::OnBnClickedInches()
{
	if ( thePrefs.m_bMetric )
	{
		thePrefs.m_bMetric = false;
		m_btnInches.SetCheck( true );
		m_btnMillimeters.SetCheck( false );

		if ( GetIsConnected() )
			PutIsMetric( thePrefs.m_bMetric );

		if ( m_auxState == UNDEF_STATE )
			m_auxState = INFO_STATE;

		// We must adjust the Offset tip length displayed.
		CString s;		
		m_editOffset.GetWindowText( s );
		float fLength = (float)_ttof(LPCTSTR(s));
		fLength = fLength / 25.4f;
		s.Format( _T("%2.4f"), fLength );
		m_editOffset.SetWindowText( LPCTSTR( s ) );

		SetVisibleWidgets();
	}
}


void CProbe_MgrDlg::OnBnClickedMilimeters()
{
	if ( !thePrefs.m_bMetric )
	{
		thePrefs.m_bMetric = true;
		m_btnInches.SetCheck( false );
		m_btnMillimeters.SetCheck( true );

		if ( GetIsConnected() )
			PutIsMetric( thePrefs.m_bMetric );

		if ( m_auxState == UNDEF_STATE )
			m_auxState = INFO_STATE;

		// We must adjust the Offset tip length displayed.
		CString s;		
		m_editOffset.GetWindowText( s );
		float fLength = (float)_ttof(LPCTSTR(s));
		fLength = fLength * 25.4f;
		s.Format( _T("%2.4f"), fLength );
		m_editOffset.SetWindowText( LPCTSTR( s ) );

		SetVisibleWidgets();
	}
}
