#include "actuator.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "hal/i2c_types.h"
#include "soc/clk_tree_defs.h"
#include <stdbool.h>
#include <stdint.h>

// The pin to kill power
// #define POWER_PIN GPIO_NUM_11 // Single Purple

#define I2C_SDA 8 // Gray
#define I2C_SCL 9 // Purple
#define I2C_MASTER_FREQ_HZ 100 * 1000
#define HUSB238_ADDR 0x08 // HUSB238 I2C address

// HUSB238 Register addresses
#define HUSB238_PD_STATUS0 0x00
#define HUSB238_PD_STATUS1 0x01
#define HUSB238_SRC_PDO_5V 0x02
#define HUSB238_SRC_PDO_9V 0x03
#define HUSB238_SRC_PDO_12V 0x04
#define HUSB238_SRC_PDO_15V 0x05
#define HUSB238_SRC_PDO_18V 0x06
#define HUSB238_SRC_PDO_20V 0x07
#define HUSB238_SRC_PDO 0x08
#define HUSB238_GO_COMMAND 0x09

// GO_COMMAND values (bits 4:0)
#define HUSB238_REQUEST_PDO 0x01
#define HUSB238_GET_SRC_CAP 0x04
#define HUSB238_HARD_RESET 0x10

static const char *TAG = "HUSB238";
static i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t dev_handle;

void husb238_toggle_power(bool new_state) {
    ESP_LOGI(TAG, "Recived toogle power state %d", new_state);
    return;
    /* if (new_state)
        gpio_set_level(POWER_PIN, 0);
    else
        gpio_set_level(POWER_PIN, 1); */
}

esp_err_t husb238_i2c_init(void) {
    i2c_master_bus_config_t conf = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = I2C_SDA,
        .scl_io_num = I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    esp_err_t err = i2c_new_master_bus(&conf, &bus_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C bus init failed: %s", esp_err_to_name(err));
        return err;
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = HUSB238_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };

    err = i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle);
    if (err != ESP_OK)
        ESP_LOGE(TAG, "I2C device add failed: %s", esp_err_to_name(err));

    /* gpio_set_direction(POWER_PIN, GPIO_MODE_OUTPUT);
    husb238_toggle_power(false); */

    return err;
}

// Write single byte to register
esp_err_t husb238_write_reg(uint8_t reg, uint8_t data) {
    uint8_t write_buf[2] = {reg, data};
    return i2c_master_transmit(dev_handle, write_buf, sizeof(write_buf), 1000);
}

// Read single byte from register
esp_err_t husb238_read_reg(uint8_t reg, uint8_t *data) {
    return i2c_master_transmit_receive(dev_handle, &reg, 1, data, 1, 1000);
}

// Get current voltage and current
void husb238_get_status(void) {
    uint8_t status0, status1;

    if (husb238_read_reg(HUSB238_PD_STATUS0, &status0) == ESP_OK &&
        husb238_read_reg(HUSB238_PD_STATUS1, &status1) == ESP_OK) {

        // Extract voltage from status0 bits [7:4]
        uint8_t voltage_code = (status0 >> 4) & 0x0F;
        const char *voltage_str = "Unattached";

        // clang-format off
        switch(voltage_code) {
            case 0x00: voltage_str = "Unattached"; break;
            case 0x01: voltage_str = "5V"; break;
            case 0x02: voltage_str = "9V"; break;
            case 0x03: voltage_str = "12V"; break;
            case 0x04: voltage_str = "15V"; break;
            case 0x05: voltage_str = "18V"; break;
            case 0x06: voltage_str = "20V"; break;
            default: voltage_str = "Reserved"; break;
        }
        // clang-format on

        // Extract current from status0 bits [3:0]
        uint8_t current_code = status0 & 0x0F;
        float current_amps = 0.0;

        // clang-format off
        switch(current_code) {
            case 0x00: current_amps = 0.5; break;
            case 0x01: current_amps = 0.7; break;
            case 0x02: current_amps = 1.0; break;
            case 0x03: current_amps = 1.25; break;
            case 0x04: current_amps = 1.5; break;
            case 0x05: current_amps = 1.75; break;
            case 0x06: current_amps = 2.0; break;
            case 0x07: current_amps = 2.25; break;
            case 0x08: current_amps = 2.5; break;
            case 0x09: current_amps = 2.75; break;
            case 0x0A: current_amps = 3.0; break;
            case 0x0B: current_amps = 3.25; break;
            case 0x0C: current_amps = 3.5; break;
            case 0x0D: current_amps = 4.0; break;
            case 0x0E: current_amps = 4.5; break;
            case 0x0F: current_amps = 5.0; break;
        }
        // clang-format on

        // Check attachment and CC direction
        bool attached = (status1 >> 6) & 0x01;
        bool cc_dir = (status1 >> 7) & 0x01;

        // Check PD response bits [5:3]
        uint8_t pd_response = (status1 >> 3) & 0x07;
        const char *response_str = "Unknown";

        switch (pd_response) {
        case 0x00:
            response_str = "No response";
            break;
        case 0x01:
            response_str = "Success";
            break;
        case 0x03:
            response_str = "Invalid command";
            break;
        case 0x04:
            response_str = "Not supported";
            break;
        case 0x05:
            response_str = "Transaction fail";
            break;
        }

        ESP_LOGI(TAG,
                 "Status: %s @ %.2fA | Attached: %s | CC: %s | Response: %s",
                 voltage_str, current_amps, attached ? "Yes" : "No",
                 cc_dir ? "CC2" : "CC1", response_str);
    }
}

