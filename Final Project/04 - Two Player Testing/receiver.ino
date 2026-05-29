#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Valid channels are 1-14, must match transmitter
#define CHANNEL 2

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        12 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 143 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRBW + NEO_KHZ800);

// This message structure must match the structure sent by the transmitter
typedef struct struct_message {
    uint16_t player; // Player number.  
    float Xval; // A 16-bit unsigned number
    float Yval; // A 16-bit unsigned number
    float Zval; // A 16-bit unsigned number
} message_t;

message_t message;

long timer = 0;

int pixel = 1;
int playerHit = 1;

void setup() {
  Serial.begin(115200);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(50);
 
  // Initialize the WiFi module
  WiFi.mode(WIFI_STA);
  WiFi.setChannel(CHANNEL);

  while (!WiFi.STA.started()) {
    Serial.println("We in here");
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

  Serial.print(message.player);Serial.print(": ");
  Serial.print("\t");Serial.print(message.Xval);
  Serial.print("\t");Serial.print(message.Yval);
  Serial.print("\t");Serial.print(message.Zval);Serial.print("\n");

  if((message.Xval > 1.5 || message.Xval < -1.5
  || message.Yval > 1.5 || message.Yval < -1.5
  || message.Zval > 1.5 || message.Zval < -1.5)
  && (pixel < 47 || pixel > 96)){
    if(message.player == 1 && pixel < 47){
      playerHit = 1;
    }
    if (message.player == 2 && pixel > 96) {
      playerHit = 2;
    }
  }
}

void loop() {
    while(pixel >= 0 && pixel <= 143){
      if(playerHit == 1){
        pixel++;
      } else {
        pixel--;
      }
      if(pixel == 0){
        Serial.print("Player 2 wins.");
        break;
      }
      if(pixel == 143) {
        Serial.print("Player 1 wins.");
        break;
      }
      pixels.clear(); // Set all pixel colors to 'off'

      // Ball hit boundary markers.
      pixels.setPixelColor(47, pixels.Color(255,255,255));
      pixels.setPixelColor(96, pixels.Color(255,255,255));
      pixels.setPixelColor(pixel, pixels.Color(0,255,0));
      pixels.show();
      delay(20);
  }
}
