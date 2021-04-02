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

  FILE:		Preferences.cpp : Persistent data class implementation.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#include "StdAfx.h"
#include "Preferences.h"
#include "Probe_Mgr.h"

// The one and only CPreferences object.
// For managing persistent user preferences.
CPreferences thePrefs;

extern CProbe_MgrApp theApp;
const TCHAR* pszSection = _T("MicroScribe");
const TCHAR* pszFlag = _T("CustomTipFlag");
const TCHAR* pszLength = _T("CustomTipLength");
const TCHAR* pszMetric = _T("Metric");

CPreferences::CPreferences(void)
	: m_bOffsetTipFlag( 0 )
	, m_fOffsetTipLength( 0.0 )
	, m_bMetric( false )
{
}


CPreferences::~CPreferences(void)
{
}


void CPreferences::Load( void )
{
	// Get flag indicating use of an offest length tip.
	m_bOffsetTipFlag = theApp.GetProfileInt( pszSection, pszFlag, 0 ) ? true : false;

	// Get the length of the offest length tip, if present.
	CString strValue = _T("0.0");
	strValue = theApp.GetProfileString( pszSection, pszLength, (LPCTSTR)strValue );
	m_fOffsetTipLength = (float)_ttof((LPCTSTR)strValue);

	// Get flag indicating use of millimeters.
	m_bMetric = theApp.GetProfileInt( pszSection, pszMetric, 0 ) ? true : false;
}


void CPreferences::Save( void )
{
	// Set flag indicating use of an offest length tip.
	theApp.WriteProfileInt( pszSection, pszFlag, (int)m_bOffsetTipFlag );

	// Set the length of the offest length tip.
	CString strValue;
	strValue.Format(_T("%0.4f"), m_fOffsetTipLength );
	theApp.WriteProfileString( pszSection, pszLength, (LPCTSTR)strValue );

	// Set flag indicating use of an offest length tip.
	theApp.WriteProfileInt( pszSection, pszMetric, (int)m_bMetric );
}
