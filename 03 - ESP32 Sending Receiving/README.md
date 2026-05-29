# 03 - Sending/Receiving Data using Feather ESP32

## Experiment Description
This experiment utilises the Adafruit HUZZAH32's ESP32 capabilities for sending/receiving data between two devices. The main goal of this experiment is to succeed in sending different types of data from one Feather board to another using its ESP-NOW wireless protocol.

## Components
### 2x Adafruit HUZZAH32 - ESP32 Feather
The HUZZAH32 feather board can send wireless signals in many ways such as over Wi-Fi connection, using ESP-NOW wireless protocol to send packets without a router, and bluetooth. For the purpose of this experiment I have decided to use ESP-NOW as it does not require a Wi-Fi connection and the packet sizes are smaller.

Through the use of MAC addresses, the devices can communicate directly without a router and without needing to communicate with every nearby device. It works on the data-link layer of the OSI model which reduces the amount of layers data needs to be transmitted through.

## Walkthrough (Record of Troubleshooting and Success)
The first step in setting this experiment up was gathering the correct libraries from Adafruit, the ESP32 library provided by Adafruit contains pieces of example code which was used as a base template for setting up the two devices. A transmitting code template and a receiving code template is used to setup a Feather board which sends the data, and a Feather board which listens out for and receives the data.

After uploading the code templates to both devices I encountered issues straight away which I couldn't understand.

```C
typedef struct struct_message {
    String val; // String value to be sent.
} message_t;
```

This code snippet initialises a struct which  acts as the message to be sent.

```C
message.val = "Hello!";

  esp_err_t result = esp_now_send(receiver.peer_addr, (uint8_t *)&message, sizeof(message));
 
  Serial.print(", status: ");
  Serial.println(result == ESP_OK ? "OK" : "ERROR");
```

Following this, the value to be sent is updated, and then transmitted to the receiver 

The transmitting device outputted transmission failure messages to the Serial Monitor, while the data was being sent successfully the transmission failed each attempt.

### Evidence: [See ESP32-01.jpg]

Upon further inspection, I found that I had not changed the target MAC address specified at the top of the file. So while the data was being sent successfully it was not finding the correct device.

```C
esp_now_peer_info_t receiver = {
  .peer_addr = {0x8C, 0x4B, 0x14, 0x0B, 0x70, 0x38},
  .channel = CHANNEL,
  .encrypt = false
};
```

The code in this snippet represents the peer information parameters of the receiver, with its peer address (MAC Address) and what channel to look for it on. After changing this value however, no luck was found and the transmission was still failing.

It was suggested that changing the channel which the data was being transmitted on may solve the issue. And so after changing this value and uploading both pieces of code again the data was successfully being transmitted and displayed in the receiving devices Serial Monitor.

### Evidence: [See ESP32-02.MOV]

The transmitting devices Serial Monitor was outputting a transmission success message, while the receiving devices Serial Monitor was outputting the message being sent after every delay specified in the transmission code.

## Evaluation
The ESP32 Feather board is capable of sending data wirelessly, and with its ESP-NOW protocol I was able to transmit data from one device to another with very low latency. Setting the devices up using the template code provided by Adafruit was easier said than done, as learning how to use the program was essential in understanding how to successfully send data from one device to another.

I found difficulty in establishing the connection between the two devices, and after failed attempts at successfully transmitting data I learnt the key details required to establish this connection: Selecting the right channel to communicate across, as well as defining the correct MAC address to look for when transmitting the data.

Given ESP-NOW's limit of 250 bytes transmission, it isn't very scalable. If I decided to communicate larger amounts of data this may not be the best solution to work with, research into Wi-Fi or bluetooth communication protocols may improve this and expand the potential of this device but with the sacrifice of latency.

The use of wireless communications through these boards will benefit my final project as it will be used to send data wirelessly from the "controllers" to the central connection point. Instead of requiring a wireless connection which could cause issues with fast motions which may cause damage to the devices, I can remove the need for wires and send movement data wirelessly.

## References
https://www.espressif.com/en/solutions/low-power-solutions/esp-now
https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts