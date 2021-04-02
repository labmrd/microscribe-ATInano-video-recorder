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

  FILE:		MicroScribe.cpp : MicroScribe interface class implementation.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#include "stdafx.h"
#include "MicroScribe.h"
#include "ArmDll32.h"
#include "Probe_Mgr.h"

#define MY_ARM_ERROR_MESSAGE (WM_APP+1)
#define MY_ARM_STATE_MESSAGE (WM_APP+2)

extern CProbe_MgrApp theApp;

///////////////////////////////////////////////////////
// CMicroScribe

CMicroScribe::CMicroScribe()
	: m_sSN( _T("") )			// Default is to connect to any SN.
	, m_bConnected( false )		// Initially not connected.
	, m_fTipDelta( 0.0 )		// Initialize with no offset to standard probe length.
	, m_nInterval( 50 )			// Initialize to 50 ms data interval (20 points per sec).
	, m_bMetricFlag( false )	// MicroScribe default units is inches.
{
	// Make sure string holders in the arm rec are set to 0.
	m_arm.hci.serial_number[0] = 0;
	m_arm.hci.serial_number[0] = 0;
	m_arm.hci.product_name[0] = 0;
	m_arm.hci.product_id[0] = 0;
	m_arm.hci.model_name[0] = 0;
	m_arm.hci.comment[0] = 0;
	m_arm.hci.param_format[0] = 0;
	m_arm.hci.version[0] = 0;
}


CMicroScribe::~CMicroScribe()
{
	// Disconnect if connected.
	if ( m_bConnected )
		Disconnect();
}	


hci_result customErrorFunc_badport (struct hci_rec *hci, hci_result condition)
{
	HWND mainW = *theApp.m_pMainWnd;
	::PostMessage( mainW, MY_ARM_ERROR_MESSAGE, 0, 0 );
	return 0;
}

hci_result customErrorFunc_cantopen (struct hci_rec *hci, hci_result condition)
{
	HWND mainW = *theApp.m_pMainWnd;
	::PostMessage( mainW, MY_ARM_ERROR_MESSAGE, 0, 1 );
	return 0;
}

hci_result customErrorFunc_nohci (struct hci_rec *hci, hci_result condition)
{
	HWND mainW = *theApp.m_pMainWnd;
	::PostMessage( mainW, MY_ARM_ERROR_MESSAGE, 0, 2 );
	return 0;
}

hci_result customErrorFunc_cantbegin (struct hci_rec *hci, hci_result condition)
{
	HWND mainW = *theApp.m_pMainWnd;
	::PostMessage( mainW, MY_ARM_ERROR_MESSAGE, 0, 3 );
	return 0;
}

hci_result customErrorFunc_timedout (struct hci_rec *hci, hci_result condition)
{
	HWND mainW = *theApp.m_pMainWnd;
	::PostMessage( mainW, MY_ARM_ERROR_MESSAGE, 0, 4 );
	return 0;
}

hci_result customErrorFunc_badpacket (struct hci_rec *hci, hci_result condition)
{
	HWND mainW = *theApp.m_pMainWnd;
	::PostMessage( mainW, MY_ARM_ERROR_MESSAGE, 0, 5 );
	return 0;
}


