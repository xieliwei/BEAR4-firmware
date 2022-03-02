#ifndef APRSLite_H
#define APRSLite_H

#include <Arduino.h>
#include "HDLC.h"
#include "AFSK.h"
#include "AX25.h"

void APRS_init();

void APRS_setCallsign(const char *call, int ssid);
void APRS_setDestination(const char *call, int ssid);
void APRS_setMessageDestination(const char *call, int ssid);
void APRS_setPath1(const char *call, int ssid);
void APRS_setPath2(const char *call, int ssid);

void APRS_useAlternateSymbolTable(bool use);
void APRS_setSymbol(char sym);

void APRS_setLat(const char *lat);
void APRS_setLon(const char *lon);
void APRS_setPower(int s);
void APRS_setHeight(int s);
void APRS_setGain(int s);
void APRS_setDirectivity(int s);

void APRS_sendPkt(const void *_buffer, size_t length, size_t path_len = 4);
void APRS_sendLoc(const void *_buffer, size_t length);
void APRS_sendMsg(const void *_buffer, size_t length);
void APRS_msgRetry();

void APRS_printSettings(Stream& DBGPORT);

#endif