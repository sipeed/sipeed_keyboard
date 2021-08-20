#include "smk_spirgb.h"

#include <FreeRTOS.h>
#include "hal_spi.h"

inline void RGB_Transmit(DRGB * unit) {
	uint32_t dtrans[3];
	uint8_t Gbuf, Rbuf, Bbuf;
	
	Gbuf = unit->G;
	Rbuf = unit->R;
	Bbuf = unit->B;
	
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

DRGB RGB_Buffer[RGB_LENGTH];

static void rgb_loop_task(void)
{
    struct device *spi;
    uint32_t i, j;
    spi_register(SPI0_INDEX, "spi", DEVICE_OFLAG_RDWR);
    spi = device_find("spi");

    if (spi) {
        device_open(spi, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_STREAM_RX);
    }
	
	for (i = 0; i < RGB_LENGTH; i++) {
		RGB_Buffer[i].word = 0x66CCFF - i - i*0x100 - i*0x10000;
	}
	j = 0;
	for (;;) {
		vTaskDelay(100);
		
		for (i = 0; i < RGB_LENGTH; i++) {
			RGB_Buffer[i].word = 0x66CCFF - (i+j) - (i+j)*0x100 - (i+j)*0x10000;
		}

		for (i = 0; i < RGB_LENGTH; i++) {
			RGB_Transmit(RGB_Buffer+i);
		}
		j++;
	}

}