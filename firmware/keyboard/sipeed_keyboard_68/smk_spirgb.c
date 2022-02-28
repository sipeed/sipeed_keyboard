#include "smk_spirgb.h"
#include "smk_event_manager.h"
#include "smk_config_manager.h"

#include "events/rgb_switch_mode_event.h"

#include "hal_spi.h"
#include "hal_dma.h"
#include <FreeRTOS.h>
#include <task.h>
#include <math.h>

#include "smk_hid_protocol.h"


extern const int CAPS_KEY_LED;
extern const float LED_GAMMA;
uint8_t shared_kb_led=0;

static uint8_t gammalist[256];

static void init_gammalist() {
    for(int i=0;i<256;i++) {
        float a=((float)i)/255.0f;
        float b=powf(a,LED_GAMMA)*255;
        gammalist[i]=b>255?255:b;
    }
}

#if(SMK_RGB_USE_DMA)
uint32_t RGB_DMA_Buffer[RGB_LENGTH*3];

void RGB_DMA_Transmit(struct device *spi, struct device *dmatxch, DRGB * rgbbuffer) {
	int i, j;
	uint8_t Gbuf, Rbuf, Bbuf;

	for (i = 0; i < RGB_LENGTH; i++) {
		Gbuf = gammalist[rgbbuffer[i].G];
		Rbuf = gammalist[rgbbuffer[i].R];
		Bbuf = gammalist[rgbbuffer[i].B];

        if(i==CAPS_KEY_LED) {
			if (shared_kb_led&0x02) {
				Gbuf = Rbuf = Bbuf = 0xff;
			}
        }

		for (j = 0; j < 8; j++) {
			RGB_DMA_Buffer[i*3+0] = (RGB_DMA_Buffer[i*3+0] << 4) | ((Gbuf&0x80)?0xE:0x8);
			Gbuf <<= 1;
			RGB_DMA_Buffer[i*3+1] = (RGB_DMA_Buffer[i*3+1] << 4) | ((Rbuf&0x80)?0xE:0x8);
			Rbuf <<= 1;
			RGB_DMA_Buffer[i*3+2] = (RGB_DMA_Buffer[i*3+2] << 4) | ((Bbuf&0x80)?0xE:0x8);
			Bbuf <<= 1;
		}
	}

    while (device_control(dmatxch, DEVICE_CTRL_DMA_CHANNEL_GET_STATUS, NULL))
		;

	device_control(spi, DEVICE_CTRL_TX_DMA_RESUME, NULL);
	device_control(dmatxch, DEVICE_CTRL_CLR_INT, NULL);

	dma_reload(dmatxch, (uint32_t)RGB_DMA_Buffer, (uint32_t)DMA_ADDR_SPI_TDR, sizeof(RGB_DMA_Buffer));
	dma_channel_start(dmatxch);
}

#else

void RGB_Transmit(struct device *spi, DRGB * rgbbuffer) {
	uint32_t dtrans[3];
	uint8_t Gbuf, Rbuf, Bbuf;
	
	for (int j = 0; j < RGB_LENGTH; j++) {
		Gbuf = rgbbuffer[j].G;
		Rbuf = rgbbuffer[j].R;
		Bbuf = rgbbuffer[j].B;
		
		for (int i = 0; i < 8; i++) {
			dtrans[0] = (dtrans[0] << 4) | ((Gbuf&0x80)?0xE:0x8);
			Gbuf <<= 1;
			dtrans[1] = (dtrans[1] << 4) | ((Rbuf&0x80)?0xE:0x8);
			Rbuf <<= 1;
			dtrans[2] = (dtrans[2] << 4) | ((Bbuf&0x80)?0xE:0x8);
			Bbuf <<= 1;
		}
		
		spi_transmit(spi, dtrans, 3, SPI_TRANSFER_TYPE_32BIT);
	}
}

#endif

/* static */ int rgb_mode = 0;
/* static */ uint8_t rgb_global_brightness = 120;

DRGB RGB_Buffer[RGB_LENGTH];

static const hid_data_reg_t modereg={
    .base=0x8000,
    .size=sizeof(rgb_mode),
    .data=&rgb_mode,
    .maptype=map_type_data,
    .datatype=data_type_fixed
};

static const hid_data_reg_t brightnessreg = {
    .base=0x8004,
    .size=sizeof(rgb_global_brightness),
    .data=&rgb_global_brightness,
    .maptype=map_type_data,
    .datatype=data_type_fixed
};

