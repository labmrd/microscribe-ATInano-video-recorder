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

  FILE:		ScribeThread.cpp : Implementation of the MicroScribe process
			thread class.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#include "stdafx.h" 
#include "windows.h" 
#include <comdef.h>
#include <windowsx.h>

#include "ScribeThread.h" 
#include "resource.h" 
#include "Probe_Mgr.h"
#include "FilterScribe.h"

extern CProbe_MgrApp theApp;

UINT gArmMessage = 0;
UINT gArmDisconnectMessage = 0;

CFilterScribe *gFilterScribe = NULL;

///////////////////////////////////////////////////////
// CScribeThread

CScribeThread::CScribeThread( CFilterScribe *filterScribe )
	: m_bRunning( false )
	, m_bThreadPaused( true )
	, m_handleThread( NULL )
	, m_hwndMicroscribe( NULL )
	, m_nPort( 0 )
	, m_nBaud( 0 )
{
	gFilterScribe = filterScribe;
}


CScribeThread::~CScribeThread()
{
	TerminateThread();
}


bool CScribeThread::Connect()
{
	if( NULL == m_hwndMicroscribe )
		return false;

	if ( !m_MicroScribe.Connect( m_hwndMicroscribe ) )
		return false;
	
	// With successful connection, this call only sets variables.
	(void)StartMonitor();
	
	return true;
}


bool CScribeThread::Disconnect()
{
	if ( !GetIsConnected() )
		return true;
	
	StopMonitor();
	m_MicroScribe.Disconnect();

	return true;	
}


bool CScribeThread::CreateThread()
{
	if ( m_bRunning ) 
		return false;	// Already running.

	DWORD lThreadId;
	DWORD lParameter;

	m_handleThread = ::CreateThread(
		NULL,			// SD
		8192,
		(LPTHREAD_START_ROUTINE)&CScribeThread::ThreadProc,
		&lParameter,	// Thread argument.
		0,				// Creation option.
		&lThreadId		// Thread identifier.
		);
	
	if ( m_handleThread == NULL )
		return false;

	SetThreadPriority( m_handleThread, THREAD_PRIORITY_TIME_CRITICAL );//THREAD_PRIORITY_HIGHEST);
	m_bRunning = true;

	return true;
}


bool CScribeThread::TerminateThread()
{
	if( !m_bRunning ) 
		return false;	// Nothing running.

	Disconnect();
	DestroyHiddenWindow();
	m_bRunning = false;

	// Exit thread.
	ULONG lExitCode = STILL_ACTIVE;
	while ( STILL_ACTIVE == lExitCode )
		GetExitCodeThread( m_handleThread, &lExitCode );

	return true;
}

bool CScribeThread::StartMonitor()
{
	// CreateThread() can be called independently.
	// If the thread is created, m_bRunning is set to true.
	if( !m_bRunning ) 
	{
		(void)CreateThread();
	}

	// If the thread is created or already running,
	// we are starting the monitor.
	m_bThreadPaused = !m_bRunning;

	// If the thread is running, then we have started the monitor.
	return m_bRunning;
}

bool CScribeThread::StopMonitor()
{
	m_bThreadPaused = true;
	return true;
} 

bool CScribeThread::CreateHiddenWindow()
{
	gArmDisconnectMessage = RegisterWindowMessage( _T(ARM_DISCONNECTED_MESSAGE) );
	gArmMessage = RegisterWindowMessage( _T(ARM_MESSAGE) );
	if ( !gArmMessage )
	{
		MessageBox( NULL, _T("Unable to register MicroScribe window message."), _T("Error"), MB_OK );
		return false;
	}

	m_hwndMicroscribe = CreateDialog( theApp.m_hInstance,MAKEINTRESOURCE(IDD_MICROSCRIBE ), NULL, (DLGPROC) WindowProc );
	
	if ( NULL == m_hwndMicroscribe )
	{
		MessageBox( NULL, _T("MicroScribe Window was not created."), _T("Error"), MB_OK );
		return false;
	}

	return true;
}


bool CScribeThread::DestroyHiddenWindow()
{
	SendMessage( m_hwndMicroscribe, WM_DESTROY, 0 ,0 );

	return true;
}


