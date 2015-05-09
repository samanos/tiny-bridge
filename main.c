#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include <util/atomic.h>

#include "tiny_hw_spi.h"
#include "usbdrv.h"
#include "transmitter.h"

#define abs(x) ((x) > 0 ? (x) : (-x))

// Called by V-USB after device reset
void hadUsbReset() {
  int frameLength, targetLength = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);
  int bestDeviation = 9999;
  uchar trialCal, bestCal, step, region;

  // do a binary search in regions 0-127 and 128-255 to get optimum OSCCAL
  for(region = 0; region <= 1; region++) {
    frameLength = 0;
    trialCal = (region == 0) ? 0 : 128;

    for(step = 64; step > 0; step >>= 1) {
      if(frameLength < targetLength) // true for initial iteration
        trialCal += step; // frequency too low
      else
        trialCal -= step; // frequency too high

      OSCCAL = trialCal;
      frameLength = usbMeasureFrameLength();

      if(abs(frameLength-targetLength) < bestDeviation) {
        bestCal = trialCal; // new optimum found
        bestDeviation = abs(frameLength -targetLength);
      }
    }
  }

  OSCCAL = bestCal;
}

#define USB_NRF_STATUS  2

/* wValue.bytes[0]: address
 * wValue.bytes[1]: payload
 * wIndex.bytes[0]:
 * wIndex.bytes[1]:
 */
#define USB_NRF_TRANSFER1 3

static uint8_t replyBuf[16] = "Hello World!";
static uint8_t command, payload;
static uint32_t delay = 1;

// this gets called when custom control message is received
USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data; // cast data to correct

  switch(rq->bRequest) { // custom command is in the bRequest field
  case USB_NRF_STATUS: // query NRF status and send it to host
    usbMsgPtr = replyBuf;
    replyBuf[0] = tiny_spi_transfer(0xFF);
    return 1;
  case USB_NRF_TRANSFER1:
    usbMsgPtr = replyBuf;
    int8_t reply_len = send_payload(rq->wValue.bytes, 1, rq->wValue.bytes + 1, 1, &replyBuf[1]);
    if (reply_len >= 0) {
      replyBuf[0] = 0;
      return reply_len + 1;
    }
    else {
      replyBuf[0] = -1;
      return 1;
    }
    /*command = rq->wValue.bytes[0];
    payload = rq->wValue.bytes[1];
    return USB_NO_MSG;*/
    /*usbMsgPtr = replyBuf;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      _delay_us(delay);
    }
    delay *= 2;
    return 0;*/
  }

  return 0; // should not get here
}

USB_PUBLIC uchar usbFunctionRead(uchar *data, uchar len) {
  int8_t reply_len = send_payload(&command, 1, &payload, 1, &data[1]);
  if (reply_len >= 0) {
    data[0] = 0;
    return reply_len + 1;
  }
  else {
    data[0] = -1;
    return 1;
  }
}


int main() {
  tiny_spi_begin();
  enable_transmitter();

  // set up a 1 second watchdog timer that resets the microcontroller
  // if 1000 milliseconds pass without a call to wdt_reset()
  wdt_enable(WDTO_1S);

  // init v-usb
  usbInit();

  usbDeviceDisconnect(); // enforce re-enumeration
  for(uchar i = 0; i < 250; i++) { // wait 500 ms
    wdt_reset(); // keep the watchdog happy
    _delay_ms(2);
  }
  usbDeviceConnect();

  sei(); // Enable interrupts after re-enumeration

  while(1) {
    wdt_reset(); // keep the watchdog happy
    usbPoll();
  }

  return 0;
}
