#include <Arduino.h>
#include <NimBLEDevice.h>

static const char* DEVICE_NAME = "ESP32-BLE-Poll";
static const char* SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0";
static const char* CHAR_UUID    = "12345678-1234-5678-1234-56789abcdef1";

const int LED_PIN = 2;

NimBLECharacteristic* ledChar = nullptr;

class WriteCallback : public NimBLECharacteristicCallbacks {
public:
  // NimBLE variant cause
  void onWrite(NimBLECharacteristic* c) {
    handleWrite(c);
  }
  // NimBLE (core 3.x) variant 2
  void onWrite(NimBLECharacteristic* c, NimBLEConnInfo&) {
    handleWrite(c);
  }

private:
  void handleWrite(NimBLECharacteristic* c) {
    std::string v = c->getValue();
    if (v.empty()) return;
    uint8_t b = static_cast<uint8_t>(v[0]);
    if (b == 0x01) {
      digitalWrite(LED_PIN, HIGH);
    } else if (b == 0x00) {
      digitalWrite(LED_PIN, LOW);
    } else {
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
    }
  }
};

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  NimBLEDevice::init(DEVICE_NAME);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); 

  NimBLEServer* server = NimBLEDevice::createServer();
  NimBLEService* svc = server->createService(SERVICE_UUID);

  ledChar = svc->createCharacteristic(
    CHAR_UUID,
    NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
  );
  ledChar->setCallbacks(new WriteCallback());

  svc->start();

  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->start();
}

void loop() {
}
