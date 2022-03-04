#pragma once

#include <Stream.h>
#include <WiFiUdp.h>

class UDPStream : public Stream {
  WiFiUDP udp;
  uint16_t port;
  
  uint8_t *tx_buffer;
  uint16_t tx_buffer_size;
  uint16_t tx_pos, tx_size;
  
  uint8_t *rx_buffer;
  uint16_t rx_buffer_size;
  uint16_t rx_pos, rx_size;
public:
  static const uint16_t DEFAULT_TX_SIZE = 512;
  static const uint16_t DEFAULT_RX_SIZE = 32;
  static constexpr const char * tx_address = "255.255.255.255";
  
  UDPStream(uint16_t tx_buffer_size = UDPStream::DEFAULT_TX_SIZE, uint16_t rx_buffer_size = UDPStream::DEFAULT_RX_SIZE);
  ~UDPStream();
  
  void begin(IPAddress ipaddr, uint16_t port = 54321);
    
  /* Clear the buffer */
  void clear(); 
  
  /* Run regularly if you want to receive */
  void task();
  
  virtual size_t write(uint8_t);
  virtual int availableForWrite(void);
  
  virtual int available();
  virtual bool contains(char);
  virtual int read();
  virtual int peek();
  virtual void flush();
};
