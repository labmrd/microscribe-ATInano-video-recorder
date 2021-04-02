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

  FILE:		MSTestDlg.cpp : Dialog implementation file.

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
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSTestDlg dialog

CMSTestDlg::CMSTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMSTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMSTestDlg)
	m_fPositionX = 0.0f;
	m_fPositionY = 0.0f;
	m_fPositionZ = 0.0f;
	m_nDOF = 0;
	m_dwElbow = 0;
	m_dwShPitch = 0;
	m_dwShYaw = 0;
	m_dwStRoll = 0;
	m_dwWrPitch = 0;
	m_dwWrRoll = 0;
	m_fVectorX = 0.0f;
	m_fVectorY = 0.0f;
	m_fVectorZ = 0.0f;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMSTestDlg)
	DDX_Control(pDX, IDC_EDIT_STATUS, m_editStatus);
	DDX_Control(pDX, IDC_EDIT_SERIALNUM, m_editSerialNum);
	DDX_Control(pDX, IDC_EDIT_PROTOCOL, m_editProtocol);
	DDX_Control(pDX, IDC_EDIT_PRODUCTNAME, m_editProductName);
	DDX_Control(pDX, IDC_EDIT_MODELNAME, m_editModelName);
	DDX_Control(pDX, IDC_EDIT_FIRMWAREVERSION, m_editFirmwareVersion);
	DDX_Control(pDX, IDC_EDIT_DLLVERSION, m_editDLLVersion);
	DDX_Control(pDX, IDC_EDIT_BUTTON2, m_editButton2);
	DDX_Control(pDX, IDC_EDIT_BUTTON1, m_editButton1);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_buttonConnect);
	DDX_Text(pDX, IDC_EDIT_POSX, m_fPositionX);
	DDX_Text(pDX, IDC_EDIT_POSY, m_fPositionY);
	DDX_Text(pDX, IDC_EDIT_POSZ, m_fPositionZ);
	DDX_Text(pDX, IDC_EDIT_DOF, m_nDOF);
	DDX_Text(pDX, IDC_EDIT_ELBOW, m_dwElbow);
	DDX_Text(pDX, IDC_EDIT_SH_PITCH, m_dwShPitch);
	DDX_Text(pDX, IDC_EDIT_SH_YAW, m_dwShYaw);
	DDX_Text(pDX, IDC_EDIT_ST_ROLL, m_dwStRoll);
	DDX_Text(pDX, IDC_EDIT_W_PITCH, m_dwWrPitch);
	DDX_Text(pDX, IDC_EDIT_W_ROLL, m_dwWrRoll);
	DDX_Text(pDX, IDC_EDIT_ROLL, m_fVectorX);
	DDX_Text(pDX, IDC_EDIT_PITCH, m_fVectorY);
	DDX_Text(pDX, IDC_EDIT_YAW, m_fVectorZ);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMSTestDlg, CDialog)
	//{{AFX_MSG_MAP(CMSTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSTestDlg message handlers

BOOL CMSTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_acInstance = NULL;
	memset(&m_armData, 0, sizeof(arm_rec));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMSTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMSTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMSTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMSTestDlg::OnButtonConnect() 
{
	CWaitCursor wc;

	if(m_acInstance)
	{
		KillTimer(1);
		m_acInstance->DestroyWindow();
		delete m_acInstance;
		m_acInstance = NULL;
		m_buttonConnect.SetWindowText("Connect");
		return;
	}

	m_acInstance = new CArmCtrl();
	if( m_acInstance->CreateEx(WS_EX_CLIENTEDGE,
				_T("#32768"),"DataWindow",
				WS_POPUPWINDOW | WS_CAPTION | WS_OVERLAPPED,
				5, 5, 90, 90, NULL, NULL) )
	{
		BOOL bRes = m_acInstance->Connect();
		if(bRes)
		{
			m_buttonConnect.SetWindowText("Disconnect");
			SetTimer( 1, 50, 0);
			GetProductInfo();
		}
		else
		{
			m_acInstance->DestroyWindow();
			delete m_acInstance;
			m_acInstance = NULL;
		}
	}
	
}

BOOL CMSTestDlg::DestroyWindow() 
{
	KillTimer(1);

	if(m_acInstance)
	{
		m_acInstance->DestroyWindow();
		delete m_acInstance;
		m_acInstance = NULL;
		m_buttonConnect.SetWindowText("Connect");
	}
	
	return CDialog::DestroyWindow();
}

