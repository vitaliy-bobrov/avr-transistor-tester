/* ************************************************************************
 *
 *   color definitions for display modules
 *
 *   (c) 2015-2023 by Markus Reschke
 *
 * ************************************************************************ */


/* source management */
#define COLORS_H


/* ************************************************************************
 *   display specific color values
 * ************************************************************************ */


/*
 *  ILI9341/ILI9342
 *  - RGB565
 */

#ifdef LCD_ILI9341
  /* common colors */
  #define COLOR_BLACK         0x0000
  #define COLOR_BLUE          0x117a
  #define COLOR_GREEN         0x0c23
  #define COLOR_MATRIX_GREEN  0x07e8
  #define COLOR_MATRIX_GREEN_ALT 0x0727
  #define COLOR_MATRIX_DARK_GREEN 0x0646
  #define COLOR_CYAN          0x07fd
  #define COLOR_RED           0xf880
  #define COLOR_MAGENTA       0xe90f
  #define COLOR_YELLOW        0xff40
  #define COLOR_ORANGE        0xfce1    /* RGB ef9a42 */
  #define COLOR_GREY          0xcedb    /* RGB cedbde */
  #define COLOR_WHITE         0xffff    /* RGB FFFFFF */
  #define COLOR_BROWN         0x7a27    /* RGB 8c615a */
  #define COLOR_VIOLET        0xa112    /* RGB a5459c */
  #define COLOR_PALE_YELLOW   0xffec    /* RGB ffb600 */
  #define COLOR_PALE_GREEN    0x07f2    /* RGB 008e10 */
  #define COLOR_PALE_RED      0xb1ca    /* RGB b53952 */
  #define COLOR_STEEL_BLUE    0x0c7a    /* RGB 088ed6 */
  #define COLOR_SKY_BLUE      0x06ff    /* RGB 00dfff */

  /* component color codes */
  #define COLOR_CODE_BLACK    0x0821
  #define COLOR_CODE_BROWN    COLOR_BROWN
  #define COLOR_CODE_RED      COLOR_RED
  #define COLOR_CODE_ORANGE   COLOR_ORANGE
  #define COLOR_CODE_YELLOW   COLOR_YELLOW
  #define COLOR_CODE_GREEN    0x0660
  #define COLOR_CODE_BLUE     COLOR_BLUE
  #define COLOR_CODE_VIOLET   COLOR_VIOLET
  #define COLOR_CODE_GREY     0x738d    /* RGB F0F0F0 */
  #define COLOR_CODE_WHITE    COLOR_WHITE
  #define COLOR_CODE_GOLD     0xc4c4    /* RGB F6C337 */
  #define COLOR_CODE_SILVER   0xf79e    /* RGB E0E0E0 */
  /* component's body color */
  #define COLOR_CODE_NONE     0xf7de
#endif


/*
 *  ILI9163
 *  - RGB565
 */

