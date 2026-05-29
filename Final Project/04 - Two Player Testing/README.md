# 04 - Implementing a second accelerometer for two player gameplay.

## Experiment Description
Following the creation of the single player testing simulation, I can now expand on the solution by implementing a second accelerometer attached to a feather board. Both players feather boards will independently communicate to the central point feather board their individual accelerometer data. This experiment aims to complete the final project, creating a game of tennis involving two players using accelerometers to hit an LED tennis ball across an LED strip light court. Both players will be able to hit the ball within their side of the court only. The final experiments end result will be two players being able to play a game of tennis with each other.

## Components

### 3x Adafruit HUZZAH32 - ESP32 Feather

### 1x MPU6050 Accelerometer and Gyroscope Module

### 1x Adafruit BNO055 Absolute Orientation Sensor

### 1x Adafruit Neopixel LED Strip Light

## Walkthrough (Record of Troubleshooting and Success)
For the second players circuit, I was provided with a different model accelerometer, one that was supposedly more efficient and provided more in depth readings than the MPU6050 I used for the first player. So I had to learn how to implement this new device the same way I implemented the MPU6050. Learning how to get the readings and what data needed to be modified to provide me the acceleration results I was looking for.

This accelerometer was the Adafruit BNO055 Absolute Orientation Sensor. So I had to import a different library to utilise its readings.

```C
#include <Adafruit_BNO055.h>
```

An object is then declared for the device, giving the object an identifier value (55) and providing setup instructions to work on the I2C communication interface. Then, in the setup function the sensor is initialised and an error checker is ran to ensure the device is setup properly before use.

```C
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

void setup() {
    ...
    /* Initialise the sensor */
    if (!bno.begin())
    {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        while (1);
    }
    ...
}
```

Then, within the loop function the object can be communicated with, the bno object is then called from to get each individual piece of data from the sensor. These details are then printed to the Serial Monitor using the printEvent() function.

```C
void loop(void)
{
  sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

  printEvent(&orientationData);
  printEvent(&angVelocityData);
  printEvent(&linearAccelData);
  printEvent(&magnetometerData);
  printEvent(&accelerometerData);
  printEvent(&gravityData);
  ...
}
```

After analysing what data is output by this device, I found that the accelerometer data is not measured as a multiple of the gravity [1g = 9.81 m/s²] like the MPU6050, but measured as gravity on its own. [9.81 m/s²].

### Evidence: [See TWO-01.MOV]

Additionally, the issue with gravity that came from the MPU6050 was also present on the BNO055, but after reading more into the BNO055's data readings I discovered that the gravityData was exactly what I needed to tackle this issue. Compared to the MPU6050, removing the gravity factor from the acceleration data was as easy as subtracting the value. I also had to account for the fact that the measurements from both accelerometers were different, and so I then divided the calculation by 9.81 to match the MPU6050 readings.

These readings would align with the swing conditions previously defined. I updated the message struct with the new calculations.

```C
message.Xval = ((accelerometerData.acceleration.x)-(gravityData.acceleration.x))/9.81;
message.Yval = ((accelerometerData.acceleration.y)-(gravityData.acceleration.y))/9.81;
message.Zval = ((accelerometerData.acceleration.z)-(gravityData.acceleration.z))/9.81;
```

### Evidence: [See TWO-02.MOV]

Now that the correct calculations had been made, the 2nd accelerometer was ready to be implemented into the solution. To differentiate between the two players, a new variable had to be created and sent to the central point in order for the solution to know which data being read was which.

```C
typedef struct struct_message {
    // uint16_t val; // A 16-bit unsigned number
    uint16_t player;
    float Xval; // A 16-bit unsigned number
    float Yval; // A 16-bit unsigned number
    float Zval; // A 16-bit unsigned number
} message_t;
```

The simple solution was to add an integer value which represented the player number, and assigning each device with values 1 and 2.

```C
// MPU6050 Transmitter
message.player = 1;
message.Xval = mpu.getAccX()+zVal;
message.Yval = mpu.getAccY()-yVal;
message.Zval = mpu.getAccZ()-xVal;
```

