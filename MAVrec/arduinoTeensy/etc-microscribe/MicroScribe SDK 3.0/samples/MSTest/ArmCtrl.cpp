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

  FILE:		ArmCtrl.cpp : implementation of the class which organizes
			the calls to Armdll32.

  PURPOSE:	Example program shows the proper use of the Armdll32 
			functions for communicating with the MicroScribe digitizer.
   
  By Valentino Felipe, May 2002
     Tom Welsh, Dec 2011

**********************************************************************/

#include "stdafx.h"
#include "ArmCtrl.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CArmWindow

CArmCtrl::CArmCtrl()
{
}

CArmCtrl::~CArmCtrl()
{
}

// Armdll32 communitcates with the client app through windows
// messages.  In order to detect these messages, they must be
// successfully registered by the application.
const UINT g_wmArmMessage = RegisterWindowMessage(ARM_MESSAGE); 
const UINT g_wmArmLostMsg = RegisterWindowMessage(ARM_DISCONNECTED_MESSAGE); 


BEGIN_MESSAGE_MAP(CArmCtrl, CWnd)
	//{{AFX_MSG_MAP(CArmCtrl)
	//}}AFX_MSG_MAP
	// Map the registered messages to functions that will
	// handle the events.
	ON_REGISTERED_MESSAGE(g_wmArmMessage, UpdateArm)
	ON_REGISTERED_MESSAGE(g_wmArmLostMsg, ConnectionLost)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArmCtrl message handlers

BOOL CArmCtrl::Connect()
{
	int nResult; 

	// Make sure the arm message was registered correctly.
	if (!g_wmArmMessage) 
	{ 
		AfxMessageBox( "Unable to register Microscribe window message" ); 
		return FALSE; 
	}
	
	// The ArmStart function prepares ArmDll32 for operation. 
	// Internally, it spawns a read thread used to retireve 
	// data from the device.
	nResult = ArmStart(m_hWnd);
	if ( ARM_SUCCESS != nResult ) 
	{ 
		AfxMessageBox( "Unable to Start ArmDll32" ); 
		return FALSE; 
	} 

	// English only dialog boxes are automatically presented by Armdll32
	// in order to preserve backward compatibility with v1.0.  You MUST
	// pass in valid custom error handlers for the following or pass NULL
	// to suppress these dialog boxes.  Error codes returned by ArmConnect()
	// allows your application to handle the problemed situations.

	// For this sample program, we have provided example error handlers
	// instead of simply passing NULL.
//	nResult = ArmSetErrorHandlerFunction(NO_HCI_HANDLER, NULL);
//	nResult = ArmSetErrorHandlerFunction(BAD_PORT_HANDLER, NULL);
//	nResult = ArmSetErrorHandlerFunction(CANT_OPEN_HANDLER, NULL);
//	nResult = ArmSetErrorHandlerFunction(CANT_BEGIN_HANDLER, NULL);

	nResult = ArmSetErrorHandlerFunction(NO_HCI_HANDLER, NO_HCI_error_handler);
	if ( ARM_SUCCESS != nResult ) 
	{ 
		AfxMessageBox( "Unable to change NO_HCI_HANDLER" ); 
	} 

	nResult = ArmSetErrorHandlerFunction(BAD_PORT_HANDLER, BAD_PORT_error_handler);
	if ( ARM_SUCCESS != nResult ) 
	{ 
		AfxMessageBox( "Unable to change BAD_PORT_HANDLER" ); 
	} 

	nResult = ArmSetErrorHandlerFunction(CANT_OPEN_HANDLER, CANT_OPEN_error_handler);
	if ( ARM_SUCCESS != nResult ) 
	{ 
		AfxMessageBox( "Unable to change CANT_OPEN_HANDLER" ); 
	} 

	nResult = ArmSetErrorHandlerFunction(CANT_BEGIN_HANDLER, CANT_BEGIN_error_handler);
	if ( ARM_SUCCESS != nResult ) 
	{ 
		AfxMessageBox( "Unable to change CANT_BEGIN_HANDLER" ); 
	} 

	// The USB port is always checked first.  The port and baud
	// values are only used if the device is not found on a USB
	// port.  Passing 0,0 will check all serial ports.
	nResult = ArmConnect(0,0); 
	if ( ARM_SUCCESS != nResult ) 
	{ 
		AfxMessageBox( "Unable to Connect ArmDll32" ); 
		return FALSE; 
	} 

	// ARM_FULL: ArmDll32 calculates and updates the Cartesian 
	// position and orientation of the stylus tip.
	// 50: 50ms MINIMUM update period.  No more than 20 arm messages
	// will be sent per second.
	nResult = ArmSetUpdateEx(ARM_FULL,50); 
	if ( ARM_SUCCESS != nResult ) 
	{ 
		AfxMessageBox( "Unable to set Update type for ArmDll32" ); 
		return FALSE; 
	} 
	return TRUE;
}