BOOL CALLBACK CScribeThread::WindowProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if ( msg == gArmDisconnectMessage )	// User disconnected the MicroScribe or power was lost.
	{
		// Disconnect the device.
		gFilterScribe->m_MicroScribe.Disconnect();

		// Try to reconnect.
		if ( IDRETRY == gFilterScribe->m_pCWnd->MessageBox( _T(" The device was manually disconnected. Do you want to reconnect? "), _T("MicroScribe Probe Manager"),MB_RETRYCANCEL ) )
			gFilterScribe->Connect();

		// If failure, show the status in the main window.
		if ( !gFilterScribe->GetIsConnected() )
		{
			gFilterScribe->m_pCWnd->GetDlgItem( IDC_STATIC_CONNECTED )->ShowWindow( true );
			gFilterScribe->m_pCWnd->GetDlgItem( IDC_STATIC_CONNECTED )->SetWindowText( _T("MicroScribe not connected") );
		}
	}
	else if ( msg == gArmMessage )	// Process a MicroScribe message.
	{
		if ( lParam && gFilterScribe )
		{
			// Make sure we cannot enter this section simultaneously.
			static bool bEnter = false;
			if( bEnter )
				return TRUE;

			bEnter = true;

			// 1 - First read data from ArmDLL32/ArmDLL64.
			gFilterScribe->m_MicroScribe.UpdateData( (void*)lParam );

			// 2 - Update filtersScribe positions, angles and buttons.
			bool gotNewPoint = gFilterScribe->UpdateFilterData( (void*)lParam );

			// 3 - Call OnData for client.
			if ( gotNewPoint )
				gFilterScribe->OnData();

			// Mark safe to enter.
			bEnter = false;
		}
	}
	else switch ( msg ) // Process other messages.
	{
		case WM_INITDIALOG:
			return TRUE;	// Our invisable window has no initialization.
		break; 

		case WM_CLOSE: 
		case WM_DESTROY:			
			gFilterScribe->m_MicroScribe.Disconnect();
			PostQuitMessage( 0 ); 
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

DWORD WINAPI CScribeThread::ThreadProc( LPVOID lParam )
{
	gFilterScribe->CreateHiddenWindow();
	gFilterScribe->Connect();	

	while ( gFilterScribe->m_bRunning )
	{
		if( !gFilterScribe->m_bThreadPaused )
		{			
			MSG msg;
			while ( GetMessage( &msg, NULL, 0, 0 ) )
			{
				if ( !IsDialogMessage( gFilterScribe->m_hwndMicroscribe, &msg ) ) 
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}

			Sleep( 20 );
		}
		else
			Sleep( 200 );
	}
	gFilterScribe->m_MicroScribe.Disconnect();
	gFilterScribe->DestroyHiddenWindow();

	ExitThread( 0 );
	
	return 1;	
}

bool CScribeThread::PutSN( CString sSN )
{
	return m_MicroScribe.SetSN( sSN );
}


int CScribeThread::GetInterval()
{
	return m_MicroScribe.GetInterval();
}


int CScribeThread::PutInterval( int newVal )
{
	return m_MicroScribe.SetInterval( newVal );
}


bool CScribeThread::GetIsConnected( void )
{
	return m_MicroScribe.GetIsConnected();
}

bool CScribeThread::GetIsMetric( void )
{
	return m_MicroScribe.GetMetricFlag();
}

void CScribeThread::PutIsMetric( bool bVal )
{
	m_MicroScribe.SetMetricFlag( bVal );
}

bool CScribeThread::GetID( BSTR *pVal )
{
	if ( !GetIsConnected() )
		return false;

	_bstr_t bOut;
	bOut = m_MicroScribe.GetID();
	*pVal = bOut.copy();

	return true;
}

bool CScribeThread::GetModelName( BSTR *pVal )
{
	if ( !GetIsConnected() )
		return false;
		
	_bstr_t bOut;
	bOut = m_MicroScribe.GetModelName();
	*pVal = bOut.copy();

	return true;
}

bool CScribeThread::GetProductName( BSTR *pVal )
{
	if ( !GetIsConnected() )
		return false;
	
	_bstr_t bOut;
	bOut = m_MicroScribe.GetProductName();
	*pVal = bOut.copy();
	
	return true;
}

bool CScribeThread::GetSerialNumber( BSTR *pVal )
{
	if ( !GetIsConnected() )
		return false;
		
	_bstr_t bOut;
	bOut = m_MicroScribe.GetSerialNumber();
	*pVal = bOut.copy();

	return true;
}

bool CScribeThread::GetVersion( BSTR *pValDriver, BSTR *pValFirmware )
{
	if ( !GetIsConnected() )
		return false;
	
	_bstr_t bOutDriver;
	_bstr_t bOutFirmware;
	char strDriver[32], strFirmware[32]; 
	m_MicroScribe.GetVersion( strDriver, strFirmware );
	bOutDriver = strDriver;
	bOutFirmware = strFirmware;
	*pValDriver	= bOutDriver.copy();
	*pValFirmware = bOutFirmware.copy();

	return true;
}

bool CScribeThread::GetComment( BSTR *pVal )
{
	if ( !GetIsConnected() )
		return false;
		
	_bstr_t bOut;
	bOut = m_MicroScribe.GetComment();
	*pVal = bOut.copy();

	return true;
}


void CScribeThread::PutOffsetTipDelta( float newVal )
{
	m_MicroScribe.SetOffsetTipDelta( newVal );

	return;
}

int CScribeThread::GetDeviceStatus()
{
	return m_MicroScribe.GetDeviceStatus();
}

void CScribeThread::GetHomePosition( int *homePos ) 
{
	m_MicroScribe.GetHomePosition( homePos );
}