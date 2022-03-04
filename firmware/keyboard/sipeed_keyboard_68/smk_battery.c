#include "smk_battery.h"
#include "hal_gpio.h"
#include "hal_adc.h"
#include "FreeRTOS.h"
#include "stdint.h"
#include "smk_event_manager.h"
#include "events/battery_update_event.h"


adc_channel_t posChList[] = { BATTERY_ADC_CHANNEL };
adc_channel_t negChList[] = { ADC_CHANNEL_GND };
adc_channel_val_t result_val;
struct device *adc_battery;

int smk_battery_init(void){
    adc_channel_cfg_t adc_channel_cfg;
    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;
    adc_register(ADC0_INDEX, "adc_battery");
    adc_battery = device_find("adc_battery");
    if (adc_battery) {
        ADC_DEV(adc_battery)->continuous_conv_mode = ENABLE;
        device_open(adc_battery, DEVICE_OFLAG_STREAM_RX);
        if (device_control(adc_battery, DEVICE_CTRL_ADC_CHANNEL_CONFIG, &adc_channel_cfg) == ERROR) {
            BAT_DEBUG("[BAT] ADC channel config error , Please check the channel corresponding to IO is initial success by board system or Channel is invaild \r\n");
            return -1;
        }
        BAT_DEBUG("[BAT] adc device find success\r\n");
        adc_channel_start(adc_battery);
        return 0;
    }
    return -2;
}

int smk_battery_get_voltage(void){
    if (adc_battery) {
        device_read(adc_battery, 0, (void *)&result_val, 1);
        BAT_DEBUG("[BAT] PosId = %d NegId = %d V= %d mV \r\n", result_val.posChan, result_val.negChan, (int)(result_val.volt * 1000));
        return (int)(result_val.volt * 1000);
    }
    return -1;
}

static uint8_t convert_volatge_to_level(int voltage){
    if (voltage >= 4200){
        return 100;
    } else if (voltage <= 3450){
        return 0;
    } else {
        return voltage * 2 / 15 - 459;
    }
}

void smk_battery_update_task(void *pvParameters){
    int ret = smk_battery_init();
    if (ret != 0) {
        goto exit;
    }
    int voltage = 0;
    uint8_t level = 0;
    while(1){
        voltage = smk_battery_get_voltage() * 2;
        level = convert_volatge_to_level(voltage);
        SMK_EVENT_RAISE(new_battery_update_event((struct battery_update_event)
        {
            .level = level,
        }))
        BAT_DEBUG("[BAT] battery voltage = %d mV \r\n", voltage);
        BAT_DEBUG("[BAT] battery level = %d%% \r\n", level);
        vTaskDelay(CONFIG_BAS_REPORT_TIME_MS);
    }
exit:
    vTaskDelete(NULL);
}