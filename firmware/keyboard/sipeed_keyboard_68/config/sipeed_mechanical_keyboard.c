/*=========================================================================================================================
 .-------.   .-------.   .-------.   .-------.   .-------.   .-------.   .-------.   .-------.   .-------.   .-------.
/  SIP  / `\/  EED  / `\/       / `\/  MECH / `\/  ANI  / `\/  CAL  / `\/       / `\/  KEY  / `\/   BO  / `\/  ARD  / `\
`------'`  /`------'`  /`------'`  /`------'`  /`------'`  /`------'`  /`------'`  /`------'`  /`------'`  /`------'`  /
V       V / V       V / V       V / V metro V / V  94@  V / V  Git  V / V  Hub  V / V       V / V       V / V       V /
 `------'    `------'    `------'    `------'    `------'    `------'    `------'    `------'    `------'    `------'
=========================================================================================================================*/

#define USE_KEYCODE_SHORT_NAME

#include "keyboard/smk_keyboard.h"
#include "keyboard/smk_debounce.h"

#include "hal_gpio.h"
#include "glb_reg.h"

#define MATRIX_COLS 11
#define MATRIX_ROWS 7
#define LAYOUT_COLS 15
#define LAYOUT_ROWS 5
#define LAYERS      3

static const smk_keycode_type smk_default_matrix_keymaps[LAYERS][LAYOUT_ROWS][LAYOUT_COLS] = {
    {
            /*<  .-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.---------------.-------.        >*/
                {  ESC  ,  NUM1 ,  NUM2 ,  NUM3 ,  NUM4 ,  NUM5 ,  NUM6 ,  NUM7 ,  NUM8 ,  NUM9 ,  NUM0 , MINUS , EQUAL ,   BACKSPACE   , GRAVE }, \
          /*<  /======='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.===========/`======/`. /    >*/
              {    TAB    ,   Q   ,   W   ,   E   ,   R   ,   T   ,   Y   ,   U   ,   I   ,   O   ,   P   , LBRKT , RBRKT ,   BSLASH  ,  DEL  }, \
        /*<  /==========='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`==========/`======/`. /    >*/
            {     CAPS    ,   A   ,   S   ,   D   ,   F   ,   G   ,   H   ,   J   ,   K   ,   L   , SCOLN , QUOTE ,      ENTER      ,  PGUP }, \
      /*<  /============='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`========.=======/`======/`. /    >*/
          {      LSHIFT     ,   Z   ,   X   ,   C   ,   V   ,   B   ,   N   ,   M   , COMMA ,  DOT  , SLASH ,     RSFT    ,   UP  ,  PGDN }, \
    /*<  /=========.======='`.====='`==.==='`======'`======'`======'`======'`======'`====.='`====.='`====.='`====.=======/`======/`======/`. /    >*/
        {  LCTRL  ,   LGUI  ,   LALT  ,                      SPACE                      ,  RALT , MO(2) , RCTRL ,  LEFT ,  DOWN , RIGHT }  \
  /*<  `---------'`--------'`--------'`------------------------------------------------'`------'`------'`------'`------'`------'`------'`. /    >*/
 /*<   V          V         V         V                     Layer 0                     V       V       V       V       V       V       V /    >*/
/*<    `----------'`--------'`--------'`------------------------------------------------'`------'`------'`------'`------'`------'`------'     >*/
    }, {
            /*<  .-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.---------------.-------.        >*/
                {  ESC  ,  NUM1 ,  NUM2 ,  NUM3 ,  NUM4 ,  NUM5 ,  NUM6 ,  NUM7 ,  NUM8 ,  NUM9 ,  NUM0 , MINUS , EQUAL ,   BACKSPACE   , GRAVE }, \
          /*<  /======='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.===========/`======/`. /    >*/
              {    TAB    ,   Q   ,   W   ,   E   ,   R   ,   T   ,   Y   ,   U   ,   I   ,   O   ,   P   , LBRKT , RBRKT ,   BSLASH  ,  DEL  }, \
        /*<  /==========='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`==========/`======/`. /    >*/
            {     CAPS    ,   A   ,   S   ,   D   ,   F   ,   G   ,   H   ,   J   ,   K   ,   L   , SCOLN , QUOTE ,      ENTER      ,  PGUP }, \
      /*<  /============='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`========.=======/`======/`. /    >*/
          {      LSHIFT     ,   Z   ,   X   ,   C   ,   V   ,   B   ,   N   ,   M   , COMMA ,  DOT  , SLASH ,     RSFT    ,   UP  ,  PGDN }, \
    /*<  /=========.======='`.====='`==.==='`======'`======'`======'`======'`======'`====.='`====.='`====.='`====.=======/`======/`======/`. /    >*/
        {  LCTRL  ,   LOPT  ,   LCMD  ,                      SPACE                      ,  RCMD , MO(2) , RCTRL ,  LEFT ,  DOWN , RIGHT }  \
  /*<  `---------'`--------'`--------'`------------------------------------------------'`------'`------'`------'`------'`------'`------'`. /    >*/
 /*<   V          V         V         V                     Layer 1                     V       V       V       V       V       V       V /    >*/
/*<    `----------'`--------'`--------'`------------------------------------------------'`------'`------'`------'`------'`------'`------'     >*/
    }, {
            /*<  .-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.-------.---------------.-------.        >*/
                { _____ ,   F1  ,   F2  ,   F3  ,   F4  ,   F5  ,   F6  ,   F7  ,   F8  ,   F9  ,  F10  ,  F11  ,  F12  ,     _____     , PRTSC }, \
          /*<  /======='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.===========/`======/`. /    >*/
              {   _____   , XXXXX , XXXXX , XXXXX , XXXXX , XXXXX , XXXXX , NUMLK ,   P7  ,   P8  ,   P9  , PSLSH ,  PAST ,   PMINUS  ,  INS  }, \
        /*<  /==========='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`.====='`==========/`======/`. /    >*/
            {    _____    , DF(0) , DF(1) , XXXXX , XXXXX , XXXXX , XXXXX , XXXXX ,   P4  ,   P5  ,   P6  , PPLUS ,      PENTER     ,  HOME }, \
      /*<  /============='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`==.==='`========.=======/`======/`. /    >*/
          {      _____      , XXXXX , XXXXX , XXXXX , XXXXX , XXXXX , XXXXX , XXXXX ,   P1  ,   P2  ,   P3  ,    XXXXX    , _____ ,  END  }, \
    /*<  /=========.======='`.====='`==.==='`======'`======'`======'`======'`======'`====.='`====.='`====.='`====.=======/`======/`======/`. /    >*/
        {  _____  ,  _____  ,  _____  ,                      _____                      ,   P0  , _____ ,  PDOT , _____ , _____ , _____ }  \
  /*<  `---------'`--------'`--------'`------------------------------------------------'`------'`------'`------'`------'`------'`------'`. /    >*/
 /*<   V          V         V         V                     Layer 2                     V       V       V       V       V       V       V /    >*/
/*<    `----------'`--------'`--------'`------------------------------------------------'`------'`------'`------'`------'`------'`------'     >*/
    }
};

