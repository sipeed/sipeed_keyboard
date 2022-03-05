#ifndef __SMK_SPIRGB_H
#define __SMK_SPIRGB_H

#include <stdint.h>

#ifndef EN_RGB_DEBUG
#define EN_RGB_DEBUG 0
#endif

#if EN_RGB_DEBUG
#include "bflb_platform.h"
#define RGB_DEBUG(fmt, ...) MSG_DBG(fmt, ##__VA_ARGS__)
#else
#define RGB_DEBUG(fmt, ...)
#endif

#define SMK_RGB_USE_DMA 1

#define RGB_LENGTH 68

enum rgb_mode_t{
	RGB_MODE_AOFF = 0,
	RGB_MODE_BON,
	RGB_MODE_AFLOW,
	RGB_MODE_BFLOW,
	RGB_MODE_AON,
	RGB_MODE_AONR,
	RGB_MODE_AONG,
	RGB_MODE_AONB,
    RGB_MODE_COUNT
};

extern int rgb_mode;

enum rgb_blend_t {
	RGB_BLEND_OVERWRITE = 0,
	RGB_BLEND_ADDITIVE,
	RGB_BLEND_ALPHA_BEGIN
};
#define RGB_BLEND_ALPHA(x) (x)

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

// key sequence described by constants / defines
typedef struct {
	int16_t xpos;
	int16_t ypos;
	uint8_t scanseq;
	uint8_t keycode;
	uint8_t keyleft;
	uint8_t keyright;
} RGB_KEY_DESC;
extern const RGB_KEY_DESC rgb_key_descriptor[RGB_LENGTH]; 

// Effect list in memory / flash
typedef struct {
	uint16_t eff_id;
	uint16_t color_id;
	uint8_t mask_id;
	uint8_t blend_type;
	uint8_t eff_next;
	uint8_t eff_rsvd;
	uint32_t time_offset;
	uint32_t eff_var[5];
} RGB_EFF_NODE;
extern RGB_EFF_NODE rgb_effect_list_fixed[8]; 

// Effect descriptor in flash
typedef struct {
	void (*eff_func)(RGB_EFF_NODE *, uint32_t);
	void (*keyhandler_func)(void *);
} RGB_EFF_DESC;
extern RGB_EFF_DESC rgb_effect_descriptor[]; 

// Color descriptor in flash

typedef struct RGB_COLOR_DESC {
	DRGB (*color_func)(struct RGB_COLOR_DESC *, uint16_t);
	uint32_t func_data[3];
} RGB_COLOR_DESC;
extern RGB_COLOR_DESC rgb_color_descriptor[10]; 

void RGB_Set_Global_Enable(uint8_t enable);
void RGB_Set_Brightness_level_up();
void RGB_Set_Brightness_level_down();
void RGB_Set_Brightness_level(uint8_t level);
DRGB rgb_alpha(DRGB source, uint8_t alpha);
void rgb_loop_task(void *pvParameters);

#endif