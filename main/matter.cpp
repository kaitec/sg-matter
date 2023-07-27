#include <esp_err.h>
#include <esp_log.h>
//#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>
//#include <app_reset.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>

extern "C" {
#include "motor.h"
}

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

uint16_t window_endpoint_id = 1;

static const char *TAG = "matter";
constexpr auto k_timeout_seconds = 300;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type)
    {
    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;
    case chip::DeviceLayer::DeviceEventType::kFabricRemoved:
    {
        ESP_LOGI(TAG, "Fabric removed successfully");
        if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0)
        {
            chip::CommissioningWindowManager &commissionMgr = chip::Server::GetInstance().GetCommissioningWindowManager();
            constexpr auto kTimeoutSeconds = chip::System::Clock::Seconds16(k_timeout_seconds);
            if (!commissionMgr.IsCommissioningWindowOpen())
            {
                CHIP_ERROR err = commissionMgr.OpenBasicCommissioningWindow(kTimeoutSeconds,
                                                                            chip::CommissioningWindowAdvertisement::kDnssdOnly);
                if (err != CHIP_NO_ERROR)
                {
                    ESP_LOGE(TAG, "Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT, err.Format());
                }
            }
        }
        break;
    }
    default:
        break;
    }
}

esp_err_t app_driver_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                      esp_matter_attr_val_t *val)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == window_endpoint_id) 
    {
        if (cluster_id == WindowCovering::Id) 
        { 
            if (attribute_id == WindowCovering::Attributes::TargetPositionLiftPercent100ths::Id) 
            {
                uint8_t lift_percent = val->val.u16/100;
                ESP_LOGI(__func__,"Set blynd. Lift: %d", 100-lift_percent);
                motor_set_lift(lift_percent);
            }
            if (attribute_id == WindowCovering::Attributes::TargetPositionTiltPercent100ths::Id) 
            {
                uint8_t tilt_percent = val->val.u16/100;
                ESP_LOGI(__func__,"Set blynd. Tilt: %d", tilt_percent);
                motor_set_tilt(tilt_percent);
            }
        } 
    }
    return err;
}

esp_err_t app_attribute_update_cb(attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;

    if (type == PRE_UPDATE) 
    {
        ESP_LOGI(__func__,"Callback_type: PRE_UPDATE");
        err = app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, val);
    }
    if (type == POST_UPDATE) 
    {
        ESP_LOGI(__func__,"Callback_type: POST_UPDATE"); 

    }
    if (type == READ) 
    {
        ESP_LOGI(__func__,"Callback_type: READ");
    }
    if (type == WRITE) 
    {
        ESP_LOGI(__func__,"Callback_type: WRITE");
    }
    return err;
}

extern "C" void matter_update_current_lift(uint8_t value)
{
    uint16_t endpoint_id = window_endpoint_id;
    uint32_t cluster_id = WindowCovering::Id;
    //uint32_t attribute_id = WindowCovering::Attributes::CurrentPositionLiftPercentage::Id;

    esp_matter_attr_val_t percent_u8  = esp_matter_nullable_uint8(value);
    esp_matter_attr_val_t percent_u16 = esp_matter_nullable_uint16(value*100);
    attribute::update(endpoint_id, cluster_id, WindowCovering::Attributes::CurrentPositionLiftPercentage::Id, &percent_u8);
    attribute::update(endpoint_id, cluster_id, WindowCovering::Attributes::CurrentPositionLiftPercent100ths::Id, &percent_u16);
}

extern "C" void matter_init()
{
    nullable<uint8_t> lift_percentage = nullable<uint8_t>(motor_get_lift());
    nullable<uint8_t> tilt_percentage = nullable<uint8_t>(motor_get_tilt());
    nullable<uint16_t> lift_percentage_100ths = nullable<uint16_t>(motor_get_lift()*100);
    nullable<uint16_t> tilt_percentage_100ths = nullable<uint16_t>(motor_get_tilt()*100);
    printf(" Motor GET Percentage - Lift: %d Tilt: %d \n", motor_get_lift(), motor_get_tilt());

    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, NULL);
    
    endpoint::window_covering_device::config_t window_config(static_cast<uint8_t>(chip::app::Clusters::WindowCovering::EndProductType::kExteriorVenetianBlind));
    window_config.window_covering.type = 0x08; //Tilt Blind - Lift & Tilt
    endpoint_t *endpoint = endpoint::window_covering_device::create(node, &window_config, endpoint_flags::ENDPOINT_FLAG_NONE, NULL);

    cluster_t *cluster = cluster::get(endpoint, chip::app::Clusters::WindowCovering::Id);

    cluster::window_covering::feature::lift::config_t lift;  
    cluster::window_covering::feature::position_aware_lift::config_t position_aware_lift;
    position_aware_lift.current_position_lift_percentage = lift_percentage;
    position_aware_lift.target_position_lift_percent_100ths = lift_percentage_100ths;
    position_aware_lift.current_position_lift_percent_100ths = lift_percentage_100ths;

    cluster::window_covering::feature::tilt::config_t tilt;  
    cluster::window_covering::feature::position_aware_tilt::config_t position_aware_tilt;
    position_aware_tilt.current_position_tilt_percentage = tilt_percentage;
    position_aware_tilt.target_position_tilt_percent_100ths = tilt_percentage_100ths;
    position_aware_tilt.current_position_tilt_percent_100ths = tilt_percentage_100ths;

    cluster::window_covering::feature::lift::add(cluster, &lift);
    cluster::window_covering::feature::tilt::add(cluster, &tilt);
    cluster::window_covering::feature::position_aware_lift::add(cluster, &position_aware_lift);
    cluster::window_covering::feature::position_aware_tilt::add(cluster, &position_aware_tilt);

    //cluster::window_covering::feature::absolute_position::config_t absolute_position;
    //cluster::window_covering::feature::absolute_position::add(cluster, &absolute_position);

    esp_matter::start(app_event_cb);

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::init();
#endif
}
