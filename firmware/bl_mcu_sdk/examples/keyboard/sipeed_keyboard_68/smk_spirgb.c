#include "smk_spirgb.h"

#include "hal_spi.h"
#include "hal_dma.h"
#include <FreeRTOS.h>
#include <task.h>

#ifdef SMK_RGB_USE_DMA

uint32_t RGB_DMA_Buffer[RGB_LENGTH*3];

void RGB_DMA_Transmit(struct device *spi, struct device *dmatxch, DRGB * rgbbuffer) {
	int i, j;
	uint8_t Gbuf, Rbuf, Bbuf;

	for (i = 0; i < RGB_LENGTH; i++) {
		Gbuf = rgbbuffer[i].G;
		Rbuf = rgbbuffer[i].R;
		Bbuf = rgbbuffer[i].B;

		for (j = 0; j < 8; j++) {
			RGB_DMA_Buffer[i*3+0] = (RGB_DMA_Buffer[i*3+0] << 4) | ((Gbuf&0x80)?0xE:0x8);
			Gbuf <<= 1;
			RGB_DMA_Buffer[i*3+1] = (RGB_DMA_Buffer[i*3+1] << 4) | ((Rbuf&0x80)?0xE:0x8);
			Rbuf <<= 1;
			RGB_DMA_Buffer[i*3+2] = (RGB_DMA_Buffer[i*3+2] << 4) | ((Bbuf&0x80)?0xE:0x8);
			Bbuf <<= 1;
		}
	}

    while (device_control(dmatxch, DMA_CHANNEL_GET_STATUS, NULL))
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

DRGB RGB_Buffer[RGB_LENGTH];

void rgb_loop_task(void *pvParameters)
{
    struct device *spi, *dma_ch3;
    uint32_t i, j;

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
	
	j = 0;
	for (;;) {
		vTaskDelay(10);

		for (i = 0; i < RGB_LENGTH; i++) {
			RGB_Buffer[i].word = 0x66CCFF - (i+j) - (i+j)*0x100 - (i+j)*0x10000;
		}

#ifdef SMK_RGB_USE_DMA
		RGB_DMA_Transmit(spi, dma_ch3, RGB_Buffer);
#else
		vTaskEnterCritical();
		RGB_Transmit(spi, RGB_Buffer);
		vTaskExitCritical();
#endif	
		j++;
	}

}