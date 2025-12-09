#ifndef COMMON
#define COMMON

// Intended for display_handler.c
#define DISP_PIN_NUM_CLK 36  // SCK
#define DISP_PIN_NUM_MOSI 35 // SDA
#define DISP_PIN_NUM_RST 47  // RES
#define DISP_PIN_NUM_DC 20   // DC

// Intended for keypad.c
// Naming convention for pins, Nx, where x is the pin number from left to right
#define KEY_PIN_N1_Y2 GPIO_NUM_4
#define KEY_PIN_N3_X2 GPIO_NUM_5
#define KEY_PIN_N4_Y3 GPIO_NUM_6
#define KEY_PIN_N5_Y4 GPIO_NUM_7
#define KEY_PIN_N6_X3 GPIO_NUM_12
#define KEY_PIN_N7_X4 GPIO_NUM_13
#define KEY_PIN_N8_Y1 GPIO_NUM_14

// Control Actuator power
#define KEY_PIN_POWER GPIO_NUM_11  // Blue
#define KEY_PIN_GROUND GPIO_NUM_10 // Torquise

#endif // !COMMON