const smk_keyboard_hardware_type * smk_keyboard_get_hardware(void)
{
    static const uint8_t matrix_col_pins[MATRIX_COLS] = { 20, 21, 22, 23, 24, 25, 26, 27, 29, 30, 31 };
    static const uint8_t matrix_row_pins[MATRIX_ROWS] = { 0, 1, 2, 3, 4, 5, 6 };

    static const uint8_t matrix_to_layout[MATRIX_COLS][MATRIX_ROWS] = {
        {  0, 15, 30, 45, 60, -1, -1 },
        {  1, 16, 31, 46, 61, 68, 69 },
        {  2, 17, 32, 47, 62, 66, 67 },
        {  3, 18, 33, 48, -1, 57, 58 },
        {  4, 19, 34, 49, -1, 56, -1 },
        {  5, 20, 35, 50, 63, -1, 43 },
        {  6, 21, 36, 51, -1, 41, 42 },
        {  7, 22, 37, 52, -1, 28, 29 },
        {  8, 23, 38, 53, -1, 26, 27 },
        {  9, 24, 39, 54, 64, 13, 14 },
        { 10, 25, 40, 55, 65, 11, 12 }
    };

    static const smk_keyboard_hardware_type hardware = {
        .matrix = {
            .total_cnt   = MATRIX_COLS * MATRIX_ROWS,
            .col_cnt     = MATRIX_COLS,
            .row_cnt     = MATRIX_ROWS,
            .col_pins    = matrix_col_pins,
            .row_pins    = matrix_row_pins,
            .layout_from = (const uint8_t *)matrix_to_layout
        },
        .layout = {
            .total_cnt = LAYOUT_COLS * LAYOUT_ROWS,
            .col_cnt   = LAYOUT_COLS,
            .row_cnt   = LAYOUT_ROWS
        },
        .scan = {
            .scan_period_ms = 2,
            .max_jitter_ms  = 10,
            .debounce_algo  = SMK_KEYBOARD_DEBOUNCE_ALGO_DEFER_G
        },
        .map = {
            .layer_cnt     = LAYERS,
            .default_layer = 0U,
            .keymaps       = (const smk_keycode_type *)smk_default_matrix_keymaps
        }
    };

    return &hardware;
}

int smk_keyscan_init_gpio(const smk_keyboard_hardware_type *hardware)
{
    for (uint8_t col = 0; col < hardware->matrix.col_cnt; ++col) {
        uint8_t pin = hardware->matrix.col_pins[col];
        gpio_set_mode(pin, GPIO_INPUT_PP_MODE);
    }
    
    for (uint8_t row = 0; row < hardware->matrix.row_cnt; ++row) {
        uint8_t pin = hardware->matrix.row_pins[row];
        gpio_set_mode(pin, GPIO_INPUT_PP_MODE);
    }

    return 0;
}

void smk_keyscan_select_col(const smk_keyboard_hardware_type *hardware, uint32_t col)
{
    uint8_t pin = hardware->matrix.col_pins[col];
    gpio_write(pin, 0U);
    gpio_set_mode(pin, GPIO_OUTPUT_MODE);
}

void smk_keyscan_unselect_col(const smk_keyboard_hardware_type *hardware, uint32_t col)
{
    uint8_t pin = hardware->matrix.col_pins[col];
    gpio_set_mode(pin, GPIO_INPUT_PP_MODE);
}

void smk_keyscan_select_col_delay(const smk_keyboard_hardware_type *hardware)
{
    bflb_platform_delay_us(0);
}

void smk_keyscan_unselect_col_delay(const smk_keyboard_hardware_type *hardware)
{
    bflb_platform_delay_us(5);
}

uint32_t smk_keyscan_read_entire_row(const smk_keyboard_hardware_type *hardware)
{
    // Here we read GPIO0-GPIO6 in one-time register read
    return ~*(uint32_t *)(GLB_BASE + GLB_GPIO_INPUT_OFFSET) & 0x7FU;
}

// uint32_t smk_keyscan_read_row_gpio(const smk_keyboard_hardware_type *hardware, uint32_t row)
// {
//     uint8_t pin = hardware->matrix.row_pins[row];
//     return (uint32_t)gpio_read(pin);
// }
