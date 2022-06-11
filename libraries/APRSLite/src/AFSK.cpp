#include <Arduino.h>
#include "AFSK.h"

#ifdef HW_ESP32S3
#include "hal/ledc_types.h"
#include "soc/ledc_periph.h"
#include "soc/ledc_struct.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"
static ledc_dev_t *ledc = &LEDC;

#define PIN_RAD_PTT (48)

#else
#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
#include <driver/dac.h>
#include <hal/dac_hal.h>
#include <hal/dac_ll.h>

#define PIN_RAD_PTT (2)
#endif

Afsk *AFSK_modem;

void afsk_putchar(char c);

#ifdef HW_ESP32S3
inline uint32_t calc_clkdiv(uint32_t freq) {
  return ((((uint32_t)APB_CLK_FREQ) / freq) >> 4);
}

inline void dac_cw_generator_disable() {
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.pause = 1;
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.low_speed_update = 1;
}

inline void dac_cw_generator_enable() {
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.pause = 0;
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.low_speed_update = 1;
}
#endif

void AFSK_init(Afsk *afsk) {
  // Allocate modem struct memory
  memset(afsk, 0, sizeof(*afsk));
  AFSK_modem = afsk;
  
#ifdef HW_ESP32S3
  // Dummy call to ledc library required to enable LEDC module
  // Enabling requires the periph_module_enable() function which
  // is hidden from us.
  ledcSetup(LEDC_CHANNEL_0, 1200, LEDC_TIMER_12_BIT);
  
  ledc->conf.apb_clk_sel = 1;
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.clock_divider = calc_clkdiv(1200);
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.tick_sel = 0;
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.duty_resolution = LEDC_TIMER_12_BIT;
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.low_speed_update = 1;
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.rst = 1;
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.rst = 0;
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.low_speed_update = 1;

  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].hpoint.hpoint = 0;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].duty.duty = (2047 << 4);
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf1.duty_inc = 1;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf1.duty_num = 0;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf1.duty_cycle = 0;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf1.duty_scale = 0;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.low_speed_update = 1;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.sig_out_en = true;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf1.duty_start = true;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.low_speed_update = 1;

  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.timer_sel = LEDC_TIMER_0;
  ledc->channel_group[LEDC_LOW_SPEED_MODE].channel[LEDC_CHANNEL_0].conf0.low_speed_update = 1;

  ledc->int_ena.val = (ledc->int_ena.val & (~(BIT(LEDC_DUTY_CHNG_END_LSCH0_INT_ENA_S + LEDC_CHANNEL_0))));
#else
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
#endif
}

static inline void set_dac_freq(uint32_t freq) {
#ifdef HW_ESP32S3
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.clock_divider = calc_clkdiv(freq);
  ledc->timer_group[LEDC_LOW_SPEED_MODE].timer[LEDC_TIMER_0].conf.low_speed_update = 1;
#else
  dac_ll_cw_set_freq(freq << 3);
#endif
}

static void AFSK_txStart() {
  if (!AFSK_modem->sending) {
    AFSK_modem->bitstuffCount = 0;
    AFSK_modem->sending = true;
    digitalWrite(PIN_RAD_PTT, LOW);
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
  digitalWrite(PIN_RAD_PTT, HIGH);
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