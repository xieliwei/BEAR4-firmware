#include "UDPStream.h"

UDPStream::UDPStream(uint16_t tx_buffer_size, uint16_t rx_buffer_size) {
  this->tx_buffer = (uint8_t*) malloc(tx_buffer_size);
  this->tx_buffer_size = tx_buffer_size;
  this->tx_pos = 0;
  this->tx_size = 0;
  
  this->rx_buffer = (uint8_t*) malloc(rx_buffer_size);
  this->rx_buffer_size = rx_buffer_size;
  this->rx_pos = 0;
  this->rx_size = 0;
}

UDPStream::~UDPStream() {
  free(tx_buffer);
  free(rx_buffer);
}

void UDPStream::begin(IPAddress ipaddr, uint16_t port) {
  this->port = port;
  
  udp.begin(ipaddr, port);
}

void UDPStream::clear() {
  this->tx_pos = 0;
  this->tx_size = 0;
  this->rx_pos = 0;
  this->rx_size = 0;
}

void UDPStream::task() {
  int packetSize = udp.parsePacket();
  while (packetSize && rx_size != rx_buffer_size) {
    int p = rx_pos + rx_size;
    if (p >= rx_buffer_size) {
      p -= rx_buffer_size;
    }
    rx_buffer[p] = udp.read();
    rx_size++;
	packetSize--;
  }
}

int UDPStream::read() {
  if (rx_size == 0) {
    return -1;
  } else {
    int ret = rx_buffer[rx_pos];
    rx_pos++;
    rx_size--;
    if (rx_pos == rx_buffer_size) {
      rx_pos = 0;
    }
    return ret;
  }
}

size_t UDPStream::write(uint8_t v) {
  if (tx_size == tx_buffer_size) {
    flush();
  }
  int p = tx_pos + tx_size;
  if (p >= tx_buffer_size) {
    p -= tx_buffer_size;
  }
  tx_buffer[p] = v;
  tx_size++;
  if (v == '\n') {
    flush();
  }
  return 1;
}

int UDPStream::available() {
  task();
  return rx_size;
}

int UDPStream::availableForWrite() {
  return tx_buffer_size - tx_size;
}

bool UDPStream::contains(char ch) {
  for (int i=0; i<rx_size; i++){
    int p = (rx_pos + i) % rx_buffer_size;
    if (rx_buffer[p] == ch) {
      return true;
    }
  }
  return false;
}

int UDPStream::peek() {
  return rx_size == 0 ? -1 : rx_buffer[rx_pos];
}

void UDPStream::flush() {
  if (!tx_size) return;
  udp.beginPacket(tx_address, port);
  while (tx_size) {
    udp.write(tx_buffer[tx_pos]);
    tx_pos++;
    tx_size--;
    if (tx_pos == tx_buffer_size) {
      tx_pos = 0;
    }
  }
  udp.endPacket();
}

