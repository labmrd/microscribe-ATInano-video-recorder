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

  FILE:		Point.h : Point object class definition/implementation.

  PURPOSE:	Example program shows the proper use of the ArmDLL API 
			functions for communicating with the MicroScribe digitizer.
   
  By Tom Welsh, Nov 2011

/////////////////////////////////////////////////////////////////////////*/

#pragma once

#define X 0
#define Y 1
#define Z 2

class Vector
{
public:
	Vector() {}
	Vector(const float v[3]) { setValue(v); }
	Vector(const double v[3]) { setValue(v); }
	Vector(double x, double y, double z) { setValue(x, y, z); }

	double&       operator[](int i)       { return comp[i]; }
	const double& operator[](int i) const { return comp[i]; }

	double       *getValue()       { return comp; }
	const double *getValue() const { return comp; }

	void setValue(const float v[3])
	{
		comp[X] = v[X]; comp[Y] = v[Y]; comp[Z] = v[Z];
	}

	void setValue(const double v[3])
	{
		comp[X] = v[X]; comp[Y] = v[Y]; comp[Z] = v[Z];
	}

	void setValue(double x, double y, double z)
	{
		comp[X] = x; comp[Y] = y; comp[Z] = z;
	}

protected:
	double comp[3];
 };



class Point : public Vector
{
public:
	Point() {}
	Point(const float p[3]) : Vector(p) {} 
	Point(const double p[3]) : Vector(p) {}
	Point(double x, double y, double z) : Vector(x, y, z) {}
};
