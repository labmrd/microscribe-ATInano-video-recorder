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

  FILE:		FilterScribe.cpp : Data filtering utility class implementation.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#include "stdafx.h"
#include "FilterScribe.h"

CFilterScribe::CFilterScribe(): 
	CScribeThread( this )
{
	InitializeFilters();
}

CFilterScribe::~CFilterScribe()
{
}

bool CFilterScribe::SetDoApplyFilters( bool bProcess )
{
	m_bProcessFilter = bProcess;
	return m_bProcessFilter;
}

bool CFilterScribe::UpdateFilterData( void* armData )
{
	//-- Read data from ArmRec
	Point position;
	Vector orientation;

	if( !m_MicroScribe.GetButtons(m_buttons) )
		return false;

	if( !m_MicroScribe.GetPosition(position) )
		return false;

	if( !m_MicroScribe.GetOrientationAngles(orientation) )
		return false;

	float m_local_x = (float)position[X];
	float m_local_y = (float)position[Y];
	float m_local_z = (float)position[Z];
	float m_local_i = (float)orientation[X];
	float m_local_j = (float)orientation[Y];
	float m_local_k = (float)orientation[Z];

	// Quit if no filtering required.
	if( m_bProcessFilter )
	{
		if( m_bButtonFilter )
		{
			// Ignore anything other than buttons/pedals 1 & 2.
			if (m_buttons > 4)
				return false;
			m_ButtonFilter.DoDataFilter( this->m_buttons );
		}
	}

	m_x	= m_local_x;
	m_y	= m_local_y;
	m_z	= m_local_z;
	m_i	= m_local_i; 
	m_j = m_local_j;
	m_k	= m_local_k;

	return true;
}

BOOL CFilterScribe::InitializeFilters()
{
	// Indicate that the button filter is active.
	// This makes more sense when you consider adding other selectable filters.
	m_bButtonFilter = true;

#pragma message(__FILE__" - ToDo: if we want to be able to select the trigger button, we must supply the identified selection here instead of a hard-coded selection.")

	m_ButtonFilter.PutActivePedal( 1 );

	return TRUE;
}