bool CMicroScribe::Connect( HWND hwnd )
{
	if( m_bConnected )
		return true;

	HWND mainW = *theApp.m_pMainWnd;

	// Connect and update status.
	if( 0 != ArmStart( hwnd ))
	{	//error
		// Set the connection indicator.
		// Values for last Argument: (These are arbitrarily chosen for this applicaiton.)
		// 0 for neutral/disconnected state (gray)
		// 1 for connected state (green)
		//-1 for error state (red)
		::PostMessage( mainW, MY_ARM_STATE_MESSAGE, 0, -1 );

		return false;
	}

	ArmSetErrorHandlerFunction( BAD_PORT_HANDLER, &customErrorFunc_badport );
	ArmSetErrorHandlerFunction( CANT_OPEN_HANDLER, &customErrorFunc_cantopen );
	ArmSetErrorHandlerFunction( NO_HCI_HANDLER, &customErrorFunc_nohci );
	ArmSetErrorHandlerFunction( CANT_BEGIN_HANDLER, &customErrorFunc_cantbegin );
	ArmSetErrorHandlerFunction( TIMED_OUT_HANDLER, &customErrorFunc_timedout );
	ArmSetErrorHandlerFunction( BAD_PACKET_HANDLER, &customErrorFunc_badpacket );

#if(0)
	// --------------------------------------------------------
	// A method for identifying which MicroScribe to connect to by its serial number.
	// To connect to the first MicroScribe found regardles of serial number delete the file.
	// or supply a NULL pointer as the SN argument.
	char serialNumber[10] = "";
	char *sn = NULL;	// We want to pass a null pointer for the SN if none is supplied.

	if( FILE* SNfile = fopen("SNfile.txt", "r") )
	{
		fscanf(SNfile, "%s", serialNumber);
		fclose(SNfile);
		size_t len = strnlen(serialNumber, 10);
		if (  len > 4 )
		{
			sn = serialNumber;
			// Ignore leading "R" if present.
			if ( sn[0] == 'R' || sn[0] == 'r' )
				sn++;
			// Make sure that the string is null terminated.
			// A serial number is 5 numbers.
			// We are not checking to make sure 5 numbers are supplied, but could here.
			sn[5] = NULL;
		}
	}
#endif

	char *sn = NULL;	// We want to pass a null pointer for the SN if none is supplied.
	CT2A serialNumber(m_sSN);

	// If a serial number was supplied, use it.
	if ( !m_sSN.IsEmpty() )
		sn = (char *)serialNumber;

	// You can specifically set the port and baud rate for the connection if a serial device, but it is better to allow ArmDLL to automatically search and connect.
	// Pass NULL for sn or use ArmConnect( 0, 0L ) to connect to the first MicroScribe found regardles ofthe SN.
	if ( ArmConnectSN( 0, 0L, sn ) != ARM_SUCCESS )
	{
		ArmEnd();

		// Set the connection indicator.
		// Values for last Argument: (These are arbitrarily chosen for this applicaiton.)
		// 0 for neutral/disconnected state (gray)
		// 1 for connected state (green)
		//-1 for error state (red)
		::PostMessage( mainW, MY_ARM_STATE_MESSAGE, 0, -1 );

		return false;
	}

	if(0 != ArmSetUpdateEx( ARM_FULL, m_nInterval ))
		return false;

	m_bConnected = true;

	// Set the connection indicator.
	// Values for last Argument: (These are arbitrarily chosen for this applicaiton.)
	// 0 for neutral/disconnected state (gray)
	// 1 for connected state (green)
	//-1 for error state (red)
	::PostMessage( mainW, MY_ARM_STATE_MESSAGE, 0, 1 );

	return true;
}


void CMicroScribe::Disconnect()
{
	//ArmEnd still needs to be called to make the other threads quit and
	//to kill the reconnect window if exists
	if( m_bConnected )
	{
		ArmDisconnect();

		m_bConnected = false;
		HWND mainW = *theApp.m_pMainWnd;

		// Set the connection indicator.
		// Values for last Argument: (These are arbitrarily chosen for this applicaiton.)
		// 0 for neutral/disconnected state (gray)
		// 1 for connected state (green)
		//-1 for error state (red)
		::PostMessage( mainW, MY_ARM_STATE_MESSAGE, 0, 0 );
	}

	ArmEnd();
}

	
bool CMicroScribe::UpdateData( void* armData )
{
	//TRACE0("CMicroScribe::UpdateData\n");

	if( !m_bConnected )
		return false;

	if (!armData)
		return false; 

	m_arm = * ((arm_rec*)armData);

	return true;
}		


///////////////////////////////////////////////////////
// CMicroScribe State Accessors

bool CMicroScribe::GetButtons( int& buttons )
{
	// Check if not connected.
	if ( !m_bConnected )
		return false;
	
	// Set the value and return.
	buttons = m_arm.hci.buttons;

	return true;
}


bool CMicroScribe::GetPosition( Point& pos )
{
	// Check if not connected.
	if ( !m_bConnected )
		return false;
	
	// Set the values and return.
	pos.setValue( m_arm.stylus_tip.x, m_arm.stylus_tip.y, m_arm.stylus_tip.z );

	return true;
}


