#ifndef ENOCEAN_H
#define ENOCEAN_H

#include "hardware.h"
#include "motor.h"

#define RXD_PIN       ENOCEAN_RX
#define TXD_PIN       ENOCEAN_TX

#define SYNC_CODE     0x55 // Packet start code
#define ERP1          0x01 // Packet Type
#define RORG_RPS      0xF6 // Data Type - Repeated Switch Communication

#define SUM_HEADER_BYTE  4
#define HEADER_OFFSET    1
#define SUM_DATA_BYTE   14
#define DATA_OFFSET      6

#define CMD_STOP  0
#define CMD_UP    1
#define CMD_DOWN  2

extern uint32_t enocean_saved_id;
extern uint32_t enocean_received_id;

void enocean_init(void);
void uart_rx_task(void *arg);
void run_enocean_read_task(void);
void enocean_processing(uint8_t val);
uint32_t getSenderId(uint8_t* data);
uint8_t calc_header_crc(uint8_t* data);
uint8_t calc_packet_crc(uint8_t* data);

void run_enocean_connection_task();
void enocean_connection_task(void *arg);

typedef struct {
        uint8_t       syncByte;
        uint16_t      dataLength;
        uint8_t       oppDataLength;
        uint8_t       packetType;
        uint8_t       headerCRC;
        uint8_t       rorgData;
        uint8_t       data;
        uint32_t      senderID;
        uint8_t       statusData; 
        uint8_t       optData[5]; 
        uint8_t       rssi;
        uint8_t       dataCRC;
}ESP3Pack;

enum NUM_ESP3 {
  NUM_SYNC = 0,
  NUM_DATA_LENGTH_H,
  NUM_DATA_LENGTH_L,
  NUM_OPT_LENGTH,
  NUM_PACKET_TYPE,
  NUM_CRC8H,
  NUM_RORG,
  NUM_DATA,
  NUM_ID_1,
  NUM_ID_2,
  NUM_ID_3,
  NUM_ID_4,
  NUM_STATUS,
  NUM_SUBTEL,
  NUM_OPT_ID_1,
  NUM_OPT_ID_2,
  NUM_OPT_ID_3,
  NUM_OPT_ID_4,
  NUM_RSSI,
  NUM_SECURITY,
  NUM_CRC8D
}; 

#endif /* ENOCEAN_H */