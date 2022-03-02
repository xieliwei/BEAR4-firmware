#ifndef AFSK_H
#define AFSK_H

#include "HDLC.h"

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