bool CMicroScribe::GetOrientationAngles( Vector& eulerAngles )
{
	// Check if not connected.
	if ( !m_bConnected )
		return false;
	
	// Set the values and return.
	eulerAngles.setValue( m_arm.stylus_dir.x, m_arm.stylus_dir.y, m_arm.stylus_dir.z );

	return true;
}


bool CMicroScribe::GetOrientationDirection( Vector& direction )
{
	// Check if not connected.
	if ( !m_bConnected )
		return false;
	
	// Extract the value and return.
	direction.setValue( m_arm.T[0][2], m_arm.T[1][2], m_arm.T[2][2] );

	return true;
}

	
bool CMicroScribe::GetMetricFlag()
{
	return m_bMetricFlag;
}


void CMicroScribe::SetMetricFlag( bool metric )
{
	m_bMetricFlag = metric;

	if ( m_bMetricFlag )
		ArmSetLengthUnits( ARM_MM );
	else
		ArmSetLengthUnits( ARM_INCHES );
}

bool CMicroScribe::SetSN( CString sSN )
{
	// Delete leading adn trailing whitespace.
	sSN.Trim();

	// Delete the leading R if provided.
	sSN.MakeUpper();
	if ( sSN.GetAt(0) == 'R' )
		sSN.TrimLeft( _T("R") );

	if ( sSN.IsEmpty() )				// An empty string is the way to clear the current SN.
	{
		m_sSN.Empty();
	}	
	else if ( sSN.GetLength() != 5 )	// SNs are 5 numbers long.
	{
		m_sSN.Empty();

		return false;
	}

	// SNs are comprised of numbers only.
	// But we are not checking for numbers.
	// If the SN is not valid at this point, the MS will simply not be found.
	m_sSN = sSN;

	return true;
}


int	CMicroScribe::GetInterval()
{
	return m_nInterval;
}


int CMicroScribe::SetInterval( int interval )
{
	if ( interval < MINIMUM_PERIOD_UPDATE )
		m_nInterval = MINIMUM_PERIOD_UPDATE;
	else if ( interval > MAXIMUM_PERIOD_UPDATE )
		m_nInterval = MAXIMUM_PERIOD_UPDATE;
	else
		m_nInterval = interval;

	return m_nInterval;
}


void CMicroScribe::SetOffsetTipDelta( float delta )
{
	m_fTipDelta = delta;

	ArmCustomTip( m_fTipDelta );
}


float CMicroScribe::GetOffsetTipDelta( void )
{
	return m_fTipDelta;
}

	
bool CMicroScribe::GetIsConnected()
{
	return m_bConnected;
}


void CMicroScribe::GetHomePosition(int * homePos) 
{
	memcpy(homePos, m_arm.hci.home_pos, sizeof(int)*NUM_ENCODERS);
}


/******************************************************************************/
//
//CMicroScribe Information Accessors
//
/******************************************************************************/

const char* CMicroScribe::GetProductName()
{
	return m_arm.hci.product_name;
}


const char* CMicroScribe::GetID()
{
	return m_arm.hci.product_id;
}


const char* CMicroScribe::GetModelName()
{
	return m_arm.hci.model_name;
}


const char* CMicroScribe::GetSerialNumber()
{
	return m_arm.hci.serial_number;
}


const char* CMicroScribe::GetComment()
{
	return m_arm.hci.comment;
}


const char* CMicroScribe::GetParameterFormat()
{
	return m_arm.hci.param_format;
}


void  CMicroScribe::GetVersion(char *strDriverVersion, char *strFirmwareVersion)
{

	ArmGetVersion(strDriverVersion, strFirmwareVersion, 32);

	return;
}

int  CMicroScribe::GetDeviceStatus()
{
	int ret = -1;
	device_status devStatus;
	ArmGetDeviceStatus	(&devStatus);
	if (devStatus.status & ARM_CONNECTED)
	{
		if ( devStatus.status & ARM_USING_SERIAL_PORT )
			ret = 1;//SERIAL
		if ( devStatus.status & ARM_USING_USB_PORT )
			ret = 2;//USB
	}

	return ret;
}
