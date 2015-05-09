#ifndef __HARDWARE_H
#define __HARDWARE_H
// This file is a header wrapper to make tinytemplate happy.

/* We do not enable UART in tinytemplate, but still need to set this
 * because it uses this define even when UART is disabled.
 */
#define BAUD_RATE 57600
#endif