```C
// BNO055 Transmitter
message.player = 2;
message.Xval = ((accelerometerData.acceleration.x)-(gravityData.acceleration.x))/9.81;
message.Yval = ((accelerometerData.acceleration.y)-(gravityData.acceleration.y))/9.81;
message.Zval = ((accelerometerData.acceleration.z)-(gravityData.acceleration.z))/9.81;
```

Now that these values had been assigned, I was able to differentiate between the two readings being sent to the receiver.

### Evidence: [See TWO-03.MOV]

With both readings being successfully transmitted, I now ran into the issue of the code needing updating. As my original solution only accounted for one player, the hitPixel system required changing.

Without the need to prevent the player from hitting multiple times at once, I was able to rely on the message.player variable from the feather board transmissions. I replaced the hitPixel solution with a new set of if statements which checks which player hit the ball AND whether the ball is in their side of the court.

In theory, this should prevent any unusual inputs. Players should only be able to hit the ball when its in their side of the court, sending it back the other way to the opposing player until the game ends.

```C
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
```

With the conditions updated, and the 2nd player accelerometer successfully sending the correct calculations to the central point alongside the 1st player accelerometer, I could now put the solution to testing.

### Evidence: [See FINAL.mp4]

After testing the solution with multiple peers, the project was a success and the experiment complete. Although, feedback gained suggested there was room for changes to the game, which in future with more time could be implemented.

## Circuit Diagrams

### Receiver Circuit Diagram
![Receiver](<circuit receiver.png>)

### Transmitter 1 Circuit Diagram
![Transmitter 1](<circuit transmitter-1.png>)

### Transmitter 2 Circuit Diagram
![Transmitter 2](<circuit transmitter-2.png>)

## Final Evaluation
The final concept for this project differed very slightly from its initial concept. I aimed to create an experience similar to Nintendo's Wii Sports Tennis, where players had the ability to immitate a game of tennis using their own movements as input. Using the accelerometers (MPU6050 and BNO055 by Adafruit) I was able to read data relating to the players movement and speed, and with the use of the HUZZAH32 ESP32 Feather boards I was able to create a wireless communication between two players and a central receiving device.

After testing the solution with my peers I believe the solution was a success, finalising the project and being able to play the game itself. The project slightly differed during the course of my experiments, most importantly with how I handled the court.

After experimenting with the Adafruit Neopixel LED Strip Light and understanding it's capabilities, I decided to scrap my original plan of using an LCD screen to display the tennis ball bouncing. After demonstrating how I could iterate through each individual pixel on the strip light it became clear to me that the LED strip light was a perfect alternative to using an LCD screen. Although, I did not give myself the chance to experiment with an LCD screen which in future I would aim to do before finalising my idea, to give myself a broader range of options and understanding of different components.

I had great difficulty implementing the MPU6050 Accelerometer compared to the BNO055 Accelerometer. The MPU6050 was a good asset for reading acceleration data utilising the devices sensors, during the first final project experiment I had a tough time trying to account for the gravity situation. The accelerometer data was being effected by gravity, the data would be inaccurate if I had not been introduced to a solution by one of my peers. The BNO055 in comparison accounts for this factor and has its own readable data for which direction gravity is influencing the acclerometer data. In future, I would hope to invest in a second BNO055 accelerometer, it provided more efficient solutions to more accurate data and would ensure my project was as efficient as possible.

A concept which was discussed amongst myself and my peers/testers was implementing difficulty factors into the game. Finding ways to make the game progressivly become harder to play such as increasing ball speed or hit force. Additionally, I was presented with the suggestion of expanding on the swing conditions to influence ball launch distance, depending on how hard the ball is hit is how far it would travel across the LED strip. In future, these concepts would make great additions and would further reach my target audience by providing a more competitive and enjoyable experience with replayability. If given more time to work on this project, I would seek to expand on existing features such as swing conditions and the LED strip tennis ball to create a more complex system.

Overall, while the project differed slightly from its initial concept I believe this project was a success. Utilising knowledge from the experiments carried out beforehand and discussion with peers I believe I have made a working solution which meets the initial requirements I set out to complete. Although in future, I would aim to invest in better equipment to ensure greater accuracy in data being read, and look to expand on already existing features to improve them and create a more competitive enjoyable experience. In future projects I aim to apply more research and experimentation to potential components which could be implemented into my solutions. 

## References
https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/overview
https://components101.com/sensors/mpu6050-module