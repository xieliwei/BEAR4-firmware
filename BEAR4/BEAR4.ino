//////////////////////////////////////////////////////
//// Globals/Config
//////////////////////////////////////////////////////
// !!!!!!! Check all RBF items before flight !!!!!!!
// Note that this needs to be UTC
uint16_t todays_date[] = {2022, 11, 12};
uint16_t todays_time[] = {22, 45, 0};
bool is_leap_year = false;

uint8_t APRS_mod_s = 30;

uint8_t SSTV_mod_m = 5;
uint16_t SSTV_inhibit_height_m = 3000;
uint32_t SSTV_inhibit_time_ms = 1200000L; // 20 minutes

String callsign = "9V1WP";
String callsign_suffix = "-11";
uint8_t callsign_ssid = 11;
String comment_suffix = "SSTV@145.550M";

String boot_message = "BEAR10 Project";

#define TEMP_SP_HIGH_DEGC (7.0)
#define TEMP_SP_LOW_DEGC (2.0)

// Hardware watchdog: 60 seconds
#define WDT_TIMEOUT (60)

// We report APRS every 15s, so sleep cycle needs
// to be less than 7.5s
// Ideally we need to have at least 1s of wake time
// so we can properly catch a full GPS PVT report
#define LP_SLEEP_S  (0)
#define HP_SLEEP_S  (3600)

// Which hardware revision? Currently 2 are available:
// ESP32 (the original ESP32 with no suffix)
// ESP32S2 (2022 release with built-in SRAM, with support for siggen, but single core)
// ESP32S3 (2022 release with built-in SRAM)
// As long as you don't define HW_ESP32S3, we assume
// you are on the original ESP32
#define HW_ESP32
//#define HW_ESP32S2
//#define HW_ESP32S3

// RBF - Set this to udpstream before flight
//#define DBGPORT Serial
#define DBGPORT udpstream

// NOTE - This will be visible in source control,
// don't use anything serious here!
// Default IP of the device in softAP mode is:
//    192.168.4.1 or BEAR10.local
const char* wifissid = "BEAR10";
const char* wifipsk = "flyinghigh";
// How long we keep the AP on before shutdown
#define WIFI_TIMEOUT 90000
#define DEBUG_SERIAL_PORT 54321

// RBF - Set this to false before flight
bool sstv_run_now = true;

// RBF - Set this to true before flight
bool inhibit_sstv = true;

// RBF - Set this to false before flight
bool fast_sstv = false;

//////////////////////////////////////////////////////
//// Global State
//////////////////////////////////////////////////////
// Stores how many consecutive APRS reports
// were giving descending altitudes
uint8_t descent_count = 0;

// APRS
uint16_t msg_id = 0;

// RTC
uint16_t tyear = 2022;
uint8_t tmonth = 11;
uint8_t tday = 12;
uint8_t thour = 0;
uint8_t tmin = 0;
uint8_t tsec = 0;
uint16_t tms = 0;
unsigned long long tsync = 0;

// GPS
// Fix Type
// 0: No fix
// 1: Dead reckoning
// 2: 2D
// 3: 3D
// 4: GNSS + Dead reckoning
// 5: Time only
byte fixType = 0;
float glatitude = 0;    // Decimal degrees
float glongitude = 0;   // Decimal degrees
float galtitudeMSL = 0; // Metres

uint16_t gheading = 0;  // Degrees
float gspeed = 0;       // km/h

uint8_t gSIV = 0;       // Number of SVs used for fix
// PDOP
// <1 Ideal
// 1-2: Excellent
// 2-5: Good
// 5-10: Moderate
// 10-20: Fair (NG)
// >20: Poor (NG)
uint16_t gPDOP = 99;

// Sensors
int16_t accel_x = 0;    // Sensor's unit (average of 16-bits)
int16_t accel_y = 0;    // Sensor's unit (average of 16-bits)
int16_t accel_z = 0;    // Sensor's unit (average of 16-bits)
bool freefall = false;

float ptemp = 0;        // DegC
float ppress = 0;       // hPa
float paltitudeMSL = 0; // Metres

float btemp = 0;        // DegC

// Battery
float Cell1 = 0.0;      // Volts
float Cell2 = 0.0;      // Volts
float BV = 0.0;         // Volts

// Heater
bool heaterOn = true;

// WiFi
bool wifiOn = false;

// Camera
bool imready = false;

//////////////////////////////////////////////////////
//// Pinmap
//////////////////////////////////////////////////////
#ifdef HW_ESP32S3
#define PIN_DTR (GPIO_NUM_0)