static const hid_data_reg_t ledreg={
    .base=0x9000,
    .size=sizeof(RGB_Buffer),
    .data=&RGB_Buffer,
    .maptype=map_type_data,
    .datatype=data_type_fixed
};

static const hid_data_reg_t ledcolorreg={
    .base=0xA000,
    .size=sizeof(rgb_color_descriptor),
    .data=&rgb_color_descriptor,
    .maptype=map_type_data,
    .datatype=data_type_fixed
};

static const hid_data_reg_t ledcfgreg={
    .base=0xB000,
    .size=sizeof(rgb_effect_list_fixed),
    .data=&rgb_effect_list_fixed,
    .maptype=map_type_data,
    .datatype=data_type_fixed
};

DRGB rgb_alpha(DRGB source, uint8_t alpha)
{
    DRGB rval;
    rval.R = (source.R * alpha) >> 8;
    rval.G = (source.G * alpha) >> 8;
    rval.B = (source.B * alpha) >> 8;
    return rval;
}

uint32_t timestamp = 0;

void rgb_loop_task(void *pvParameters)
{
    vTaskDelay(500);
    rgb_mode = smk_rgb_config->rgb_mode;
    struct device *spi, *dma_ch3;
    uint32_t i;

    init_gammalist();

    spi_register(SPI0_INDEX, "spi");
    spi = device_find("spi");
    dma_register(DMA0_CH3_INDEX, "dma_ch3");
    dma_ch3 = device_find("dma_ch3");

    if (spi && dma_ch3) {
        device_open(spi, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
		device_open(dma_ch3, 0);
	} else {
		vTaskDelete(NULL);
	}

    hid_data_protocal_reg(&modereg);
    hid_data_protocal_reg(&brightnessreg);
    hid_data_protocal_reg(&ledreg);
    hid_data_protocal_reg(&ledcolorreg);
    hid_data_protocal_reg(&ledcfgreg);

	for (;;) {
		vTaskDelay(10);

		if (rgb_mode < RGB_MODE_COUNT) {
			
			for (i = 0; i < RGB_LENGTH; i++) {
				RGB_Buffer[i].word = 0;
			}

			RGB_EFF_NODE *ceff = &rgb_effect_list_fixed[rgb_mode];
			int ceffid = ceff->eff_id;
			int ceffoffset = ceff->time_offset;
			rgb_effect_descriptor[ceffid].eff_func(ceff, timestamp - ceffoffset);
		
			if (rgb_global_brightness != 255) {
				for (i = 0; i < RGB_LENGTH; i++) {
					RGB_Buffer[i] = rgb_alpha(RGB_Buffer[i], rgb_global_brightness);
				}
			}
		}
		
		timestamp ++;

#if(SMK_RGB_USE_DMA)
		RGB_DMA_Transmit(spi, dma_ch3, RGB_Buffer);
#else
		vTaskEnterCritical();
		RGB_Transmit(spi, RGB_Buffer);
		vTaskExitCritical();
#endif	
	}

}

void switch_to_next_rgb_mode(void)
{
    rgb_mode++;
    if(rgb_mode >= RGB_MODE_COUNT) {
        rgb_mode = 0;
    }
    smk_rgb_config->rgb_mode = rgb_mode;
    smk_config_save();
    // EM_DEBUG("[SMK][EVENT]current rgb_mode: %d", rgb_mode);
}

void switch_to_last_rgb_mode(void)
{
    rgb_mode--;
    if(rgb_mode < 0) {
        rgb_mode = RGB_MODE_COUNT - 1;
    }
    smk_rgb_config->rgb_mode = rgb_mode;
    smk_config_save();
//     EM_DEBUG("[SMK][EVENT]current rgb_mode: %d", rgb_mode);
}

int rgb_switch_mode_event_listener(const smk_event_t *eh){
    const struct rgb_switch_mode_event *ev = as_rgb_switch_mode_event(eh);
    if (ev)
    {
        EM_DEBUG("[SMK][EVENT]: rgb_swicth_mode. direction:%d\r\n", ev->direction);
        if (ev->direction == 1) {
            switch_to_next_rgb_mode();
        } else if (ev->direction == -1) {
            switch_to_last_rgb_mode();
        }
        EM_DEBUG("[SMK][EVENT]current rgb_mode: %d\r\n", rgb_mode);
    }
    return 0;
}

SMK_LISTENER(rgb_switch_mode_event_listener, rgb_switch_mode_event_listener);
SMK_SUBSCRIPTION(rgb_switch_mode_event_listener, rgb_switch_mode_event);
