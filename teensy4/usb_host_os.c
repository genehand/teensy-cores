/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2024 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "usb_host_os.h"

// USB Descriptor types (from USB spec)
#define USB_DESC_DEVICE        0x01
#define USB_DESC_CONFIGURATION 0x02
#define USB_DESC_STRING        0x03
#define USB_DESC_BOS           0x0F

// Track the first two non-device descriptor requests
static uint8_t desc_req_buf[2];
static int desc_req_idx = 0;
static usb_host_os_t detected_os = USB_HOST_OS_UNKNOWN;

void usb_host_os_reset(void)
{
	desc_req_idx = 0;
	detected_os = USB_HOST_OS_UNKNOWN;
}

void usb_host_os_desc_request(uint8_t desc_type)
{
	// Skip Device Descriptor requests - all OSes request this first
	if (desc_type == USB_DESC_DEVICE) {
		return;
	}

	// Only track BOS, Configuration, and String descriptors
	if (desc_type != USB_DESC_BOS && 
	    desc_type != USB_DESC_CONFIGURATION && 
	    desc_type != USB_DESC_STRING) {
		return;
	}

	// Skip redundant requests of the same type
	if (desc_req_idx == 1 && desc_req_buf[0] == desc_type) {
		return;
	}

	// Record descriptor request order (only need first 2)
	if (desc_req_idx < 2) {
		desc_req_buf[desc_req_idx++] = desc_type;
	}

	// Detect OS once we have 2 descriptor requests
	if (desc_req_idx >= 2 && detected_os == USB_HOST_OS_UNKNOWN) {
		if (desc_req_buf[0] == USB_DESC_BOS && 
		    desc_req_buf[1] == USB_DESC_CONFIGURATION) {
			// Linux: BOS -> Config
			detected_os = USB_HOST_OS_LINUX;
		} else if (desc_req_buf[0] == USB_DESC_CONFIGURATION && 
		           desc_req_buf[1] == USB_DESC_BOS) {
			// Windows: Config -> BOS
			detected_os = USB_HOST_OS_WINDOWS;
		} else if (desc_req_buf[0] == USB_DESC_STRING && 
		           (desc_req_buf[1] == USB_DESC_BOS || 
		            desc_req_buf[1] == USB_DESC_CONFIGURATION)) {
			// macOS: String -> (BOS or Config)
			detected_os = USB_HOST_OS_MACOS;
		}
	}
}

usb_host_os_t usb_host_os_get(void)
{
	return detected_os;
}
