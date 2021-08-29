#ifndef __SMK_KEYCODE_H
#define __SMK_KEYCODE_H

typedef enum {
    KC_NO              = 0x00, // Reserved (no event indicated)
    KC_ROLLOVER        = 0x01, // Keyboard ErrorRollOver
    KC_FAIL            = 0x02, // Keyboard POSTFail
    KC_UNDEFINED       = 0x03, // Keyboard ErrorUndefined
    KC_A               = 0x04, // Keyboard a and A
    KC_B               = 0x05, // Keyboard b and B
    KC_C               = 0x06, // Keyboard c and C
    KC_D               = 0x07, // Keyboard d and D
    KC_E               = 0x08, // Keyboard e and E
    KC_F               = 0x09, // Keyboard f and F
    KC_G               = 0x0A, // Keyboard g and G
    KC_H               = 0x0B, // Keyboard h and H
    KC_I               = 0x0C, // Keyboard i and I
    KC_J               = 0x0D, // Keyboard j and J
    KC_K               = 0x0E, // Keyboard k and K
    KC_L               = 0x0F, // Keyboard l and L
    KC_M               = 0x10, // Keyboard m and M
    KC_N               = 0x11, // Keyboard n and N
    KC_O               = 0x12, // Keyboard o and O
    KC_P               = 0x13, // Keyboard p and P
    KC_Q               = 0x14, // Keyboard q and Q
    KC_R               = 0x15, // Keyboard r and R
    KC_S               = 0x16, // Keyboard s and S
    KC_T               = 0x17, // Keyboard t and T
    KC_U               = 0x18, // Keyboard u and U
    KC_V               = 0x19, // Keyboard v and V
    KC_W               = 0x1A, // Keyboard w and W
    KC_X               = 0x1B, // Keyboard x and X
    KC_Y               = 0x1C, // Keyboard y and Y
    KC_Z               = 0x1D, // Keyboard z and Z
    KC_1               = 0x1E, // Keyboard 1 and !
    KC_2               = 0x1F, // Keyboard 2 and @
    KC_3               = 0x20, // Keyboard 3 and #
    KC_4               = 0x21, // Keyboard 4 and $
    KC_5               = 0x22, // Keyboard 5 and %
    KC_6               = 0x23, // Keyboard 6 and ^
    KC_7               = 0x24, // Keyboard 7 and &
    KC_8               = 0x25, // Keyboard 8 and *
    KC_9               = 0x26, // Keyboard 9 and (
    KC_0               = 0x27, // Keyboard 0 and )
    KC_ENTER           = 0x28, // Keyboard Return (ENTER)
    KC_ESCAPE          = 0x29, // Keyboard ESCAPE
    KC_BACKSPACE       = 0x2A, // Keyboard DELETE (Backspace)
    KC_TAB             = 0x2B, // Keyboard Tab
    KC_SPACEBAR        = 0x2C, // Keyboard Spacebar
    KC_MINUS           = 0x2D, // Keyboard - and (underscore)
    KC_EQUAL           = 0x2E, // Keyboard = and +
    KC_LBRACKET        = 0x2F, // Keyboard [ and {
    KC_RBRACKET        = 0x30, // Keyboard ] and }
    KC_BACKSLASH       = 0x31, // Keyboard \ and |
    KC_NONUS_HASH      = 0x32, // Keyboard Non-US # and ~
    KC_SEMICOLON       = 0x33, // Keyboard ; and :
    KC_QUOTE           = 0x34, // Keyboard ' and "
    KC_GRAVE           = 0x35, // Keyboard Grave Accent and Tilde
    KC_COMMA           = 0x36, // Keyboard , and <
    KC_DOT             = 0x37, // Keyboard . and >
    KC_SLASH           = 0x38, // Keyboard / and ?
    KC_CAPSLOCK        = 0x39, // Keyboard Caps Lock
    KC_F1              = 0x3A, // Keyboard F1
    KC_F2              = 0x3B, // Keyboard F2
    KC_F3              = 0x3C, // Keyboard F3
    KC_F4              = 0x3D, // Keyboard F4
    KC_F5              = 0x3E, // Keyboard F5
    KC_F6              = 0x3F, // Keyboard F6
    KC_F7              = 0x40, // Keyboard F7
    KC_F8              = 0x41, // Keyboard F8
    KC_F9              = 0x42, // Keyboard F9
    KC_F10             = 0x43, // Keyboard F10
    KC_F11             = 0x44, // Keyboard F11
    KC_F12             = 0x45, // Keyboard F12
    KC_PRINTSCREEN     = 0x46, // Keyboard PrintScreen
    KC_SCROLLLOCK      = 0x47, // Keyboard Scroll Lock
    KC_PAUSE           = 0x48, // Keyboard Pause
    KC_INSERT          = 0x49, // Keyboard Insert
    KC_HOME            = 0x4A, // Keyboard Home
    KC_PAGEUP          = 0x4B, // Keyboard PageUp
    KC_DELETE          = 0x4C, // Keyboard Delete Forward
    KC_END             = 0x4D, // Keyboard End
    KC_PAGEDOWN        = 0x4E, // Keyboard PageDown
    KC_RIGHT           = 0x4F, // Keyboard RightArrow
    KC_LEFT            = 0x50, // Keyboard LeftArrow
    KC_DOWN            = 0x51, // Keyboard DownArrow
    KC_UP              = 0x52, // Keyboard UpArrow
    KC_NUMLOCK         = 0x53, // Keypad Num Lock and Clear
    KC_KP_SLASH        = 0x54, // Keypad /
    KC_KP_ASTERISK     = 0x55, // Keypad *
    KC_KP_MINUS        = 0x56, // Keypad -
    KC_KP_PLUS         = 0x57, // Keypad +
    KC_KP_ENTER        = 0x58, // Keypad ENTER
    KC_KP_1            = 0x59, // Keypad 1 and End
    KC_KP_2            = 0x5A, // Keypad 2 and Down Arrow
    KC_KP_3            = 0x5B, // Keypad 3 and PageDn
    KC_KP_4            = 0x5C, // Keypad 4 and Left Arrow
    KC_KP_5            = 0x5D, // Keypad 5
    KC_KP_6            = 0x5E, // Keypad 6 and Right Arrow
    KC_KP_7            = 0x5F, // Keypad 7 and Home
    KC_KP_8            = 0x60, // Keypad 8 and Up Arrow
    KC_KP_9            = 0x61, // Keypad 9 and PageUp
    KC_KP_0            = 0x62, // Keypad 0 and Insert
    KC_KP_DOT          = 0x63, // Keypad . and Delete
    KC_NONUS_BACKSLASH = 0x64, // Keyboard Non-US \ and |
    KC_APPLICATION     = 0x65, // Keyboard Application
    // 0x66-0xDF is reserved for further use
    KC_LCTRL           = 0xE0, // Keyboard LeftControl
    KC_LSHIFT          = 0xE1, // Keyboard LeftShift
    KC_LALT            = 0xE2, // Keyboard LeftAlt
    KC_LGUI            = 0xE3, // Keyboard Left GUI
    KC_RCTRL           = 0xE4, // Keyboard RightControl
    KC_RSHIFT          = 0xE5, // Keyboard RightShift
    KC_RALT            = 0xE6, // Keyboard RightAlt
    KC_RGUI            = 0xE7, // Keyboard Right GUI
    // 0xE0-0xFF is reserved for further use
} keycode_type;