#ifdef LCD_ILI9163
  /* common colors */
  #define COLOR_BLACK         0x0000    /* RGB 000000 */
  #define COLOR_BLUE          0x001f    /* RGB 0000F8 */
  #define COLOR_GREEN         0x07e0    /* RGB 00FC00 */
  #define COLOR_CYAN          0x07ff    /* RGB 00FCF8 */
  #define COLOR_RED           0xf800    /* RGB F80000 */
  #define COLOR_MAGENTA       0xf81f    /* RGB F800F8 */
  #define COLOR_YELLOW        0xffe0    /* RGB F8FC00 */
  #define COLOR_ORANGE        0xfd20    /* RGB F8A400 */
  #define COLOR_GREY          0xc618    /* RGB C0C0C0 */
  #define COLOR_WHITE         0xffff    /* RGB FFFFFF */
  #define COLOR_BROWN         0x6920    /* RGB 682400 */
  #define COLOR_VIOLET        0x780f    /* RGB 780078 */
  #define COLOR_PALE_YELLOW   0xe70c    /* RGB E2E164 */
  #define COLOR_PALE_GREEN    0x9fec    /* RGB 99FF66 */
  #define COLOR_PALE_RED      0xfb6d    /* RGB FF6F6F */
  #define COLOR_STEEL_BLUE    0x64df    /* RGB 6098F8 */
  #define COLOR_SKY_BLUE      0x06ff    /* RGB 06DCF9 */

  /* component color codes */
  #define COLOR_CODE_BLACK    0x0000    /* RGB 000000 */
  #define COLOR_CODE_BROWN    0xcb25    /* RGB CE652C */
  #define COLOR_CODE_RED      0xf800    /* RGB F80000 */
  #define COLOR_CODE_ORANGE   0xfe40    /* RGB FFCB00 */
  #define COLOR_CODE_YELLOW   0xffe0    /* RGB F8FC00 */
  #define COLOR_CODE_GREEN    0x05c0    /* RGB 00BB00 */
  #define COLOR_CODE_BLUE     0x001f    /* RGB 0000F8 */
  #define COLOR_CODE_VIOLET   0xca19    /* RGB CF40CF */
  #define COLOR_CODE_GREY     0xf79e    /* RGB F0F0F0 */
  #define COLOR_CODE_WHITE    0xffff    /* RGB FFFFFF */
  #define COLOR_CODE_GOLD     0xf606    /* RGB F6C337 */
  #define COLOR_CODE_SILVER   0xe71c    /* RGB E0E0E0 */
  /* component's body color */
  #define COLOR_CODE_NONE     0x06ff    /* RGB 06DCF9 */
#endif


/*
 *  ILI9481
 *  - RGB565
 */

#ifdef LCD_ILI9481
  /* common colors */
  #define COLOR_BLACK         0x0000    /* RGB 000000 */
  #define COLOR_BLUE          0x001f    /* RGB 0000F8 */
  #define COLOR_GREEN         0x07e0    /* RGB 00FC00 */
  #define COLOR_CYAN          0x07ff    /* RGB 00FCF8 */
  #define COLOR_RED           0xf800    /* RGB F80000 */
  #define COLOR_MAGENTA       0xf81f    /* RGB F800F8 */
  #define COLOR_YELLOW        0xffe0    /* RGB F8FC00 */
  #define COLOR_ORANGE        0xfd20    /* RGB F8A400 */
  #define COLOR_GREY          0xc618    /* RGB C0C0C0 */
  #define COLOR_WHITE         0xffff    /* RGB FFFFFF */
  #define COLOR_BROWN         0x6920    /* RGB 682400 */
  #define COLOR_VIOLET        0x780f    /* RGB 780078 */
  #define COLOR_PALE_YELLOW   0xe70c    /* RGB E2E164 */
  #define COLOR_PALE_GREEN    0x9fec    /* RGB 99FF66 */
  #define COLOR_PALE_RED      0xfb6d    /* RGB FF6F6F */
  #define COLOR_STEEL_BLUE    0x64df    /* RGB 6098F8 */
  #define COLOR_SKY_BLUE      0x06ff    /* RGB 06DCF9 */

  /* component color codes */
  #define COLOR_CODE_BLACK    0x0000    /* RGB 000000 */
  #define COLOR_CODE_BROWN    0xcb25    /* RGB CE652C */
  #define COLOR_CODE_RED      0xf800    /* RGB F80000 */
  #define COLOR_CODE_ORANGE   0xfe40    /* RGB FFCB00 */
  #define COLOR_CODE_YELLOW   0xffe0    /* RGB F8FC00 */
  #define COLOR_CODE_GREEN    0x07e0    /* RGB 00FC00 */
  #define COLOR_CODE_BLUE     0x001f    /* RGB 0000F8 */
  #define COLOR_CODE_VIOLET   0xca19    /* RGB CF40CF */
  #define COLOR_CODE_GREY     0xf79e    /* RGB F0F0F0 */
  #define COLOR_CODE_WHITE    0xffff    /* RGB FFFFFF */
  #define COLOR_CODE_GOLD     0xf606    /* RGB F6C337 */
  #define COLOR_CODE_SILVER   0xe71c    /* RGB E0E0E0 */
  /* component's body color */
  #define COLOR_CODE_NONE     0x06ff    /* RGB 06DCF9 */
