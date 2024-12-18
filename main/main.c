#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include <nvs_flash.h>
#include <stdio.h>
#include "matter.h"
#include "hardware.h"
#include "enocean.h"
#include "motor.h"
#include "flash.h"

void app_main(void)
{
    flash_init();
    hardware_init();
    motor_init();
    enocean_init();
    matter_init();
}