// Check which voltages are available from the source
void husb238_check_available_voltages(void) {
    uint8_t pdo;
    ESP_LOGI(TAG, "Available voltages from power source:");

    if (husb238_read_reg(HUSB238_SRC_PDO_5V, &pdo) == ESP_OK && (pdo & 0x80)) {
        float current = ((pdo & 0x0F) == 0) ? 0.5 : (0.5 + (pdo & 0x0F) * 0.25);
        ESP_LOGI(TAG, "  5V @ %.2fA", current);
    }
    if (husb238_read_reg(HUSB238_SRC_PDO_9V, &pdo) == ESP_OK && (pdo & 0x80)) {
        float current = ((pdo & 0x0F) == 0) ? 0.5 : (0.5 + (pdo & 0x0F) * 0.25);
        ESP_LOGI(TAG, "  9V @ %.2fA", current);
    }
    if (husb238_read_reg(HUSB238_SRC_PDO_12V, &pdo) == ESP_OK && (pdo & 0x80)) {
        float current = ((pdo & 0x0F) == 0) ? 0.5 : (0.5 + (pdo & 0x0F) * 0.25);
        ESP_LOGI(TAG, "  12V @ %.2fA", current);
    }
    if (husb238_read_reg(HUSB238_SRC_PDO_15V, &pdo) == ESP_OK && (pdo & 0x80)) {
        float current = ((pdo & 0x0F) == 0) ? 0.5 : (0.5 + (pdo & 0x0F) * 0.25);
        ESP_LOGI(TAG, "  15V @ %.2fA", current);
    }
    if (husb238_read_reg(HUSB238_SRC_PDO_18V, &pdo) == ESP_OK && (pdo & 0x80)) {
        float current = ((pdo & 0x0F) == 0) ? 0.5 : (0.5 + (pdo & 0x0F) * 0.25);
        ESP_LOGI(TAG, "  18V @ %.2fA", current);
    }
    if (husb238_read_reg(HUSB238_SRC_PDO_20V, &pdo) == ESP_OK && (pdo & 0x80)) {
        float current = ((pdo & 0x0F) == 0) ? 0.5 : (0.5 + (pdo & 0x0F) * 0.25);
        ESP_LOGI(TAG, "  20V @ %.2fA", current);
    }
}

// Request specific voltage (use HUSB238_SELECT_5V, HUSB238_SELECT_9V, etc.)
esp_err_t husb238_request_voltage(uint8_t voltage_select) {
    const char *voltage_str = "Unknown";

    // clang-format off
    switch(voltage_select) {
        case HUSB238_SELECT_5V: voltage_str = "5V"; break;
        case HUSB238_SELECT_9V: voltage_str = "9V"; break;
        case HUSB238_SELECT_12V: voltage_str = "12V"; break;
        case HUSB238_SELECT_15V: voltage_str = "15V"; break;
        case HUSB238_SELECT_18V: voltage_str = "18V"; break;
        case HUSB238_SELECT_20V: voltage_str = "20V"; break;
    }
    // clang-format on

    ESP_LOGI(TAG, "Requesting %s...", voltage_str);

    // Step 1: Write PDO selection to SRC_PDO register (0x08)
    esp_err_t err = husb238_write_reg(HUSB238_SRC_PDO, voltage_select);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set PDO selection");
        return err;
    }

    vTaskDelay(pdMS_TO_TICKS(10));

    // Step 2: Send request command to GO_COMMAND register (0x09)
    err = husb238_write_reg(HUSB238_GO_COMMAND, HUSB238_REQUEST_PDO);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to send GO command");
        return err;
    }

    // Wait for voltage transition (typically takes 200-500ms)
    vTaskDelay(pdMS_TO_TICKS(500));
    ESP_LOGI(TAG, "Voltage request completed");

    return ESP_OK;
}

#undef I2C_SCL
#undef I2C_SDA
#undef I2C_MASTER_NUM