#define PIN_RAD_PTT (GPIO_NUM_48)
#define PIN_RAD_EN (GPIO_NUM_35)
#define PIN_RAD_PW (GPIO_NUM_20)
#define PIN_RAD_SQL (GPIO_NUM_5)
#define PIN_RAD_TX (GPIO_NUM_47)
#define PIN_RAD_RX (GPIO_NUM_19)

#define PIN_HEATER (GPIO_NUM_9)

#define PIN_I2C_SDA (GPIO_NUM_42)
#define PIN_I2C_SCL (GPIO_NUM_2)

#define PIN_TX_AUD (GPIO_NUM_17)
// In older hardware revisions, this pin is swapped with ONEWIRE
#define PIN_RX_AUD (GPIO_NUM_8)

#define ADC_CELL1 (GPIO_NUM_6)
#define ADC_CELL2 (GPIO_NUM_7)

// Expansion Ports
#define PIN_SPI_CS (GPIO_NUM_38)
#define PIN_SPI_CLK (GPIO_NUM_39)
#define PIN_SPI_MISO (GPIO_NUM_40)
#define PIN_SPI_MOSI (GPIO_NUM_1)

// In older hardware revisions, this pin is swapped with RX_AUD
#define PIN_ONEWIRE (GPIO_NUM_18)

#define PIN_I2C2_SDA (GPIO_NUM_15)
#define PIN_I2C2_SCL (GPIO_NUM_16)

#define PIN_CAM_TX (GPIO_NUM_15)
#define PIN_CAM_RX (GPIO_NUM_16)

#define PIN_GPS_TX (GPIO_NUM_37)
#define PIN_GPS_RX (GPIO_NUM_36)
#elif defined(HW_ESP32S2)
// Oddly Serial2 not declared in ESP32S2
HardwareSerial Serial2(2);
#define PIN_DTR (GPIO_NUM_0)

#define PIN_RAD_PTT (GPIO_NUM_34)
#define PIN_RAD_EN (GPIO_NUM_35)
#define PIN_RAD_PW (GPIO_NUM_20)
#define PIN_RAD_SQL (GPIO_NUM_5)
#define PIN_RAD_TX (GPIO_NUM_33)
#define PIN_RAD_RX (GPIO_NUM_19)

#define PIN_HEATER (GPIO_NUM_9)

#define PIN_I2C_SDA (GPIO_NUM_42)
#define PIN_I2C_SCL (GPIO_NUM_2)

#define PIN_TX_AUD (GPIO_NUM_17)
// In older hardware revisions, this pin is swapped with ONEWIRE
#define PIN_RX_AUD (GPIO_NUM_8)

#define ADC_CELL1 (GPIO_NUM_6)
#define ADC_CELL2 (GPIO_NUM_7)

// Expansion Ports
#define PIN_SPI_CS (GPIO_NUM_38)
#define PIN_SPI_CLK (GPIO_NUM_39)
#define PIN_SPI_MISO (GPIO_NUM_40)
#define PIN_SPI_MOSI (GPIO_NUM_1)

// In older hardware revisions, this pin is swapped with RX_AUD
#define PIN_ONEWIRE (GPIO_NUM_18)

#define PIN_I2C2_SDA (GPIO_NUM_15)
#define PIN_I2C2_SCL (GPIO_NUM_16)

#define PIN_CAM_TX (GPIO_NUM_15)
#define PIN_CAM_RX (GPIO_NUM_16)

#define PIN_GPS_TX (GPIO_NUM_37)
#define PIN_GPS_RX (GPIO_NUM_36)
#else
#define PIN_DTR (GPIO_NUM_0)

#define PIN_RAD_PTT (GPIO_NUM_2)
#define PIN_RAD_EN (GPIO_NUM_4)
#define PIN_RAD_PW (GPIO_NUM_12)
#define PIN_RAD_SQL (GPIO_NUM_39)
#define PIN_RAD_TX (GPIO_NUM_15)
#define PIN_RAD_RX (GPIO_NUM_14)

#define PIN_HEATER (GPIO_NUM_13)

#define PIN_I2C_SDA (GPIO_NUM_21)
#define PIN_I2C_SCL (GPIO_NUM_22)

#define PIN_TX_AUD (GPIO_NUM_25)
// In older hardware revisions, this pin is swapped with ONEWIRE
#define PIN_RX_AUD (GPIO_NUM_27)

#define ADC_CELL1 (GPIO_NUM_34)
#define ADC_CELL2 (GPIO_NUM_35)

// Expansion Ports
#define PIN_SPI_CS (GPIO_NUM_5)
#define PIN_SPI_CLK (GPIO_NUM_18)
#define PIN_SPI_MISO (GPIO_NUM_19)
#define PIN_SPI_MOSI (GPIO_NUM_23)