void CMSTestDlg::OnTimer(UINT_PTR nIDEvent) 
{
	if(1 == nIDEvent && m_acInstance)
	{
		// The display here is only updated every 50ms.
		// However, CArmCtrl collects and processes data 
		// each time it receives an arm message.
		m_armData = m_acInstance->GetArmData();
		m_acInstance->GetJointAngles(m_arJointAngles);
		m_l3TipPosition = m_acInstance->GetTipPosition();
		m_a3TipUnitVector = m_acInstance->GetTipOrientation();
		m_dwButtonsState = m_acInstance->GetButtonsState();
		UpdateDlgData();
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CMSTestDlg::UpdateDlgData()
{
	static DWORD dwLastButtonsState = 0;
	BOOL bSaveLast = FALSE;

	if( !(dwLastButtonsState & ARM_BUTTON_1) && (m_dwButtonsState & ARM_BUTTON_1) )
	{
		// Look for the transition from inactive to active
		m_editButton1.SetWindowText("DOWN");
		bSaveLast = TRUE;
	}
	else if((dwLastButtonsState & ARM_BUTTON_1) && !(m_dwButtonsState & ARM_BUTTON_1))
	{
		// Look for the transition from active to inactive
		m_editButton1.SetWindowText("UP");
		bSaveLast = TRUE;
	}

	if( !(dwLastButtonsState & ARM_BUTTON_2) && (m_dwButtonsState & ARM_BUTTON_2) )
	{
		// Look for the transition from inactive to active
		m_editButton2.SetWindowText("DOWN");
		bSaveLast = TRUE;
	}
	else if((dwLastButtonsState & ARM_BUTTON_2) && !(m_dwButtonsState & ARM_BUTTON_2))
	{
		// Look for the transition from active to inactive
		m_editButton2.SetWindowText("UP");
		bSaveLast = TRUE;
	}
	
	if(bSaveLast)
		dwLastButtonsState = m_dwButtonsState;

	m_fPositionX = m_l3TipPosition.x;
	m_fPositionY = m_l3TipPosition.y;
	m_fPositionZ = m_l3TipPosition.z;

	m_fVectorX = m_a3TipUnitVector.x;
	m_fVectorY = m_a3TipUnitVector.y;
	m_fVectorZ = m_a3TipUnitVector.z;

	// Not wanting to display degrees with decimals, casting floats to DWORDs
	m_dwShYaw = (DWORD)m_arJointAngles[0];
	m_dwShPitch = (DWORD)m_arJointAngles[1];
	m_dwElbow = (DWORD)m_arJointAngles[2];
	m_dwWrRoll = (DWORD)m_arJointAngles[3];
	m_dwWrPitch = (DWORD)m_arJointAngles[4];
	m_dwStRoll = (DWORD)m_arJointAngles[5];
	UpdateData(FALSE);
}

void CMSTestDlg::GetProductInfo()
{
	int nRes;
	char buffer1[MAX_PATH], buffer2[MAX_PATH];
	device_status dsDeviceStatus;

	if(m_acInstance)
	{
		nRes = ArmGetProductName(buffer1, MAX_PATH);
		if(nRes == ARM_SUCCESS)
			m_editProductName.SetWindowText(buffer1);

		nRes = ArmGetModelName(buffer1, MAX_PATH);
		if(nRes == ARM_SUCCESS)
			m_editModelName.SetWindowText(buffer1);

		nRes = ArmGetSerialNumber(buffer1, MAX_PATH);
		if(nRes == ARM_SUCCESS)
			m_editSerialNum.SetWindowText(buffer1);

		nRes = ArmGetNumDOF(&m_nDOF);

		nRes = ArmGetVersion(buffer1, buffer2, MAX_PATH);
		if(nRes == ARM_SUCCESS)
		{
			m_editDLLVersion.SetWindowText(buffer1);
			m_editFirmwareVersion.SetWindowText(buffer2);
		}

		nRes = ArmGetDeviceStatus (&dsDeviceStatus);
		if(nRes == ARM_SUCCESS)
		{
			if(dsDeviceStatus.status & ARM_CONNECTED)
				m_editStatus.SetWindowText("Connected");
			else
				m_editStatus.SetWindowText("Not Connected");

			if(dsDeviceStatus.status & ARM_USING_SERIAL_PORT)
			{
				sprintf_s(buffer1,"COM: %d, Baud: %d",dsDeviceStatus.PortNumber, dsDeviceStatus.Baud);
				m_editProtocol.SetWindowText(buffer1);
			}
			else if(dsDeviceStatus.status & ARM_USING_USB_PORT)
				m_editProtocol.SetWindowText("USB");

		}
	}
}