#define IS_MOD_KEYS(keycode) (KC_LCTRL <= (keycode) && (keycode) <= KC_RGUI)

#define KC_TRANSPARENT 0x01
#define KC_TRANS       KC_TRANSPARENT

// Short name version of keycode, which must be used with attention.
// It would be better to use them in particular source code, and not
// mixed with code other than the keymap definition. Every keycode has
// at least one short name, which has a length of no more than 5.

#ifdef USE_KEYCODE_SHORT_NAME

#define XXXXX KC_NO
#define _____ KC_TRANS

#define A         KC_A
#define B         KC_B
#define C         KC_C
#define D         KC_D
#define E         KC_E
#define F         KC_F
#define G         KC_G
#define H         KC_H
#define I         KC_I
#define J         KC_J
#define K         KC_K
#define L         KC_L
#define M         KC_M
#define N         KC_N
#define O         KC_O
#define P         KC_P
#define Q         KC_Q
#define R         KC_R
#define S         KC_S
#define T         KC_T
#define U         KC_U
#define V         KC_V
#define W         KC_W
#define X         KC_X
#define Y         KC_Y
#define Z         KC_Z
#define NUM0      KC_0
#define Num0      KC_0
#define NUM1      KC_1
#define Num1      KC_1
#define NUM2      KC_2
#define Num2      KC_2
#define NUM3      KC_3
#define Num3      KC_3
#define NUM4      KC_4
#define Num4      KC_4
#define NUM5      KC_5
#define Num5      KC_5
#define NUM6      KC_6
#define Num6      KC_6
#define NUM7      KC_7
#define Num7      KC_7
#define NUM8      KC_8
#define Num8      KC_8
#define NUM9      KC_9
#define Num9      KC_9
#define ENTER     KC_ENTER
#define Enter     KC_ENTER
#define ESC       KC_ESCAPE
#define Esc       KC_ESCAPE
#define BSPC      KC_BACKSPACE
#define BSPACE    KC_BACKSPACE
#define BACKSPACE KC_BACKSPACE
#define Backspace KC_BACKSPACE
#define TAB       KC_TAB
#define Tab       KC_TAB
#define SPC       KC_SPACEBAR
#define SPACE     KC_SPACEBAR
#define Space     KC_SPACEBAR
#define MINUS     KC_MINUS
#define Minus     KC_MINUS
#define EQUAL     KC_EQUAL
#define Equal     KC_EQUAL
#define LBRKT     KC_LBRACKET
#define LBrkt     KC_LBRACKET
#define RBRKT     KC_RBRACKET
#define RBrkt     KC_RBRACKET
#define BSLS      KC_BACKSLASH
#define BSLSH     KC_BACKSLASH
#define BSLASH    KC_BACKSLASH
#define BSlash    KC_BACKSLASH
#define BACKSLASH KC_BACKSLASH
#define BackSlash KC_BACKSLASH
#define SCLN      KC_SEMICOLON
#define SCOLN     KC_SEMICOLON
#define SColn     KC_SEMICOLON
#define QUOTE     KC_QUOTE
#define Quote     KC_QUOTE
#define GRAVE     KC_GRAVE
#define Grave     KC_GRAVE
#define COMMA     KC_COMMA
#define Comma     KC_COMMA
#define DOT       KC_DOT
#define Dot       KC_DOT
#define SLASH     KC_SLASH
#define Slash     KC_SLASH
#define CAPS      KC_CAPSLOCK
#define CAPSLOCK  KC_CAPSLOCK
#define CapsLock  KC_CAPSLOCK
#define F1        KC_F1
#define F2        KC_F2
#define F3        KC_F3
#define F4        KC_F4
#define F5        KC_F5
#define F6        KC_F6
#define F7        KC_F7
#define F8        KC_F8
#define F9        KC_F9
#define F10       KC_F10
#define F11       KC_F11
#define F12       KC_F12
#define PRTSC     KC_PRINTSCREEN
#define PrtSc     KC_PRINTSCREEN
#define SCRLK     KC_SCROLLLOCK
#define ScrLk     KC_SCROLLLOCK
#define PAUSE     KC_PAUSE
#define Pause     KC_PAUSE
#define INS       KC_INSERT
#define Ins       KC_INSERT
#define INSRT     KC_INSERT
#define HOME      KC_HOME
#define Home      KC_HOME
#define PGUP      KC_PAGEUP
#define PgUp      KC_PAGEUP
#define DEL       KC_DELETE
#define Del       KC_DELETE
#define END       KC_END
#define End       KC_END
#define PGDN      KC_PAGEDOWN
#define PgDn      KC_PAGEDOWN
#define RIGHT     KC_RIGHT
#define Right     KC_RIGHT
#define LEFT      KC_LEFT
#define Left      KC_LEFT
#define DOWN      KC_DOWN
#define Down      KC_DOWN
#define UP        KC_UP
#define Up        KC_UP
#define NUMLK     KC_NUMLOCK
#define NumLk     KC_NUMLOCK
#define PSLS      KC_KP_SLASH
#define PSLSH     KC_KP_SLASH
#define slash     KC_KP_SLASH
#define PAST      KC_KP_ASTERISK
#define ast       KC_KP_ASTERISK
#define PMNS      KC_KP_MINUS
#define PMINUS    KC_KP_MINUS
#define minus     KC_KP_MINUS
#define PPLS      KC_KP_PLUS
#define PPLUS     KC_KP_PLUS
#define plus      KC_KP_PLUS
#define PENT      KC_KP_ENTER
#define PENTER    KC_KP_ENTER
#define enter     KC_KP_ENTER
#define P1        KC_KP_1
#define p1        KC_KP_1
#define P2        KC_KP_2
#define p2        KC_KP_2
#define P3        KC_KP_3
#define p3        KC_KP_3
#define P4        KC_KP_4
#define p4        KC_KP_4
#define P5        KC_KP_5
#define p5        KC_KP_5
#define P6        KC_KP_6
#define p6        KC_KP_6
#define P7        KC_KP_7
#define p7        KC_KP_7
#define P8        KC_KP_8
#define p8        KC_KP_8
#define P9        KC_KP_9
#define p9        KC_KP_9
#define P0        KC_KP_0
#define p0        KC_KP_0
#define PDOT      KC_KP_DOT
#define dot       KC_KP_DOT
#define LCTL      KC_LCTRL
#define LCTRL     KC_LCTRL
#define LCtrl     KC_LCTRL
#define LSFT      KC_LSHIFT
#define LSHFT     KC_LSHIFT
#define LShft     KC_LSHIFT
#define LSHIFT    KC_LSHIFT
#define LShift    KC_LSHIFT
#define LALT      KC_LALT
#define LAlt      KC_LALT
#define LOPT      KC_LALT
#define LOpt      KC_LALT
#define LGUI      KC_LGUI
#define LCMD      KC_LGUI
#define LCmd      KC_LGUI
#define RCTL      KC_RCTRL
#define RCTRL     KC_RCTRL
#define RCtrl     KC_RCTRL
#define RSFT      KC_RSHIFT
#define RSHFT     KC_RSHIFT
#define RShft     KC_RSHIFT
#define RSHIFT    KC_RSHIFT
#define RShift    KC_RSHIFT
#define RALT      KC_RALT
#define RAlt      KC_RALT
#define ROPT      KC_RALT
#define ROpt      KC_RALT
#define RGUI      KC_RGUI
#define RCMD      KC_RGUI
#define RCmd      KC_RGUI

#endif

#include <stdint.h>

typedef uint16_t smk_keycode_type;

typedef enum {
    BASIC_KEYCODE                 = 0x0000,
    BASIC_KEYCODE_MAX             = 0x00FF,
    DEFAULT_LAYER                 = 0x0100,
    DEFAULT_LAYER_MAX             = 0x01FF,
    MOMENTARILY_ACTIVES_LAYER     = 0x0200,
    MOMENTARILY_ACTIVES_LAYER_NAX = 0x02FF
} smk_comb_keycode_type;

#define LAYER_TYPE_MASK(keycode) ((keycode) & 0xFF00)

#define MO(n) (MOMENTARILY_ACTIVES_LAYER + (n))
#define MO_LAYER(keycode) ((keycode) & 0xFF)

#define DF(n) (DEFAULT_LAYER + (n))
#define DF_LAYER(keycode) ((keycode) & 0xFF)

#endif
