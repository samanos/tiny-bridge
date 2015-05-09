#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "protocol.h"
#include <avr/io.h>

// ********************************************************
// **** Tiny Hw Spi
// **
#define TINY_HW_SPI_PORT  B
#define TINY_HW_SPI_USCK  2
#define TINY_HW_SPI_DO    1 // DO is MISO on ATtiny85
#define TINY_HW_SPI_DI    0 // DI is MOSI on ATtiny85

// ********************************************************
// **** VUSB
// **
#define USB_CFG_IOPORTNAME                  B
#define USB_CFG_DMINUS_BIT                  3
#define USB_CFG_DPLUS_BIT                   4 // using PCINT4, because INT0 is used by tiny-hw-spi as SCK

// define a hook for OSCCAL calibration
#define USB_RESET_HOOK(resetStarts)         if(!resetStarts){hadUsbReset();}
#ifndef __ASSEMBLER__
extern void hadUsbReset(void);
#endif
#define USB_CFG_HAVE_MEASURE_FRAME_LENGTH   1

#define USB_CFG_VENDOR_ID                   0xc0, 0x16 /* = 0x16c0 = 5824 = voti.nl */
#define USB_CFG_DEVICE_ID                   0xdc, 0x05 /* = 0x05dc = 1500 */

#define USB_CFG_DEVICE_CLASS                0xff
#define USB_CFG_DEVICE_SUBCLASS             0

#define USB_CFG_INTERFACE_CLASS             0
#define USB_CFG_INTERFACE_SUBCLASS          0
#define USB_CFG_INTERFACE_PROTOCOL          0

#define USB_CFG_VENDOR_NAME                 VENDOR_NAME_ARRAY
#define USB_CFG_VENDOR_NAME_LEN             VENDOR_NAME_ARRAY_LEN

#define USB_CFG_DEVICE_NAME                 PRODUCT_NAME_ARRAY
#define USB_CFG_DEVICE_NAME_LEN             PRODUCT_NAME_ARRAY_LEN

#define USB_CFG_DEVICE_VERSION              0x00, 0x01 // Version number of the device: Minor number first, then major number.

#define USB_CFG_HAVE_INTRIN_ENDPOINT        0
#define USB_CFG_IS_SELF_POWERED             1
#define USB_CFG_MAX_BUS_POWER               100 // max power consumption in mA

#define USB_CFG_IMPLEMENT_FN_READ           1

// use PCINT4 for vusb interruprs
#define USB_INTR_CFG                        PCMSK
#define USB_INTR_CFG_SET                    (1 << PCINT4)
#define USB_INTR_CFG_CLR                    0
#define USB_INTR_ENABLE                     GIMSK
#define USB_INTR_ENABLE_BIT                 PCIE
#define USB_INTR_PENDING                    GIFR
#define USB_INTR_PENDING_BIT                PCIF
#define USB_INTR_VECTOR                     PCINT0_vect

#endif
