/* Get all possible data from MPU6050
 * Accelerometer values are given as multiple of the gravity [1g = 9.81 m/s²]
 * Gyro values are given in deg/s
 * Angles are given in degrees
 * Note that X and Y are tilt angles and not pitch/roll.
 *
 * License: MIT
 */

 #include "Wire.h"
 #include <MPU6050_light.h>
 #include <esp_now.h>
 #include <WiFi.h>
 #include <esp_wifi.h>
 
 // Valid channels are 1-14, must match receiver.
 #define CHANNEL 2
 
 esp_now_peer_info_t receiver = {
   .peer_addr = {0x8C, 0x4B, 0x14, 0x11, 0x82, 0x40},
   .channel = CHANNEL,
   .encrypt = false
 };
 
 MPU6050 mpu(Wire);
 
 long timer = 0;
 
 typedef struct struct_message {
     // uint16_t val; // A 16-bit unsigned number
     float Xval; // A 16-bit unsigned number
     float Yval; // A 16-bit unsigned number
     float Zval; // A 16-bit unsigned number
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
 
   Wire.begin();
   
   byte status = mpu.begin();
   Serial.print(F("MPU6050 status: "));
   Serial.println(status);
   while(status!=0){ } // stop everything if could not connect to MPU6050
   
   Serial.println(F("Calculating offsets, do not move MPU6050"));
   delay(1000);
   mpu.calcOffsets(true,true); // gyro and accelero
   Serial.println("Done!\n");
   
 }
 
 void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
   Serial.print("Transmission ");
   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "succeeded" : "failed");
 }
 
void loop() {
   mpu.update();
 
   if(millis() - timer > 1000){ // print data every second
     Serial.print(F("TEMPERATURE: "));Serial.println(mpu.getTemp());
 
     Serial.print(F("ACCELERO  X: "));Serial.print(mpu.getAccX());
     Serial.print("\tY: ");Serial.print(mpu.getAccY());
     Serial.print("\tZ: ");Serial.println(mpu.getAccZ());
   
     Serial.print(F("GYRO      X: "));Serial.print(mpu.getGyroX());
     Serial.print("\tY: ");Serial.print(mpu.getGyroY());
     Serial.print("\tZ: ");Serial.println(mpu.getGyroZ());
   
     Serial.print(F("ACC ANGLE X: "));Serial.print(mpu.getAccAngleX());
     Serial.print("\tY: ");Serial.println(mpu.getAccAngleY());
     
     Serial.print(F("ANGLE     X: "));Serial.print(mpu.getAngleX());
     Serial.print("\tY: ");Serial.print(mpu.getAngleY());
     Serial.print("\tZ: ");Serial.println(mpu.getAngleZ());
 
       // Serial.print(F("NEW ACCELO  X: "));Serial.print(mpu.getAccX()-SinX);
       // Serial.print("\tY: ");Serial.print(mpu.getAccY()-SinY);
       // Serial.print("\tZ: ");Serial.println(mpu.getAccZ()-Zvalue);
       Serial.println(F("=====================================================\n"));
     timer = millis();
   }
 
 
     float SinX = mpu.getAngleX()*(M_PI/180);
     float SinY = mpu.getAngleY()*(M_PI/180);
     float xVal = cos(SinX)*cos(SinY);
     float yVal = sin(SinX)*cos(SinY);
     float zVal = sin(SinY);
     // Serial.print("\t");Serial.print(xVal);
     // Serial.print("\t");Serial.print(yVal);
     // Serial.print("\t");Serial.print(zVal);
 
     message.Xval = mpu.getAccX()+zVal;
     message.Yval = mpu.getAccY()-yVal;
     message.Zval = mpu.getAccZ()-xVal;
 
   esp_err_t result = esp_now_send(receiver.peer_addr, (uint8_t *)&message, sizeof(message));
  
   // Serial.print(", status: ");
   // Serial.println(result == ESP_OK ? "OK" : "ERROR");
 
   // Serial.print(message.Xval);
 
   delay(100);
 
 }
 