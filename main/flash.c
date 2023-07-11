#include <stdio.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs.h"

esp_err_t err;
nvs_handle_t my_handle;
//uint16_t data_to_flash;

void flash_init(void)
{
    err = nvs_flash_init();
    if (err != ESP_OK)
	{ 
		printf("Error (%s) NVS Flash Init!\n", esp_err_to_name(err));
    } 
    // if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    // {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     err = nvs_flash_init();
    // }
}

void flash_position_write(uint32_t value)
{
	err = nvs_open("storage", NVS_READWRITE, &my_handle);

	if (err != ESP_OK)
	{ 
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else
    {
        err = nvs_set_u32(my_handle, "position", value);
        //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        err = nvs_commit(my_handle);
        //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
}

void flash_haight_write(uint32_t value)
{
	err = nvs_open("storage", NVS_READWRITE, &my_handle);

	if (err != ESP_OK)
	{ 
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else
    {
        err = nvs_set_u32(my_handle, "haight", value);
        //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        err = nvs_commit(my_handle);
        //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
}

uint32_t flash_position_read(void)
{
	uint32_t value=0;
	err = nvs_open("storage", NVS_READWRITE, &my_handle);

	if (err != ESP_OK)
	{ 
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else
    {
	    err = nvs_get_u32(my_handle, "position", &value);
	    //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
    return value;
}

uint32_t flash_haight_read(void)
{
	uint32_t value=0;
	err = nvs_open("storage", NVS_READWRITE, &my_handle);

	if (err != ESP_OK)
	{ 
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else
    {
	    err = nvs_get_u32(my_handle, "haight", &value);
	    //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
    return value;
}

void flash_enocean_write(uint32_t value)
{
	err = nvs_open("storage", NVS_READWRITE, &my_handle);

	if (err != ESP_OK)
	{ 
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else
    {
        err = nvs_set_u32(my_handle, "enocean", value);
        //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        err = nvs_commit(my_handle);
        //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
}

uint32_t flash_enocean_read(void)
{
	uint32_t value=0;
	err = nvs_open("storage", NVS_READWRITE, &my_handle);

	if (err != ESP_OK)
	{ 
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 
    else
    {
	    err = nvs_get_u32(my_handle, "enocean", &value);
	    //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        nvs_close(my_handle);
    }
    return value;
}

// void write_flash_16(uint16_t value)
// {
// 	err = nvs_open("storage", NVS_READWRITE, &my_handle);

// 	if (err != ESP_OK)
// 	{ 
// 		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     } 
//     else
//     {
//         err = nvs_set_u16(my_handle, "data_to_flash_16", value);
//         //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
//         err = nvs_commit(my_handle);
//         //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
//         nvs_close(my_handle);
//     }
// }

// uint16_t read_flash_16(void)
// {
// 	uint16_t value=0;
// 	err = nvs_open("storage", NVS_READWRITE, &my_handle);

// 	if (err != ESP_OK)
// 	{ 
// 		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     } 
//     else
//     {
// 	    err = nvs_get_u16(my_handle, "data_to_flash", &value);
// 	    //printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
//         nvs_close(my_handle);
//     }
//     return value;
// }