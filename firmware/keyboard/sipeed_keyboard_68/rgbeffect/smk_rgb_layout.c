#include "smk_spirgb.h"

const RGB_KEY_DESC rgb_key_descriptor[RGB_LENGTH] = {
   {	// LCTRL
        .xpos = 1.60 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 0,
        .keycode = 0xFF,
        .keyleft = 0xFF,
        .keyright = 1
    },
    {	// LWIN
        .xpos = 4.00 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 1,
        .keycode = 0xFF,
        .keyleft = 0,
        .keyright = 2
    },
    {	// LALT
        .xpos = 6.40 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 2,
        .keycode = 0xFF,
        .keyleft = 1,
        .keyright = 3
    },
    {	// SPACE
        .xpos = 13.60 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 3,
        .keycode = 0xFF,
        .keyleft = 2,
        .keyright = 4
    },
    {	// RALT
        .xpos = 20.50 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 4,
        .keycode = 0xFF,
        .keyleft = 3,
        .keyright = 5
    },
    {	// FN
        .xpos = 22.40 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 5,
        .keycode = 0xFF,
        .keyleft = 4,
        .keyright = 6
    },
    {	// RCTRL
        .xpos = 24.30 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 6,
        .keycode = 0xFF,
        .keyleft = 5,
        .keyright = 7
    },
    {	// LARROW
        .xpos = 26.20 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 7,
        .keycode = 0xFF,
        .keyleft = 6,
        .keyright = 8
    },
    {	// DARROW
        .xpos = 28.10 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 8,
        .keycode = 0xFF,
        .keyleft = 7,
        .keyright = 9
    },
    {	// RARROW
        .xpos = 30.00 * 256,
        .ypos = 7.60 * 256,
        .scanseq = 9,
        .keycode = 0xFF,
        .keyleft = 8,
        .keyright = 0xFF
    },
    {	// PGDN
        .xpos = 30.00 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 10,
        .keycode = 0xFF,
        .keyleft = 11,
        .keyright = 0xFF
    },
    {	// UARROW
        .xpos = 28.10 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 11,
        .keycode = 0xFF,
        .keyleft = 12,
        .keyright = 10
    },
    {	// RSHIFT
        .xpos = 25.50 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 12,
        .keycode = 0xFF,
        .keyleft = 13,
        .keyright = 11
    },
    {	// SLASH
        .xpos = 22.90 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 13,
        .keycode = 0xFF,
        .keyleft = 14,
        .keyright = 12
    },
    {	// DOT
        .xpos = 21.00 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 14,
        .keycode = 0xFF,
        .keyleft = 15,
        .keyright = 13
    },
    {	// COMMA
        .xpos = 19.10 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 15,
        .keycode = 0xFF,
        .keyleft = 16,
        .keyright = 14
    },
    {	// KEYNAME
        .xpos = 17.20 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 16,
        .keycode = 0xFF,
        .keyleft = 17,
        .keyright = 15
    },
    {	// KEYNAME
        .xpos = 15.30 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 17,
        .keycode = 0xFF,
        .keyleft = 18,
        .keyright = 16
    },
    {	// KEYNAME
        .xpos = 13.40 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 18,
        .keycode = 0xFF,
        .keyleft = 19,
        .keyright = 17
    },
    {	// KEYNAME
        .xpos = 11.50 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 19,
        .keycode = 0xFF,
        .keyleft = 20,
        .keyright = 18
    },
    {	// KEYNAME
        .xpos = 9.60 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 20,
        .keycode = 0xFF,
        .keyleft = 21,
        .keyright = 19
    },
    {	// KEYNAME
        .xpos = 7.70 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 21,
        .keycode = 0xFF,
        .keyleft = 22,
        .keyright = 20
    },
    {	// KEYNAME
        .xpos = 5.80 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 22,
        .keycode = 0xFF,
        .keyleft = 23,
        .keyright = 21
    },
    {	// KEYNAME
        .xpos = 2.70 * 256,
        .ypos = 5.70 * 256,
        .scanseq = 23,
        .keycode = 0xFF,
        .keyleft = 0xFF,
        .keyright = 22
    },
    {	// KEYNAME
        .xpos = 2.20 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 24,
        .keycode = 0xFF,
        .keyleft = 0xFF,
        .keyright = 25
    },
    {	// KEYNAME
        .xpos = 4.80 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 25,
        .keycode = 0xFF,
        .keyleft = 24,
        .keyright = 26
    },
    {	// KEYNAME
        .xpos = 6.70 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 26,
        .keycode = 0xFF,
        .keyleft = 25,
        .keyright = 27
    },
    {	// KEYNAME
        .xpos = 8.60 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 27,
        .keycode = 0xFF,
        .keyleft = 26,
        .keyright = 28
    },
    {	// KEYNAME
        .xpos = 10.50 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 28,
        .keycode = 0xFF,
        .keyleft = 27,
        .keyright = 29
    },
    {	// KEYNAME
        .xpos = 12.40 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 29,
        .keycode = 0xFF,
        .keyleft = 28,
        .keyright = 30
    },
    {	// KEYNAME
        .xpos = 14.30 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 30,
        .keycode = 0xFF,
        .keyleft = 29,
        .keyright = 31
    },
    {	// KEYNAME
        .xpos = 16.20 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 31,
        .keycode = 0xFF,
        .keyleft = 30,
        .keyright = 32
    },
    {	// KEYNAME
        .xpos = 18.10 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 32,
        .keycode = 0xFF,
        .keyleft = 31,
        .keyright = 33
    },
    {	// KEYNAME
        .xpos = 20.00 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 33,
        .keycode = 0xFF,
        .keyleft = 32,
        .keyright = 34
    },
    {	// KEYNAME
        .xpos = 21.90 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 34,
        .keycode = 0xFF,
        .keyleft = 33,
        .keyright = 35
    },
    {	// KEYNAME
        .xpos = 23.80 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 35,
        .keycode = 0xFF,
        .keyleft = 34,
        .keyright = 36
    },
    {	// KEYNAME
        .xpos = 26.90 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 36,
        .keycode = 0xFF,
        .keyleft = 35,
        .keyright = 37
    },
    {	// KEYNAME
        .xpos = 30.00 * 256,
        .ypos = 3.80 * 256,
        .scanseq = 37,
        .keycode = 0xFF,
        .keyleft = 36,
        .keyright = 0xFF
    },
    {	// KEYNAME
        .xpos = 30.00 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 38,
        .keycode = 0xFF,
        .keyleft = 39,
        .keyright = 0xFF
    },
    {	// KEYNAME
        .xpos = 27.60 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 39,
        .keycode = 0xFF,
        .keyleft = 40,
        .keyright = 38
    },
    {	// KEYNAME
        .xpos = 25.20 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 40,
        .keycode = 0xFF,
        .keyleft = 41,
        .keyright = 39
    },
    {	// KEYNAME
        .xpos = 23.30 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 41,
        .keycode = 0xFF,
        .keyleft = 42,
        .keyright = 40
    },
    {	// KEYNAME
        .xpos = 21.40 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 42,
        .keycode = 0xFF,
        .keyleft = 43,
        .keyright = 41
    },
    {	// KEYNAME
        .xpos = 19.50 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 43,
        .keycode = 0xFF,
        .keyleft = 44,
        .keyright = 42
    },
    {	// KEYNAME
        .xpos = 17.60 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 44,
        .keycode = 0xFF,
        .keyleft = 45,
        .keyright = 43
    },
    {	// KEYNAME
        .xpos = 15.70 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 45,
        .keycode = 0xFF,
        .keyleft = 46,
        .keyright = 44
    },
    {	// KEYNAME
        .xpos = 13.80 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 46,
        .keycode = 0xFF,
        .keyleft = 47,
        .keyright = 45
    },
    {	// KEYNAME
        .xpos = 11.90 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 47,
        .keycode = 0xFF,
        .keyleft = 48,
        .keyright = 46
    },
    {	// KEYNAME
        .xpos = 10.00 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 48,
        .keycode = 0xFF,
        .keyleft = 49,
        .keyright = 47
    },
    {	// KEYNAME
        .xpos = 8.10 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 49,
        .keycode = 0xFF,
        .keyleft = 50,
        .keyright = 48
    },
    {	// KEYNAME
        .xpos = 6.20 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 50,
        .keycode = 0xFF,
        .keyleft = 51,
        .keyright = 49
    },
    {	// KEYNAME
        .xpos = 4.30 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 51,
        .keycode = 0xFF,
        .keyleft = 52,
        .keyright = 50
    },
    {	// KEYNAME
        .xpos = 1.90 * 256,
        .ypos = 1.90 * 256,
        .scanseq = 52,
        .keycode = 0xFF,
        .keyleft = 0xFF,
        .keyright = 51
    },
    {	// KEYNAME
        .xpos = 1.50 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 53,
        .keycode = 0xFF,
        .keyleft = 0xFF,
        .keyright = 54
    },
    {	// KEYNAME
        .xpos = 3.40 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 54,
        .keycode = 0xFF,
        .keyleft = 53,
        .keyright = 55
    },
    {	// KEYNAME
        .xpos = 5.30 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 55,
        .keycode = 0xFF,
        .keyleft = 54,
        .keyright = 56
    },
    {	// KEYNAME
        .xpos = 7.20 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 56,
        .keycode = 0xFF,
        .keyleft = 55,
        .keyright = 57
    },
    {	// KEYNAME
        .xpos = 9.10 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 57,
        .keycode = 0xFF,
        .keyleft = 56,
        .keyright = 58
    },
    {	// KEYNAME
        .xpos = 11.00 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 58,
        .keycode = 0xFF,
        .keyleft = 57,
        .keyright = 59
    },
    {	// KEYNAME
        .xpos = 12.90 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 59,
        .keycode = 0xFF,
        .keyleft = 58,
        .keyright = 60
    },
    {	// KEYNAME
        .xpos = 14.80 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 60,
        .keycode = 0xFF,
        .keyleft = 59,
        .keyright = 61
    },
    {	// KEYNAME
        .xpos = 16.70 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 61,
        .keycode = 0xFF,
        .keyleft = 60,
        .keyright = 62
    },
    {	// KEYNAME
        .xpos = 18.60 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 62,
        .keycode = 0xFF,
        .keyleft = 61,
        .keyright = 63
    },
    {	// KEYNAME
        .xpos = 20.50 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 63,
        .keycode = 0xFF,
        .keyleft = 62,
        .keyright = 64
    },
    {	// KEYNAME
        .xpos = 22.40 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 64,
        .keycode = 0xFF,
        .keyleft = 63,
        .keyright = 65
    },
    {	// KEYNAME
        .xpos = 24.30 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 65,
        .keycode = 0xFF,
        .keyleft = 64,
        .keyright = 66
    },
    {	// KEYNAME
        .xpos = 27.15 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 66,
        .keycode = 0xFF,
        .keyleft = 65,
        .keyright = 67
    },
    {	// KEYNAME
        .xpos = 30.00 * 256,
        .ypos = 0.00 * 256,
        .scanseq = 67,
        .keycode = 0xFF,
        .keyleft = 66,
        .keyright = 0xFF
    }
};