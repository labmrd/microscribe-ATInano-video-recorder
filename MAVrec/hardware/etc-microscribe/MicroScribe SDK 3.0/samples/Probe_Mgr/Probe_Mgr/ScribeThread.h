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

  FILE:		ScribeThread.h : Definition of the MicroScribe process
			thread class.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#pragma once

#include "MicroScribe.h"

#define THREADFUNC		DWORD WINAPI

class CFilterScribe;

class CScribeThread
{
public:
	CScribeThread( CFilterScribe* );
	~CScribeThread();

public:
	bool StartMonitor();
	bool StopMonitor();
	bool TerminateThread();
	bool CreateThread();
	bool Connect();
	bool Disconnect();
	bool PutSN( CString sSN );
	int  GetInterval();
	int  PutInterval( int newVal );
	bool GetIsConnected();
	bool GetIsMetric();
	void PutIsMetric( bool bVal );
	bool GetID( BSTR *pVal );
	bool GetModelName( BSTR *pVal );
	bool GetProductName( BSTR *pVal );
	bool GetSerialNumber( BSTR *pVal );
	bool GetVersion( BSTR *pValDriver, BSTR *pValFirmware );
	bool GetComment( BSTR *pVal );
	void PutOffsetTipDelta( float newVal );
	int  GetDeviceStatus();
	void GetHomePosition( int *homePos );

protected:
	CMicroScribe m_MicroScribe;

	bool	m_bRunning;
	bool	m_bThreadPaused;
	HANDLE  m_handleThread;
	long	m_nPort, m_nBaud;
	HWND	m_hwndMicroscribe;

protected:
	bool CreateHiddenWindow();
	bool DestroyHiddenWindow();
	static DWORD WINAPI ThreadProc( LPVOID lParam );
	static BOOL CALLBACK  WindowProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	virtual bool OnData( )=0;
};