// In older hardware revisions, this pin is swapped with RX_AUD
#define PIN_ONEWIRE (GPIO_NUM_26)

#define PIN_I2C2_SDA (GPIO_NUM_32)
#define PIN_I2C2_SCL (GPIO_NUM_33)

#define PIN_CAM_TX (GPIO_NUM_32)
#define PIN_CAM_RX (GPIO_NUM_33)

#define PIN_GPS_TX (GPIO_NUM_17)
#define PIN_GPS_RX (GPIO_NUM_16)
#endif

//////////////////////////////////////////////////////
//// APRS
//////////////////////////////////////////////////////
#include <APRSLite.h>

//////////////////////////////////////////////////////
//// BATTERY
//////////////////////////////////////////////////////
#define BATT_AVG_LEN 100

//////////////////////////////////////////////////////
//// DRA818
//////////////////////////////////////////////////////
#ifdef HW_ESP32S3
#include "hal/ledc_types.h"
#include "soc/ledc_periph.h"
#include "soc/ledc_struct.h"
#include "hal/gpio_hal.h"
#include "esp_rom_gpio.h"
ledc_dev_t *ledc = &LEDC;
#else
#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
#include <driver/dac.h>
#include <hal/dac_hal.h>
#include <hal/dac_ll.h>
#endif
#include <SoftwareSerial.h>
SoftwareSerial radioCtrl(PIN_RAD_RX, PIN_RAD_TX);

//////////////////////////////////////////////////////
//// GPS
//////////////////////////////////////////////////////
#include "assistnow.h"
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
SFE_UBLOX_GNSS myGNSS;

//////////////////////////////////////////////////////
//// SENSORS
//////////////////////////////////////////////////////
#include "I2Cdev.h"
#include <Wire.h>
#include "MPU6050.h"
#include <BMP388_DEV.h>

BMP388_DEV bmp388;

#define SEN_AVG_LEN 1000
//int16_t ax[SEN_AVG_LEN], ay[SEN_AVG_LEN], az[SEN_AVG_LEN];
//int16_t gx[SEN_AVG_LEN], gy[SEN_AVG_LEN], gz[SEN_AVG_LEN];
MPU6050 accelgyro;

#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(PIN_ONEWIRE);
DallasTemperature sensors(&oneWire);
DeviceAddress deviceAddress;

#define OW_SENSOR_RES (9)
unsigned long ow_lastconv = 0;

//////////////////////////////////////////////////////
//// SSTV
//////////////////////////////////////////////////////
#include "Adafruit_GFX.h"

#include "BEAR320x240.h"
#define WIDTH       BEAR320x240_WIDTH
#define HEIGHT      BEAR320x240_HEIGHT
#define COMPONENTS  BEAR320x240_COMPONENTS

const uint8_t (*im_ref)[WIDTH][COMPONENTS] = BEAR320x240;
const uint8_t (*im_cm)[4] = BEAR320x240_cm;

#define HALFWIDTH   (WIDTH/2)
#define HALFHEIGHT  (HEIGHT/2)

uint8_t (*im_buf)[WIDTH][COMPONENTS] = 0;
GFXcanvas8 *im;

//////////////////////////////////////////////////////
//// Camera
//////////////////////////////////////////////////////
#include <Adafruit_VC0706.h>
#include "JPEGDEC.h"

Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);
JPEGDEC jpeg;

#define MAX_ALLOWED_JPG_SIZE (32000)
uint8_t jpg_img[MAX_ALLOWED_JPG_SIZE];
uint16_t jpg_sz = 0;

int drawMCUs(JPEGDRAW *pDraw) {
  if (!im_buf) return 0;

  int x = pDraw->x;
  int y = pDraw->y;
  int w = pDraw->iWidth;
  int h = pDraw->iHeight;

  for (int16_t i = 0; i < w; i++) {
    for (int16_t j = 0; j < h; j++) {
      uint16_t pval = pDraw->pPixels[i + j * w];
      im_buf[y + j][x + i][0] = idx_nearest_colour((pval & 0xF800) >> 8, (pval & 0x07E0) >> 3, (pval & 0x001F) << 3);;
    }
  }
  return 1;
}

//////////////////////////////////////////////////////
//// WATCHDOG
//////////////////////////////////////////////////////
#include <esp_task_wdt.h>

//////////////////////////////////////////////////////
//// WIFI
//////////////////////////////////////////////////////
#include <WiFi.h>
#include <WiFiAP.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <DynamicArduinoOTA.h>
#include <UDPStream.h>
#include <esp_wifi.h>

UDPStream udpstream;
