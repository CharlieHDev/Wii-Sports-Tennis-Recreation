#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Valid channels are 1-14, must match receiver.
#define CHANNEL 6

esp_now_peer_info_t receiver = {
  .peer_addr = {0x8C, 0x4B, 0x14, 0x0B, 0x70, 0x38},
  .channel = CHANNEL,
  .encrypt = false
};

// This is the message we transmit; change this structure
// to send something different.
typedef struct struct_message {
    uint16_t val; // A 16-bit unsigned number
} message_t;

message_t message;

void setup() {
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(CHANNEL);

  while (!WiFi.STA.started()) {
    delay(100);
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Failed to initialise ESP-NOW");
    return;
  }
     
  if (esp_now_add_peer(&receiver) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Transmission ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "succeeded" : "failed");
}

void loop() {

  int sensorValue = analogRead(A2);
  int brightness = map(sensorValue, 0, 4095, 0, 255);

  message.val = brightness;

  esp_err_t result = esp_now_send(receiver.peer_addr, (uint8_t *)&message, sizeof(message));
 
  Serial.print(", status: ");
  Serial.println(result == ESP_OK ? "OK" : "ERROR");

  delay(100);
}