#endif


/*
 *  ILI9486
 *  - RGB565
 */

#ifdef LCD_ILI9486
  /* common colors */
  #define COLOR_BLACK         0x0000    /* RGB 000000 */
  #define COLOR_BLUE          0x001f    /* RGB 0000F8 */
  #define COLOR_GREEN         0x07e0    /* RGB 00FC00 */
  #define COLOR_CYAN          0x07ff    /* RGB 00FCF8 */
  #define COLOR_RED           0xf800    /* RGB F80000 */
  #define COLOR_MAGENTA       0xf81f    /* RGB F800F8 */
  #define COLOR_YELLOW        0xffe0    /* RGB F8FC00 */
  #define COLOR_ORANGE        0xfd20    /* RGB F8A400 */
  #define COLOR_GREY          0xc618    /* RGB C0C0C0 */
  #define COLOR_WHITE         0xffff    /* RGB FFFFFF */
  #define COLOR_BROWN         0x6920    /* RGB 682400 */
  #define COLOR_VIOLET        0x780f    /* RGB 780078 */
  #define COLOR_PALE_YELLOW   0xe70c    /* RGB E2E164 */
  #define COLOR_PALE_GREEN    0x9fec    /* RGB 99FF66 */
  #define COLOR_PALE_RED      0xfb6d    /* RGB FF6F6F */
  #define COLOR_STEEL_BLUE    0x64df    /* RGB 6098F8 */
  #define COLOR_SKY_BLUE      0x06ff    /* RGB 06DCF9 */

  /* component color codes */
  #define COLOR_CODE_BLACK    0x0000    /* RGB 000000 */
  #define COLOR_CODE_BROWN    0xcb25    /* RGB CE652C */
  #define COLOR_CODE_RED      0xf800    /* RGB F80000 */
  #define COLOR_CODE_ORANGE   0xfe40    /* RGB FFCB00 */
  #define COLOR_CODE_YELLOW   0xffe0    /* RGB F8FC00 */
  #define COLOR_CODE_GREEN    0x07e0    /* RGB 00FC00 */
  #define COLOR_CODE_BLUE     0x001f    /* RGB 0000F8 */
  #define COLOR_CODE_VIOLET   0xca19    /* RGB CF40CF */
  #define COLOR_CODE_GREY     0xf79e    /* RGB F0F0F0 */
  #define COLOR_CODE_WHITE    0xffff    /* RGB FFFFFF */
  #define COLOR_CODE_GOLD     0xf606    /* RGB F6C337 */
  #define COLOR_CODE_SILVER   0xe71c    /* RGB E0E0E0 */
  /* component's body color */
  #define COLOR_CODE_NONE     0x06ff    /* RGB 06DCF9 */
#endif


/*
 *  ILI9488
 *  - RGB565
 */

