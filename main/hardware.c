#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_timer.h>

#include "hardware.h"
#include "enocean.h"
#include "flash.h"
#include "motor.h"
#include "matter.h"
//#include "INA226.h"

esp_timer_handle_t fast_timer;   // 1 ms
esp_timer_handle_t slow_timer;   // 10 ms
esp_timer_handle_t second_timer; // 1 sec

uint16_t btn_count=0;
uint16_t btn_sum=0;
bool btn_state=0;

void second_timer_callback(void *priv) // 1 sec
{
    
}

void slow_timer_callback(void *priv) // 10 ms
{
    if(motor_start) motor_handler();
    button_handler(btn_state);
}

void fast_timer_callback(void *priv) // 1 ms
{
    motor_timer_function();
    /* contact bounce protection */
    btn_count++;
    if(gpio_get_level(BUTTON)==0) btn_sum++;
    if(btn_count>9)
    {
       if(btn_sum>5) btn_state=1; 
       else btn_state=0;
       btn_count=0;
       btn_sum=0;
    }
}

void timer_init(void)
{
    esp_timer_create_args_t second_timer_config = {
        .callback = second_timer_callback,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "second"};
    esp_timer_create(&second_timer_config, &second_timer);

    esp_timer_create_args_t slow_timer_config = {
        .callback = slow_timer_callback,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "slow"};
    esp_timer_create(&slow_timer_config, &slow_timer);

    esp_timer_create_args_t fast_timer_config = {
        .callback = fast_timer_callback,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "fast"};
    esp_timer_create(&fast_timer_config, &fast_timer);
  
    esp_timer_start_periodic(fast_timer,  1000U);
    esp_timer_start_periodic(slow_timer,  10000U);  
    esp_timer_start_periodic(second_timer,1000000U); 
}

void IRAM_ATTR gpio_isr_handler(void* arg)
{
    motor_feedback = gpio_get_level(MOTOR_FB);
}

void gpio_init(void)
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    gpio_set_level(LED_R, LED_OFF); gpio_set_level(UP_DIR, LED_ON);
    gpio_set_level(LED_G, LED_OFF); gpio_set_level(DOWN_DIR, LED_ON);

    io_conf.intr_type = GPIO_INTR_ANYEDGE;// GPIO_INTR_NEGEDGE // GPIO_INTR_POSEDGE // GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(MOTOR_FB, gpio_isr_handler, (void*) MOTOR_FB);
}

void led_green_blink(void)
{
    for(int i=0; i<3; i++)
    {
        gpio_set_level(LED_G, LED_ON); 
        vTaskDelay(100/portTICK_PERIOD_MS);
        gpio_set_level(LED_G, LED_OFF);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

uint16_t bh_count=0;
void button_handler(bool state) // call 10ms
{
   if(bh_count>200 && bh_count<500) // 2-5 second
   {
     gpio_set_level(LED_G, LED_ON);
     if(state==0)
     {
        bh_count=0;
        gpio_set_level(LED_G, LED_OFF);
        run_enocean_connection_task();
     }
   }

   if(bh_count>500 && bh_count<600) gpio_set_level(LED_G, LED_OFF);

   if(bh_count>600 && bh_count<900) // 6-9 second
   {
     gpio_set_level(LED_R, LED_ON);
     if(state==0)
     {
        bh_count=0;
        gpio_set_level(LED_R, LED_OFF);
        motor_reset();
     }
   }

   if(bh_count>900) gpio_set_level(LED_R, LED_OFF);

   if(state) bh_count++;
   else bh_count=0;
}

void hardware_init()
{
    gpio_init();
    timer_init();
    // i2c_init();
    // INA226_init();
    // INA226_calibrate();
    led_green_blink();
}