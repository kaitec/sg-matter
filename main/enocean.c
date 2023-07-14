#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "enocean.h"
#include "hardware.h"
#include "flash.h"
#include "motor.h"

static const int RX_BUF_SIZE = 1024;
uint32_t enocean_saved_id=0;
uint32_t enocean_received_id=0;

void run_enocean_read_task()
{
  xTaskCreate(uart_rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}

void enocean_init(void) 
{
    const uart_config_t uart_config = {
        .baud_rate = 58823,//57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 0, NULL, 0);

    enocean_saved_id=flash_enocean_read();
    run_enocean_read_task();
}

void uart_rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_2, data, RX_BUF_SIZE, 100 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;

            //ESP_LOGI(__func__, "EnOcean Recived - Data: %02X, CRC8: %02X, Calc: %02X", data[NUM_DATA], data[NUM_CRC8D], calc_packet_crc(data)); 

            if(data[NUM_SYNC]==SYNC_CODE && data[NUM_PACKET_TYPE]==ERP1 && data[NUM_RORG]==RORG_RPS)
            {
                if(data[NUM_CRC8D] == calc_packet_crc(data))
                {
                   enocean_received_id = getSenderId(data);

                   if(enocean_saved_id == enocean_received_id)
                   {
                     enocean_processing(data[NUM_DATA]);
                   }
                   else
                   {
                     //ESP_LOGI(RX_TASK_TAG, "No home packet, id: %08X", getSenderId(data));
                   } 
                }
            }
        }
    }
    free(data);
}

void enocean_processing(uint8_t val)
{
   switch (val) 
   {
      case 0x30: 
        enocean_set_lift(CMD_UP);
        //ESP_LOGI(__func__, "Enocean comand: Roll Up");
        break;
      case 0x10: 
        enocean_set_lift(CMD_DOWN);
        //ESP_LOGI(__func__, "Enocean comand: Roll Down");
        break;
      case 0x70: 
        enocean_set_tilt(CMD_UP); 
        //ESP_LOGI(__func__, "Enocean comand: Tilt Up");       
        break;
      case 0x50: 
        enocean_set_tilt(CMD_DOWN); 
        //ESP_LOGI(__func__, "Enocean comand: Tilt Down");       
        break;
      case 0x00:       
        break;
   }
}

uint32_t getSenderId(uint8_t* data) 
{
   uint32_t id = ((data[NUM_ID_1]<<24 & 0xFF000000) + (data[NUM_ID_2]<<16 & 0x00FF0000) + (data[NUM_ID_3]<<8 & 0x0000FF00) + (data[NUM_ID_4] & 0x000000FF));
   return id;
}

uint8_t CRC8[256] = {
0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5,
0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85,
0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2,
0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32,
0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c,
0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec,
0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c,
0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b,
0x76, 0x71, 0x78, 0x7f, 0x6A, 0x6d, 0x64, 0x63,
0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb,
0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8D, 0x84, 0x83,
0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb,
0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};

uint8_t calc_header_crc(uint8_t* data)
{
    uint8_t crc=0;
    for(uint8_t i=0; i<SUM_HEADER_BYTE; i++)
    {
       crc = CRC8[(crc ^ data[HEADER_OFFSET+i])];
    }
    return crc;
}

uint8_t calc_packet_crc(uint8_t* data)
{
    uint8_t crc=0;
    for(uint8_t i=0; i<SUM_DATA_BYTE; i++)
    {
       crc = CRC8[(crc ^ data[DATA_OFFSET+i])];
    }
    return crc;
}

void run_enocean_connection_task()
{
  xTaskCreate(enocean_connection_task, "enocean_connection_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
}

void enocean_connection_task(void *arg)
{
  enocean_received_id=0;
  uint8_t counter=0;

  while(1)
  {
    counter++;
    gpio_set_level(LED_G, LED_ON); gpio_set_level(LED_R, LED_OFF); 
    vTaskDelay(500/portTICK_PERIOD_MS);
    gpio_set_level(LED_G, LED_OFF); gpio_set_level(LED_R, LED_ON); 
    vTaskDelay(500/portTICK_PERIOD_MS); gpio_set_level(LED_R, LED_OFF); 
    if(enocean_received_id)
    {
      enocean_saved_id=enocean_received_id;
      led_green_blink();
      flash_enocean_write(enocean_received_id);
      vTaskDelete(NULL);
    }
    if(counter>10)
    {
      vTaskDelete(NULL);
    }
  }
}