afx_msg LRESULT CArmCtrl::UpdateArm(WPARAM wParam, LPARAM lParam)
{
	int nRes;

	// The ARM_MESSAGE was sent to CArmCtrl.

	// Collect data and process it as the messages come in.
	// This is separate from displaying the data to avoid
	// bogging down the display updates for MSTestDlg with 
	// arm messages that could be coming as fast as every 
	// 50ms (the value passed in ArmSetUpdateEx).
	m_armData = *((arm_rec*)lParam);
	nRes = ArmGetJointAngles(ARM_DEGREES, m_arJointAngles);
	nRes = ArmGetTipPosition(&m_l3TipPosition);
	nRes = ArmGetTipOrientationUnitVector(&m_a3TipUnitVector);
	nRes = ArmGetButtonsState(&m_dwButtonsState);


	return lParam;
}

afx_msg LRESULT CArmCtrl::ConnectionLost(WPARAM wParam, LPARAM lParam)
{
	// The ARM_DISCONNECTED_MESSAGE was sent to CArmCtrl.
	// Notify the user and shut down Armdll32.
	AfxMessageBox( "Connection Lost!" ); 
	ArmEnd();

	return lParam;
}

void CArmCtrl::Disconnect()
{
	ArmEnd();
}

BOOL CArmCtrl::DestroyWindow() 
{
	ArmEnd(); // which calls ArmDisconnect();
	
	return CWnd::DestroyWindow();
}

arm_rec CArmCtrl::GetArmData()
{
	return m_armData;
}

void CArmCtrl::GetJointAngles(angle* pAngles)
{
	if(pAngles)
	{
		memcpy(pAngles, m_arJointAngles, sizeof(angle) * NUM_DOF);
	}
}

length_3D CArmCtrl::GetTipPosition()
{
	return m_l3TipPosition;
}

angle_3D CArmCtrl::GetTipOrientation()
{
	return m_a3TipUnitVector;
}

DWORD CArmCtrl::GetButtonsState()
{
	return m_dwButtonsState;
}

hci_result CANT_OPEN_error_handler(hci_rec *hci, hci_result condition)
{
	// Customizing this error handler will suppress the dialog boxes
	// requesting user input.  These dialog boxes remain in Armdll32
	// v2.0 to maintain backward compatibility.  However, any new 
	// applications created with Armdll32 v2.0 should suppress the
	// default dialog box.  Handle the error by providing a means
	// to check the ArmConnect return/error code and notify the user
	// of the error.

	// This error handler is merely a place holder that outputs the
	// encountered error message as debug info.  It's usefull in finding
	// out what condition caused the problem.  Returning the error 
	// message will cause Armdll32 to return the an appropriate error
	// code through ArmConnect.

	// This handler will be encountered if the serial port is already in use.
	OutputDebugString(condition);
	return condition;
}

hci_result NO_HCI_error_handler(hci_rec *hci, hci_result condition)
{
	// Customizing this error handler will suppress the dialog boxes
	// requesting user input.  These dialog boxes remain in Armdll32
	// v2.0 to maintain backward compatibility.  However, any new 
	// applications created with Armdll32 v2.0 should suppress the
	// default dialog box.  Handle the error by providing a means
	// to check the ArmConnect return/error code and notify the user
	// of the error.

	// This error handler is merely a place holder that outputs the
	// encountered error message as debug info.  It's usefull in finding
	// out what condition caused the problem.  Returning the error 
	// message will cause Armdll32 to return the an appropriate error
	// code through ArmConnect.

	// This handler will be encountered if the device is not powered or
	// is not connected.
	OutputDebugString(condition);
	return condition;
}

hci_result BAD_PORT_error_handler(hci_rec *hci, hci_result condition)
{
	// Customizing this error handler will suppress the dialog boxes
	// requesting user input.  These dialog boxes remain in Armdll32
	// v2.0 to maintain backward compatibility.  However, any new 
	// applications created with Armdll32 v2.0 should suppress the
	// default dialog box.  Handle the error by providing a means
	// to check the ArmConnect return/error code and notify the user
	// of the error.

	// This error handler is merely a place holder that outputs the
	// encountered error message as debug info.  It's usefull in finding
	// out what condition caused the problem.  Returning the error 
	// message will cause Armdll32 to return the an appropriate error
	// code through ArmConnect.

	// This handler will be encountered if the port does not exist or
	// is faulty.
	OutputDebugString(condition);
	return condition;
}

hci_result CANT_BEGIN_error_handler(hci_rec *hci, hci_result condition)
{
	// Customizing this error handler will suppress the dialog boxes
	// requesting user input.  These dialog boxes remain in Armdll32
	// v2.0 to maintain backward compatibility.  However, any new 
	// applications created with Armdll32 v2.0 should suppress the
	// default dialog box.  Handle the error by providing a means
	// to check the ArmConnect return/error code and notify the user
	// of the error.

	// This error handler is merely a place holder that outputs the
	// encountered error message as debug info.  It's usefull in finding
	// out what condition caused the problem.  Returning the error 
	// message will cause Armdll32 to return the an appropriate error
	// code through ArmConnect.

	// This handler will be encountered if the device is detected, but
	// communication for some reason is not possible.
	OutputDebugString(condition);
	return condition;
}
