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

  FILE:		MSConsol.c : C console program that uses the MicroScribe
			ArmDLL interface.

  PURPOSE:	This program shows the proper use of the MicroScribe  
			ArmDLL API functions to poll data from the digitizer.
   
  By Valentino Felipe, May 2002
     Tom Welsh, Dec 2011

**********************************************************************/

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <armdll32.h>

int CheckButtonsState(void);

void main(void)
{
	// Flag to tell us when we're ready to exit
	int done = 0;
	char wait = 0;

	// Tip position
	length_3D l3TipPosition;

	/* --- Initialization --- */

	// The ArmStart function prepares ArmDll32 for operation. 
	// Internally, it spawns a read thread used to retireve 
	// data from the device.
	int nResult = ArmStart(NULL);
	if ( ARM_SUCCESS != nResult ) 
	{ 
#ifdef _M_X64
		printf("Unable to Start ArmDll64\n");
#else
		printf("Unable to Start ArmDll32\n");
#endif
		exit(1); 
	} 

	/* (Optional) disables dialog box pop-ups.  You can create
	 *   your own error handlers and install them instead, but that
	 *   is an 'advanced' feature.  See the Armdll32 v2.0 reference
	 *   for more details.
	 */
	nResult = ArmSetErrorHandlerFunction(NO_HCI_HANDLER, NULL);
	nResult = ArmSetErrorHandlerFunction(BAD_PORT_HANDLER, NULL);
	nResult = ArmSetErrorHandlerFunction(CANT_OPEN_HANDLER, NULL);
	nResult = ArmSetErrorHandlerFunction(CANT_BEGIN_HANDLER, NULL);

	/* Greet the user */
	printf("\nRevware MicroScribe ArmDLL sample program for C.\n");
	printf("Press both buttons at once to exit.\n");
	printf("---------------------------------\n\n");

	/* Time to connect to the hardware */
	// The USB port is always checked first.  The port and baud
	// values are only used if the device is not found on a USB
	// port.  Passing 0,0 will check all serial ports.
	printf("Autodectecting connection port\n");
	nResult = ArmConnect(0,0); 
	if ( ARM_SUCCESS != nResult ) 
	{ 
		printf( "Unable to Connect to a MicroScribe\n" ); 
		ArmEnd();
		exit(1); 
	} 
	else
	{
		int nRes;
		char buffer1[MAX_PATH], buffer2[MAX_PATH];

		nRes = ArmGetProductName(buffer1, MAX_PATH);
		if(nRes == ARM_SUCCESS)
			printf("Product Name:     %s\n", buffer1);

		nRes = ArmGetModelName(buffer1, MAX_PATH);
		if(nRes == ARM_SUCCESS)
			printf("Model Name:       %s\n", buffer1);
		nRes = ArmGetSerialNumber(buffer1, MAX_PATH);
		if(nRes == ARM_SUCCESS)
			printf("Serial Number:    %s\n", buffer1);
		nRes = ArmGetVersion(buffer1, buffer2, MAX_PATH);
		if(nRes == ARM_SUCCESS)
		{
			printf("ArmDLL Version: %s\n", buffer1);
			printf("Firmware Version: %s\n", buffer2);
		}

		printf("Press \'c\' to continue...");
		while(wait != 'c')
		{
			wait= _getch();
		}
	}

	// ARM_FULL: ArmDll32 calculates and updates the Cartesian 
	// position and roll-pitch-yaw orientation of the stylus tip.
	// 50: 50ms MINIMUM update period.  Armdll32 will update data no 
	// more than 20 times per second.  Updates only occur when device
	// status has changed.  This can be set to a minimum of 3ms which
	// would mean that data will be updated no more than 333 times per
	// second.  See Armdll32 v2.0 programmer's reference for more details.
	nResult = ArmSetUpdateEx(ARM_FULL,50); 
	if ( ARM_SUCCESS != nResult ) 
	{ 
		printf( "Unable to set Update type for ArmDll\n" ); 
		ArmDisconnect();
		ArmEnd();
		exit(1); 
	}

	/* Do this until there is an error or a button is pressed */
	while (!done)
	{
		/* Update the stylus position */
		nResult = ArmGetTipPosition(&l3TipPosition);

		if(ARM_NOT_CONNECTED == nResult)
		{
			printf("Connection lost!\n");
			done = 1;
		}

		/* Print stylus coordinates */
		printf("X = %f, Y = %f, Z = %f\t",
			l3TipPosition.x, l3TipPosition.y,
			l3TipPosition.z);

		/* If a button is being pressed, we want to exit */
		if (CheckButtonsState()) done = 1;
	}

	/* Time to end this session.*/
	ArmDisconnect();
	ArmEnd();

	printf("Session ended\n");
}


int CheckButtonsState()
{
	static unsigned long dwLastButtonsState = 0;
	unsigned long dwCurrentButtonsState;
	int bButton1Up = 0, bButton1Down = 0;
	int bButton2Up = 0, bButton2Down = 0;

	ArmGetButtonsState(&dwCurrentButtonsState);
	printf("ButtonsState = 0x%02xh\n", dwCurrentButtonsState);

	if( !(dwLastButtonsState & ARM_BUTTON_1) && (dwCurrentButtonsState & ARM_BUTTON_1) )
	{
		// Look for the transition from inactive to active
		printf("Button 1 Down\t");
		bButton1Down = 1;
	}
	else if((dwLastButtonsState & ARM_BUTTON_1) && !(dwCurrentButtonsState & ARM_BUTTON_1))
	{
		// Look for the transition from active to inactive
		printf("Button 1 Up\t");
		bButton1Up = 1;
	}

	if( !(dwLastButtonsState & ARM_BUTTON_2) && (dwCurrentButtonsState & ARM_BUTTON_2) )
	{
		// Look for the transition from inactive to active
		printf("Button 2 Down");
		bButton2Down = 1;
	}
	else if((dwLastButtonsState & ARM_BUTTON_2) && !(dwCurrentButtonsState & ARM_BUTTON_2))
	{
		// Look for the transition from active to inactive
		printf("Button 2 Up");
		bButton2Up = 1;
	}

	if(bButton1Down || bButton1Up || bButton2Down || bButton2Up)
	{
		printf("\n");
		dwLastButtonsState = dwCurrentButtonsState;
	}

	// For our purposes we are looking for any dual button events.
	return dwCurrentButtonsState == 3;
}