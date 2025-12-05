#ifndef ACTUATOR
#define ACTUATOR

#include "esp_err.h"
#include <stdbool.h>

// PDO_SELECT values for SRC_PDO register (bits 7:4)
#define HUSB238_SELECT_5V (0x01 << 4)
#define HUSB238_SELECT_9V (0x02 << 4)
#define HUSB238_SELECT_12V (0x03 << 4)
#define HUSB238_SELECT_15V (0x08 << 4)
#define HUSB238_SELECT_18V (0x09 << 4)
#define HUSB238_SELECT_20V (0x0A << 4)

void husb238_toggle_power(bool new_state);
esp_err_t husb238_i2c_init(void);
void husb238_get_status(void);
void husb238_check_available_voltages(void);
esp_err_t husb238_request_voltage(uint8_t voltage);

#endif // !ACTUATOR
