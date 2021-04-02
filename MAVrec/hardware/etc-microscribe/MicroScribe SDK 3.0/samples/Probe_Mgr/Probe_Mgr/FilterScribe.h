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

  FILE:		FilterScribe.h : Data filtering utility class definition.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#pragma once

#include "ScribeThread.h"
#include "ButtonFilter.h"

class CFilterScribe : public CScribeThread
{
public:
	CFilterScribe();
	~CFilterScribe();

	bool UpdateFilterData(void *);
	bool SetDoApplyFilters(bool);

public:
	CButtonFilter  	m_ButtonFilter; 
	bool			m_bButtonFilter;

	CWnd * m_pCWnd;

	float	m_x, m_y, m_z;
	float	m_i, m_j, m_k;
	int		m_buttons;
	bool	m_extendedData;		// To track when IJK is to be supplied.

protected:
	bool	m_bProcessFilter;

	BOOL InitializeFilters();

};