#ifdef LCD_ILI9488
  /* common colors */
  #define COLOR_BLACK         0x0000    /* RGB 000000 */
  #define COLOR_BLUE          0x001f    /* RGB 0000F8 */
  #define COLOR_GREEN         0x07e0    /* RGB 00FC00 */
  #define COLOR_CYAN          0x07ff    /* RGB 00FCF8 */
  #define COLOR_RED           0xf800    /* RGB F80000 */
  #define COLOR_MAGENTA       0xf81f    /* RGB F800F8 */
  #define COLOR_YELLOW        0xffe0    /* RGB F8FC00 */
  #define COLOR_ORANGE        0xfd20    /* RGB F8A400 */
  #define COLOR_GREY          0xc618    /* RGB C0C0C0 */
  #define COLOR_WHITE         0xffff    /* RGB FFFFFF */
  #define COLOR_BROWN         0x6920    /* RGB 682400 */
  #define COLOR_VIOLET        0x780f    /* RGB 780078 */
  #define COLOR_PALE_YELLOW   0xe70c    /* RGB E2E164 */
  #define COLOR_PALE_GREEN    0x9fec    /* RGB 99FF66 */
  #define COLOR_PALE_RED      0xfb6d    /* RGB FF6F6F */
  #define COLOR_STEEL_BLUE    0x64df    /* RGB 6098F8 */
  #define COLOR_SKY_BLUE      0x06ff    /* RGB 06DCF9 */

  /* component color codes */
  #define COLOR_CODE_BLACK    0x0000    /* RGB 000000 */
  #define COLOR_CODE_BROWN    0xcb25    /* RGB CE652C */
  #define COLOR_CODE_RED      0xf800    /* RGB F80000 */
  #define COLOR_CODE_ORANGE   0xfe40    /* RGB FFCB00 */
  #define COLOR_CODE_YELLOW   0xffe0    /* RGB F8FC00 */
  #define COLOR_CODE_GREEN    0x07e0    /* RGB 00FC00 */
  #define COLOR_CODE_BLUE     0x001f    /* RGB 0000F8 */
  #define COLOR_CODE_VIOLET   0xca19    /* RGB CF40CF */
  #define COLOR_CODE_GREY     0xf79e    /* RGB F0F0F0 */
  #define COLOR_CODE_WHITE    0xffff    /* RGB FFFFFF */
  #define COLOR_CODE_GOLD     0xf606    /* RGB F6C337 */
  #define COLOR_CODE_SILVER   0xe71c    /* RGB E0E0E0 */
  /* component's body color */
  #define COLOR_CODE_NONE     0x06ff    /* RGB 06DCF9 */
#endif


/*
 *  ST7735
 *  - RGB565
 */

#ifdef LCD_ST7735
  /* common colors */
  #define COLOR_BLACK         0x0000    /* RGB 000000 */
  #define COLOR_BLUE          0x001f    /* RGB 0000F8 */
  #define COLOR_GREEN         0x07e0    /* RGB 00FC00 */
  #define COLOR_CYAN          0x07ff    /* RGB 00FCF8 */
  #define COLOR_RED           0xf800    /* RGB F80000 */
  #define COLOR_MAGENTA       0xf81f    /* RGB F800F8 */
  #define COLOR_YELLOW        0xffe0    /* RGB F8FC00 */
  #define COLOR_ORANGE        0xfd20    /* RGB F8A400 */
  #define COLOR_GREY          0xc618    /* RGB C0C0C0 */
  #define COLOR_WHITE         0xffff    /* RGB FFFFFF */
  #define COLOR_BROWN         0x6920    /* RGB 682400 */
  #define COLOR_VIOLET        0x780f    /* RGB 780078 */
  #define COLOR_PALE_YELLOW   0xe70c    /* RGB E2E164 */
  #define COLOR_PALE_GREEN    0x9fec    /* RGB 99FF66 */
  #define COLOR_PALE_RED      0xfb6d    /* RGB FF6F6F */
  #define COLOR_STEEL_BLUE    0x64df    /* RGB 6098F8 */
  #define COLOR_SKY_BLUE      0x06ff    /* RGB 06DCF9 */

  /* component color codes */
  #define COLOR_CODE_BLACK    0x0000    /* RGB 000000 */
  #define COLOR_CODE_BROWN    0x6920    /* RGB 682400 */
  #define COLOR_CODE_RED      0xf800    /* RGB F80000 */
  #define COLOR_CODE_ORANGE   0xfc80    /* RGB F89300 */
  #define COLOR_CODE_YELLOW   0xffe0    /* RGB F8FC00 */
  #define COLOR_CODE_GREEN    0x05c0    /* RGB 00BB00 */
  #define COLOR_CODE_BLUE     0x001f    /* RGB 0000F8 */
  #define COLOR_CODE_VIOLET   0x780f    /* RGB 780078 */
  #define COLOR_CODE_GREY     0x9cd3    /* RGB 9A9A9A */
  #define COLOR_CODE_WHITE    0xffff    /* RGB FFFFFF */
  #define COLOR_CODE_GOLD     0xf606    /* RGB F6C337 */
  #define COLOR_CODE_SILVER   0xe71c    /* RGB E0E0E0 */
  /* component's body color */
  #define COLOR_CODE_NONE     0x06ff    /* RGB 06DCF9 */
