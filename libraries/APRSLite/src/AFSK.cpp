#include <Arduino.h>
#include "AFSK.h"

#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
#include <driver/dac.h>
#include <hal/dac_hal.h>
#include <hal/dac_ll.h>

Afsk *AFSK_modem;

void afsk_putchar(char c);

void AFSK_init(Afsk *afsk) {
  // Allocate modem struct memory
  memset(afsk, 0, sizeof(*afsk));
  AFSK_modem = afsk;
  
  dac_cw_config_t dac_cw_config = {
    .en_ch = DAC_CHANNEL_1,
    .scale = DAC_CW_SCALE_1,
    .phase = DAC_CW_PHASE_0,
    .freq = 1200 << 3,
    .offset = 1
  };
  dac_cw_generator_config(&dac_cw_config);
  REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, 7);
  dac_cw_generator_enable();
  dac_output_enable(DAC_CHANNEL_1);
}

static inline void set_dac_freq(uint32_t freq) {
  dac_ll_cw_set_freq(freq << 3);
}

static void AFSK_txStart() {
  if (!AFSK_modem->sending) {
    AFSK_modem->bitstuffCount = 0;
    AFSK_modem->sending = true;
    digitalWrite(2, LOW);
	delay(1000);
	set_dac_freq(MARK_FREQ);
	AFSK_modem->txMark = true;
	dac_cw_generator_enable();
	delay(100);
  }
}

void AFSK_txStop(void) {
  dac_cw_generator_disable();
  delay(100);
  digitalWrite(2, HIGH);
  AFSK_modem->sending = false;
}

void afsk_putchar(char c) {
  AFSK_txStart();

  if (!AFSK_modem->escapeNext && (c == HDLC_FLAG || c == HDLC_RESET)) {
    AFSK_modem->bitStuff = false;
    AFSK_modem->bitstuffCount = 0;
  } else {
    AFSK_modem->bitStuff = true;
  }
  AFSK_modem->escapeNext = false;

  if (c == AX25_ESC) {
    AFSK_modem->escapeNext = true;
  }

  uint8_t txBit = 0x01;
  while (txBit) {
    if (c & txBit) {
      AFSK_modem->bitstuffCount++;
    } else {
      AFSK_modem->bitstuffCount = 0;
      AFSK_modem->txMark = !AFSK_modem->txMark;
      set_dac_freq(TONE(AFSK_modem->txMark));
    }
    txBit <<= 1;
    delayMicroseconds(BITLEN_MICROS);
    
    if (AFSK_modem->bitStuff && AFSK_modem->bitstuffCount >= BIT_STUFF_LEN) {
      AFSK_modem->bitstuffCount = 0;
      AFSK_modem->txMark = !AFSK_modem->txMark;
      set_dac_freq(TONE(AFSK_modem->txMark));
      delayMicroseconds(BITLEN_MICROS);
    }
  }
}