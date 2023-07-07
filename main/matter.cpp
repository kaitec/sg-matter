#include <esp_err.h>
#include <esp_log.h>
//#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <esp_matter_ota.h>
//#include <app_reset.h>

#include <app/server/CommissioningWindowManager.h>
#include <app/server/Server.h>

extern "C" {
#include "motor.h"
}

/** Matter max values (used for remapping attributes) */
#define MATTER_LIFT_PERCENT 100
#define MATTER_TILT_PERCENT 100
/** Standard max values (used for remapping attributes) */
#define STANDARD_LIFT_PERCENT 100
#define STANDARD_TILT_PERCENT 100

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
                uint8_t percent = 100-(val->val.u16 / 100);
                ESP_LOGI(__func__,"Set blynd. Lift: %d", percent);
                set_blind(LIFT, percent);
            }
            // if (attribute_id == WindowCovering::Attributes::TargetPositionTiltPercent100ths::Id) 
            // {
            //     ESP_LOGI(__func__,"Set blynd. Tilt: %d", percent);
            // }
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

        if (attribute_id == WindowCovering::Attributes::CurrentPositionLiftPercentage::Id) 
        {

        }

        // node_t *node = node::get();
        // endpoint_t *endpoint = endpoint::get(node, endpoint_id);
        // cluster_t *cluster = cluster::get(endpoint, cluster_id);
        // attribute_t *attrib = attribute::get(cluster, attribute_id);
        // attribute::set_override_callback(attrib, NULL);
        
        // uint8_t lift_val = 34;
        // esp_matter_attr_val_t lift = esp_matter_nullable_uint8(lift_val);
        // attribute::update(endpoint_id, cluster_id, attribute_id, &lift); 
        // attribute::set_override_callback(attrib, NULL);
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
    uint32_t attribute_id = WindowCovering::Attributes::CurrentPositionLiftPercentage::Id;

    esp_matter_attr_val_t val = esp_matter_nullable_uint8(value);
    attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

extern "C" void matter_init()
{
    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, NULL);

    endpoint::window_covering_device::config_t window_config(static_cast<uint8_t>(chip::app::Clusters::WindowCovering::EndProductType::kExteriorVenetianBlind));
    endpoint_t *endpoint = endpoint::window_covering_device::create(node, &window_config, endpoint_flags::ENDPOINT_FLAG_NONE, NULL);

    cluster_t *cluster = cluster::get(endpoint, chip::app::Clusters::WindowCovering::Id);
    cluster::window_covering::feature::lift::config_t lift;
    cluster::window_covering::feature::position_aware_lift::config_t position_aware_lift;
    cluster::window_covering::feature::absolute_position::config_t absolute_position;
    cluster::window_covering::feature::lift::add(cluster, &lift);
    cluster::window_covering::feature::position_aware_lift::add(cluster, &position_aware_lift);
    cluster::window_covering::feature::absolute_position::add(cluster, &absolute_position);
    //cluster::window_covering::attribute::create_current_position_lift_percentage(cluster, 0);

    esp_matter::start(app_event_cb);

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::init();
#endif
}