#endif


/*
 *  Semi ST7735
 *  - BGR565 (red and blue reversed)
 */

#ifdef LCD_SEMI_ST7735

  /* common colors (in BGR format) */
  #define COLOR_BLACK         0x0000    /* RGB 000000 */
  #define COLOR_BLUE          0xf800    /* RGB 0000F8 */
  #define COLOR_GREEN         0x07e0    /* RGB 00FC00 */
  #define COLOR_CYAN          0xffe0    /* RGB 00FCF8 */
  #define COLOR_RED           0x001f    /* RGB F80000 */
  #define COLOR_MAGENTA       0xf81f    /* RGB F800F8 */
  #define COLOR_YELLOW        0x07ff    /* RGB F8FC00 */
  #define COLOR_ORANGE        0x053f    /* RGB F8A400 */
  #define COLOR_GREY          0xc618    /* RGB C0C0C0 */
  #define COLOR_WHITE         0xffff    /* RGB FFFFFF */
  #define COLOR_BROWN         0x012d    /* RGB 682400 */
  #define COLOR_VIOLET        0x780f    /* RGB 780078 */
  #define COLOR_PALE_YELLOW   0x671c    /* RGB E2E164 */
  #define COLOR_PALE_GREEN    0x9fec    /* RGB 99FF66 */
  #define COLOR_PALE_RED      0xfb6d    /* RGB FF6F6F */
  #define COLOR_STEEL_BLUE    0xfccc    /* RGB 6098F8 */
  #define COLOR_SKY_BLUE      0xfee0    /* RGB 06DCF9 */

  /* component color codes (in BGR format) */
  #define COLOR_CODE_BLACK    0x0000    /* RGB 000000 */
  #define COLOR_CODE_BROWN    0x012d    /* RGB 682400 */
  #define COLOR_CODE_RED      0x001f    /* RGB F80000 */
  #define COLOR_CODE_ORANGE   0x049f    /* RGB F89300 */
  #define COLOR_CODE_YELLOW   0x07ff    /* RGB F8FC00 */
  #define COLOR_CODE_GREEN    0x05c0    /* RGB 00BB00 */
  #define COLOR_CODE_BLUE     0xf800    /* RGB 0000F8 */
  #define COLOR_CODE_VIOLET   0x780f    /* RGB 780078 */
  #define COLOR_CODE_GREY     0x9cd3    /* RGB 9A9A9A */
  #define COLOR_CODE_WHITE    0xffff    /* RGB FFFFFF */
  #define COLOR_CODE_GOLD     0x361e    /* RGB F6C337 */
  #define COLOR_CODE_SILVER   0xe71c    /* RGB E0E0E0 */
  /* component's body color (in BGR format) */
  #define COLOR_CODE_NONE     0xfee0    /* RGB 06DCF9 */

#endif


/*
 *  VT100 serial terminal
 *  - 8 fixed foreground and background colors
 *  - foreground colors: 30-37
 *  - background colors: 40-47
 *  - firmware adds automatically 10 for background colors
 */

#ifdef LCD_VT100
  /* foreground */
  #define COLOR_BLACK         30
  #define COLOR_RED           31
  #define COLOR_GREEN         32
  #define COLOR_YELLOW        33
  #define COLOR_BLUE          34
  #define COLOR_MAGENTA       35
  #define COLOR_CYAN          36
  #define COLOR_WHITE         37
#endif


/*
 *  R&D Display
 *  - RGB565
 */

