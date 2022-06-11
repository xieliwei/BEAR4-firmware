//////////////////////////////////////////////////////
//// Globals/Config
//////////////////////////////////////////////////////
// !!!!!!! Check all RBF items before flight !!!!!!!
// Note that this needs to be UTC
uint16_t todays_date[] = {2022, 03, 04};
uint16_t todays_time[] = {15, 05, 00};
bool is_leap_year = false;

uint8_t APRS_mod_s = 30;

uint8_t SSTV_mod_m = 5;
uint16_t SSTV_inhibit_height_m = 5000;
uint32_t SSTV_inhibit_time_ms = 1200000L; // 20 minutes

String callsign = "9V1UP";
String callsign_suffix = "-11";
uint8_t callsign_ssid = 11;
String comment_suffix = "SSTV@145.500M";

String boot_message = "BEAR5 Project";

#define TEMP_SP_HIGH_DEGC (7.0)
#define TEMP_SP_LOW_DEGC (2.0)

// Hardware watchdog: 60 seconds
#define WDT_TIMEOUT (60)

// We report APRS every 15s, so sleep cycle needs
// to be less than 7.5s
// Ideally we need to have at least 1s of wake time
// so we can properly catch a full GPS PVT report
#define LP_SLEEP_S  (12)
#define HP_SLEEP_S  (2)

//#define DBGPORT Serial
#define DBGPORT udpstream

// NOTE - This will be visible in source control,
// don't use anything serious here!
// Default IP of the device in softAP mode is:
//    192.168.4.1 or BEAR5.local
const char* wifissid = "BEAR5";
const char* wifipsk = "flyinghigh";
// How long we keep the AP on before shutdown
#define WIFI_TIMEOUT 60000
#define DEBUG_SERIAL_PORT 54321

//////////////////////////////////////////////////////
//// Global State
//////////////////////////////////////////////////////
// APRS
uint16_t msg_id = 0;

// RTC
uint16_t tyear = 2022;
uint8_t tmonth = 11;
uint8_t tday = 1;
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

// Battery
float Cell1 = 0.0;      // Volts
float Cell2 = 0.0;      // Volts
float BV = 0.0;         // Volts

// Heater
bool heaterOn = true;

// WiFi
bool wifiOn = false;

//////////////////////////////////////////////////////
//// Pinmap
//////////////////////////////////////////////////////
#define PIN_DTR (0)

#define PIN_RAD_PTT (2)
#define PIN_RAD_EN (4)
#define PIN_RAD_PW (12)
#define PIN_RAD_SQL (39)

#define PIN_HEATER (13)

#define PIN_I2C_SDA (21)
#define PIN_I2C_SCL (22)

#define PIN_TX_AUD (25)
#define PIN_RX_AUD (26)

#define ADC_CELL1 (34)
#define ADC_CELL2 (35)

// Expansion Ports
#define PIN_SPI_CS (5)
#define PIN_SPI_CLK (18)
#define PIN_SPI_MISO (19)
#define PIN_SPI_MOSI (23)

#define PIN_ONEWIRE (27)

#define PIN_I2C2_SDA (32)
#define PIN_I2C2_SCL (33)

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
#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
#include <driver/dac.h>
#include <hal/dac_hal.h>
#include <hal/dac_ll.h>
#include <SoftwareSerial.h>
SoftwareSerial radioCtrl(14, 15);

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

uint8_t im_buf[HEIGHT][WIDTH][COMPONENTS];
GFXcanvas8 im(WIDTH, HEIGHT, (uint8_t*)im_buf);

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
