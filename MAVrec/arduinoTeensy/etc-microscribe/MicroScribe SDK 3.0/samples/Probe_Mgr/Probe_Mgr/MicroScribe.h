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

  FILE:		MicroScribe.h : MicroScribe interface class definition.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#pragma once

#include "ArmDll32.h"
#include "Point.h"

//
//MicroScribe class definition
//
class CMicroScribe
{

public:
	CMicroScribe();
	~CMicroScribe();
	
	// Connect / Disconnect / Update
	bool	Connect( HWND );
	void	Disconnect();
	bool	UpdateData( void * );
		
	// Field Accessors

	// MicroScribe State
	bool	GetButtons( int& buttons );
	bool	GetPosition( Point& pos );
	bool	GetOrientationAngles( Vector& eulerAngles ); 
	bool	GetOrientationDirection( Vector& direction ); 

	bool	GetMetricFlag();
	void	SetMetricFlag( bool metric );

	bool	SetSN( CString sSN );

	int		GetInterval();
	int		SetInterval( int period );

	float	GetOffsetTipDelta( void );
	void	SetOffsetTipDelta( float delta );

	bool	GetIsConnected();
	//bool	GetIsHomed();
	void	GetHomePosition(int *homePos);

	// MicroScribe Information
	const char*	GetProductName();
	const char*	GetID();
	const char*	GetModelName();
	const char*	GetSerialNumber();
	const char*	GetComment();
	const char*	GetParameterFormat();
	void  GetVersion(char *strDriverVersion, char *strFirmwareVersion);
	int   GetDeviceStatus();
	
protected:
	// Implementation Fields
	bool	m_bConnected;	// Is the MicroScribe connected?
	arm_rec	m_arm;			// The arm structure.
	bool	m_bMetricFlag;	// Whether to report metric or not.
	float	m_fTipDelta;	// The change in tip length (in the current units).
	CString m_sSN;			// The device SN to connect to, if specified.
	int		m_nInterval;	// The data communications rate, in ms.
};
