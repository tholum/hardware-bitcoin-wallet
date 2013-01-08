/** \file main.c
  *
  * \brief Entry point for hardware Bitcoin wallet.
  *
  * This file is licensed as described by the file LICENCE.
  */

#include <stdint.h>
#include <string.h>
#include <p32xxxx.h>
#include "usb_hal.h"
#include "usb_standard_requests.h"
#include "usb_callbacks.h"
#include "usb_hid_stream.h"
#include "pic32_system.h"
#include "serial_fifo.h"
#include "../hwinterface.h"

/** This will be called whenever an unrecoverable error occurs. This should
  * not return. */
void usbFatalError(void)
{
	disableInterrupts();
#ifdef PIC32_STARTER_KIT
	PORTDSET = 1; // turn on red LED
#else
	PORTDSET = 0x10; // turn on red LED
#endif // #ifdef PIC32_STARTER_KIT
	while (1)
	{
		// do nothing
	}
}

/** Entry point. This is the first thing which is called after startup code.
  * This never returns. */
int main(void)
{
	uint8_t mode;
	uint8_t counter;

	disableInterrupts();
	pic32SystemInit();
	usbInit();
	usbHIDStreamInit();
	usbDisconnect(); // just in case
	usbSetupControlEndpoint();
	restoreInterrupts(1);
	// All USB-related modules should be initialised before
	// calling usbConnect().
	usbConnect();
	mode = streamGetOneByte();
	counter = 0;
	while (1)
	{
		if ((mode == 'g') || (mode == 'i') || (mode == 'j'))
		{
			if (mode == 'i')
			{
				delayCycles(3600000); // pretend to be doing some processing
			}
			else if (mode == 'j')
			{
				delayCycles(360000000); // pretend to be doing lots of processing
			}
			// Expect data to be an incrementing sequence. This is designed
			// to expose any out-of-order cases.
			if (streamGetOneByte() != counter)
			{
				usbFatalError();
			}
			counter++;
		}
		else if ((mode == 'p') || (mode == 't') || (mode == 'x'))
		{
			if (mode == 't')
			{
				delayCycles(3600000); // pretend to be doing some processing
			}
			else if (mode == 'x')
			{
				delayCycles(360000000); // pretend to be doing lots of processing
			}
			// Send data which is an incrementing sequence. This is designed
			// to expose any out-of-order cases.
			streamPutOneByte(counter++);
		}
		else if (mode == 'r')
		{
			// Reply, or loopback mode. This tests simultaneous sending and
			// receiving.
			streamPutOneByte(streamGetOneByte());
		}
		else
		{
			usbFatalError();
		}
	}
}
