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

  FILE:		ButtonFilter.cpp : Buttion handling utility class implementation.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#include "stdafx.h"
#include "ButtonFilter.h"
#include "armdll32.h"

CButtonFilter::CButtonFilter()
{
	m_nActiveButton = ARM_BUTTON_1;
	m_bActiveOnDown = true;
	m_bActiveOnToggle = true;
	m_bConfigured   = true;
	m_nButtons = ARM_BUTTON_NONE; 
	m_nPreviousButtons = ARM_BUTTON_NONE;
}

CButtonFilter::~CButtonFilter()
{
}

bool CButtonFilter::IsConfigured()
{
	return m_bConfigured;
}

int CButtonFilter::GetActivePedal()
{
	return m_nActiveButton;
}

void CButtonFilter::PutActivePedal(int nVal)
{
	m_nActiveButton = nVal;
}

bool CButtonFilter::GetActiveOnDown()
{
	return  m_bActiveOnDown;
}

void CButtonFilter::PutActiveOnDown( bool bVal )
{
	(bVal)? m_bActiveOnDown = true : m_bActiveOnDown = false;
}

bool CButtonFilter::GetActiveOnToggle()
{
	return m_bActiveOnToggle;
}

void CButtonFilter::PutActiveOnToggle( bool bVal )
{	
	 (bVal)? m_bActiveOnToggle = true : m_bActiveOnToggle = false; 
}

bool CButtonFilter::DoDataFilter( int nButtons )
{
	if( m_bActiveOnToggle ) 
	{
		m_nButtons_new = nButtons;

		// Confirm button change
		if( m_nButtons_new == m_nPreviousButtons )
		{
			m_nPreviousButtons = m_nButtons_new;
			return false;
		}

		// up active and active button was pushed and is now released;
		if ( !m_bActiveOnDown 
			 && (m_nPreviousButtons != m_nActiveButton || m_nButtons_new == m_nActiveButton)
			 || m_nButtons_new == (ARM_BUTTON_1 + ARM_BUTTON_2)
			) 
		{
			m_nPreviousButtons = m_nButtons_new;
			return false;
		}
		
		// down active and current button is not active button or
		if ( m_bActiveOnDown && m_nActiveButton != m_nButtons_new ) 
		{
			m_nPreviousButtons = m_nButtons_new;
			return false;
		}
	}
	else
	{		
		// Not Activated by toggle of state but new button is not the active button
		if ( (m_bActiveOnDown && m_nButtons_new != m_nActiveButton) || (!m_bActiveOnDown && m_nButtons_new != ARM_BUTTON_NONE) )
		{
			m_nPreviousButtons = m_nButtons_new;
			return false;
		}
		
	}

	m_nButtons = m_nButtons_new;
	m_nPreviousButtons = m_nButtons_new;

	return true;
}
