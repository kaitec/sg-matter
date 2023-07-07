#ifndef HARDWARE_H_
#define HARDWARE_H_

#pragma once

#define PCB_REV  10

#if(PCB_REV==10)
#define LED_R          32 // RGB LED
#define LED_G          33 // RGB LED
#define HALL_IN        34 // HALL sensor
#define UP_DIR          2 // MOTOR drive up
#define DOWN_DIR        4 // MOTOR drive down
#define BUTTON         35 // Settings button
#define I2C_SDA        14
#define I2C_SCL        15
#define ENOCEAN_TX     12
#define ENOCEAN_RX     13
#define MOTOR_FB       HALL_IN
#define WIND_SENSOR    ENOCEAN_TX
#define GPIO_OUTPUT_PIN_SEL ((1ULL<<LED_R) | (1ULL<<LED_G) | (1ULL<<UP_DIR) | (1ULL<<DOWN_DIR))
#define GPIO_INPUT_PIN_SEL  ((1ULL<<HALL_IN) | (1ULL<<ENOCEAN_RX) | (1ULL<<BUTTON) | (1ULL<<WIND_SENSOR))
#endif

#define LED_ON   0
#define LED_OFF  1

#define ESP_INTR_FLAG_DEFAULT 0

void gpio_init(void);
void timer_init(void);
void led_green_blink(void);
void hardware_init(void);
void button_handler(bool state);

#endif /* HARDWARE_H_ */