#ifdef LCD_RD_DISPLAY
  /* common colors */
  #define COLOR_BLACK         0x0000    /* RGB 000000 */
  #define COLOR_BLUE          0x001f    /* RGB 0000F8 */
  #define COLOR_GREEN         0x07e0    /* RGB 00FC00 */
  #define COLOR_CYAN          0x07ff    /* RGB 00FCF8 */
  #define COLOR_RED           0xf800    /* RGB F80000 */
  #define COLOR_MAGENTA       0xf81f    /* RGB F800F8 */
  #define COLOR_YELLOW        0xffe0    /* RGB F8FC00 */
  #define COLOR_ORANGE        0xfd20    /* RGB F8A400 */
  #define COLOR_GREY          0xc618    /* RGB C0C0C0 */
  #define COLOR_WHITE         0xffff    /* RGB FFFFFF */
  #define COLOR_BROWN         0x6920    /* RGB 682400 */
  #define COLOR_VIOLET        0x780f    /* RGB 780078 */
  #define COLOR_PALE_YELLOW   0xe70c    /* RGB E2E164 */
  #define COLOR_PALE_GREEN    0x9fec    /* RGB 99FF66 */
  #define COLOR_PALE_RED      0xfb6d    /* RGB FF6F6F */
  #define COLOR_STEEL_BLUE    0x64df    /* RGB 6098F8 */
  #define COLOR_SKY_BLUE      0x06ff    /* RGB 06DCF9 */

  /* component color codes */
  #define COLOR_CODE_BLACK    0x0000    /* RGB 000000 */
  #define COLOR_CODE_BROWN    0x6920    /* RGB 682400 */
  #define COLOR_CODE_RED      0xf800    /* RGB F80000 */
  #define COLOR_CODE_ORANGE   0xfc80    /* RGB F89300 */
  #define COLOR_CODE_YELLOW   0xffe0    /* RGB F8FC00 */
  #define COLOR_CODE_GREEN    0x05c0    /* RGB 00BB00 */
  #define COLOR_CODE_BLUE     0x001f    /* RGB 0000F8 */
  #define COLOR_CODE_VIOLET   0x780f    /* RGB 780078 */
  #define COLOR_CODE_GREY     0x9cd3    /* RGB 9A9A9A */
  #define COLOR_CODE_WHITE    0xffff    /* RGB FFFFFF */
  #define COLOR_CODE_GOLD     0xf606    /* RGB F6C337 */
  #define COLOR_CODE_SILVER   0xe71c    /* RGB E0E0E0 */
  /* component's body color */
  #define COLOR_CODE_NONE     0x06ff    /* RGB 06DCF9 */
#endif



/* ************************************************************************
 *   default colors
 * ************************************************************************ */


/* background color */
#define COLOR_BACKGROUND      COLOR_BLACK

/* standard pen color */
#define COLOR_PEN             COLOR_MATRIX_GREEN_ALT

/* titles */
#define COLOR_TITLE           COLOR_PALE_GREEN

/* values */
#define COLOR_VALUE           COLOR_MATRIX_GREEN

/* cursor */
#define COLOR_CURSOR          COLOR_PALE_GREEN

/* infos (hello/bye) */
#define COLOR_INFO            COLOR_CYAN

/* warnings (not used yet) */
#define COLOR_WARN            COLOR_YELLOW

/* errors */
#define COLOR_ERROR           COLOR_RED

/* marker (selected item) */
#define COLOR_MARKER          COLOR_PALE_GREEN

/* symbols */
#define COLOR_SYMBOL          COLOR_PALE_GREEN

/* color codes for probe pins */
#define COLOR_PROBE_1         COLOR_SKY_BLUE
#define COLOR_PROBE_2         COLOR_PALE_YELLOW
#define COLOR_PROBE_3         COLOR_RED

/* battery status */
#define COLOR_BAT_OK          COLOR_PALE_GREEN
#define COLOR_BAT_WEAK        COLOR_YELLOW
#define COLOR_BAT_LOW         COLOR_RED



/* ************************************************************************
 *   EOF
 * ************************************************************************ */
