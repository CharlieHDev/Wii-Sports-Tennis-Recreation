#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Valid channels are 1-14, must match transmitter
#define CHANNEL 6

// This message structure must match the structure sent by the transmitter
typedef struct struct_message {
    uint16_t val; // A 16-bit unsigned number
} message_t;

message_t message;

void setup() {
  Serial.begin(115200);
 
  // Initialize the WiFi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(CHANNEL);

  while (!WiFi.STA.started()) {
    delay(100);
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Failed to initialise ESP-NOW");
    return;
  }
 
  Serial.print("Receiver initialised with MAC address ");
  Serial.println(WiFi.macAddress());

  esp_now_register_recv_cb(onDataReceived);
}

void onDataReceived(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int length) {
  memcpy(&message, data, sizeof(message));

  analogWrite(27, message.val);

  Serial.print("Received data: ");
  Serial.println(message.val);
}

void loop() {
  // Nothing to do.
}
