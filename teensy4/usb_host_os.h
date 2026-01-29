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

#pragma once

#include <stdint.h>

// Host OS detection based on USB descriptor request order.
// Different operating systems request descriptors in different orders after
// the Device Descriptor:
//
//   Windows:  Config Desc -> BOS Desc
//   Linux:    BOS Desc -> Config Desc
//   macOS:    String Desc -> (BOS or Config)
//
// This heuristic is not 100% reliable but works for most common cases.

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	USB_HOST_OS_UNKNOWN = 0,
	USB_HOST_OS_WINDOWS,
	USB_HOST_OS_LINUX,
	USB_HOST_OS_MACOS
} usb_host_os_t;

// Get the detected host operating system.
// Returns USB_HOST_OS_UNKNOWN if detection hasn't completed yet
// (not enough descriptor requests received).
usb_host_os_t usb_host_os_get(void);

// Reset the detection state. Called internally on USB reset.
void usb_host_os_reset(void);

// Record a descriptor request. Called internally from usb.c
void usb_host_os_desc_request(uint8_t desc_type);

#ifdef __cplusplus
}
#endif
