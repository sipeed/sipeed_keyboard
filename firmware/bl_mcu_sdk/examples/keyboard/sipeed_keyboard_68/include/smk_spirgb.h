#ifndef __SMK_SPIRGB_H
#define __SMK_SPIRGB_H

#include <stdint.h>

#define SMK_RGB_USE_DMA

// Frame format
// Each uint32_t => 8b color channel
// Sequence: 24b GRB, MSB first, closest led first
// 0 => 8 -___, 1 => E ---_, high to low
// static uint32_t tbase[3] = {0x8EE88EE8, 0xEE88EE88, 0xEEEEEEEE};

// Config changes
// - "spi_config.h"
// - - SPI_SWAP_ENABLE -> 0
// - "pinmux_config.h"
// - - CONFIG_GPIO16_FUNC -> GPIO_FUN_SPI (MISO/MOSI) / MISO-MOSI switching defaulted at 1
// - "peripheral_config.h"
// - SPI0
// - - .clk = 4000000
// - - .direction = SPI_MSB_BYTE3_DIRECTION_FIRST
// - - .datasize = SPI_DATASIZE_32BIT
// - DMA0_CH3
// - - .src_width = DMA_TRANSFER_WIDTH_32BIT
// - - .dst_width = DMA_TRANSFER_WIDTH_32BIT

typedef struct {
	union {
		uint32_t word;
		struct {
			uint8_t B;
			uint8_t G;
			uint8_t R;
			uint8_t D;
		};
	};
} DRGB;

typedef struct {
	int16_t xpos;
	int16_t ypos;
	uint8_t keyseq;
	uint8_t keycode;
	uint8_t sequp;
	uint8_t seqdown;
	uint8_t seqleft;
	uint8_t seqright;
} KEY_DESC;
// key sequence described by cosntants / defines

// Effect list in memory
typedef struct {
	uint16_t eff_id;
	uint8_t eff_mask;
	uint8_t eff_next;
	void *eff_data;
	void (*eff_func)(void *);
	uint32_t eff_var[5];
} EFF_NODE;

#define RGB_LENGTH 68

void rgb_loop_task(void *pvParameters);

#endif