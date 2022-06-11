#ifndef AFSK_H
#define AFSK_H

#include "HDLC.h"

// Which hardware revision? Currently 2 are available:
// ESP32 (the original ESP32 with no suffix)
// ESP32S3 (2022 release with built-in SRAM)
// As long as you don't define HW_ESP32S3, we assume
// you are on the original ESP32
#define HW_ESP32
//#define HW_ESP32S3
#define BITRATE       (1200)
#define BITLEN_MICROS (830) // 833
#define BIT_STUFF_LEN (5)
#define MARK_FREQ     (1200)
#define SPACE_FREQ    (2200)

#define TONE(b)  ((b) ? MARK_FREQ : SPACE_FREQ)

typedef struct Afsk
{
  bool escapeNext;                        // Continue bitstuffing next character
  bool bitStuff;                          // Whether bitstuffing is allowed
  uint8_t bitstuffCount;                  // Counter for bit-stuffing
  volatile bool sending;                  // Set when modem is sending
  bool txMark;
} Afsk;

void AFSK_init(Afsk *afsk);
void AFSK_txStop(void);
void afsk_putchar(char c);

#endif