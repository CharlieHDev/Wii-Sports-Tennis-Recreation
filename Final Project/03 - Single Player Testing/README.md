# 03 - Configuring "player hit" mechanic.

## Experiment Description
With the LED strip light functioning as well as appropriate data being sent from the accelerometer, the next step to reaching the final goal and the purpose of this experiment is to detect when a player has swung their controller. Detecting the players movement reaching a certain speed and registering that as a "swing" to which the ball should change direction.

This experiment does not rely on there being two players, it is simply to ensure that the data being read from an accelerometer is being converted into a "swing", with the end result being a game where one player can hit the ball back and fourth with themselves. This can be used in future to test the boundaries of the swing more easily.

## Components

### 2x Adafruit HUZZAH32 - ESP32 Feather

### 1x MPU6050 Accelerometer and Gyroscope Module

### 1x Adafruit Neopixel LED Strip Light

## Walkthrough (Record of Troubleshooting and Success)
The majority of this experiment involved trial and error, figuring out the most optimal way of not only how often to read the data but additionally what boundaries to set as a minimum to be considered a "swing". If a swing is too light users could find the experience too easy or even make unintentional inputs. If the swing is too heavy the solution may be considered too difficult and users may not always make inputs that they desire.

Based on the data being read from the accelerometer, the data was measured as a multiple of the gravity [1g = 9.81 m/s²], with 0 representing no movement, 1.0 representing one multiple of gravity moving in one direction, and -1.0 representing one multiple of gravity moving in the opposite direction.

I had to create a minimum value which would be considered a "swing", which considered both directions of movement across all three axis'. This if statement was created which would run if the player moved the accelerometer in any direction with more than 1g (bigger than 1g or smaller than -1g). OR gates were used to ensure the code was executed regardless of what direction the device was swung.

```C
if(message.Xval > 1.0 || message.Xval < -1.0
|| message.Yval > 1.0 || message.Yval < -1.0
|| message.Zval > 1.0 || message.Zval < -1.0)
```

Now that the minimum values had been set for the "swing" to be registered, I then had to change the direction the tennis ball was moving. Utilising the playerHit variable I had created beforehand, I was able to determine which direction the tennis ball was moving. By changing this value when the device was "swung" I am able to reverse it upon detection of a swing using a set of if statements.

This if else statement simply switches between which player hit the ball last. 

```C
if(playerHit == 1){
    playerHit = 2;
} else {
    playerHit = 1;
}
```

Changing this value will result in the LED light strip to reverse its movement.

```C
void loop() {
    while(pixel >= 0 && pixel <= 143){
      if(playerHit == 1){
        pixel++;
      } else {
        pixel--;
      }
      ...
```

Now that I've setup my parameters for the swing movement, I am able to demonstrate the code physically by slowing down the ball to ensure I have enough time to perform the tests. After uploading my transmitter and receiver solutions I was able to test whether the if statements were functioning correctly.

After attempting the experiment, I was able to change the direction in which the ball was moving by slightly swinging the device.

### Evidence: [See SWING-01.MOV]

My swing system had a flaw, I had not accounted for the rate at which the data was being read. The data was being sent from the accelerometer across the feather boards every 100ms. Meaning the swing detection was 10 times a second, which caused some swings to be detected multiple times at once. As evidenced in the video I managed to swing the device just long enough for the ball to change directions twice sending it back the same way it started.

Although in future I would have safeguards in place to prevent this, for now to test the swinging boundaries I needed to ensure that the ball could not be hit twice at the same time.

I have now added a hitPixel variable which will change whenever the player swings.

```C
int pixel = 1;
int hitPixel = 1;
int playerHit = 1;
```

When the player swings their device, the hitPixel is set to the pixel where the player hit the ball. So if a player swings their device when the ball is at pixel 90, hitPixel will change to 90.

I then changed the swing condition to include an OR gate, ensuring that the pixel of which the ball is currently at is not within a 47 pixel range of the hitPixel. So when the player swings their device, the hit will only be registered if the ball is far away enough from the previous hit.

```C
if((message.Xval > 1.0 || message.Xval < -1.0
|| message.Yval > 1.0 || message.Yval < -1.0
|| message.Zval > 1.0 || message.Zval < -1.0)
&& (pixel > hitPixel+47 || pixel < hitPixel-47))
```

This helped to ensure that I could test the swing functionality easier, and prevented me from hitting the ball multiple times at once. Granting me easy access to testing and finding the optimal swing speed without the need for two players.

The final part to this experiment was setting up the player hit boundaries. In tennis each player is placed at either end of the court, divided by a net which they cannot cross. To replicate this in my solution, I decided to split the court into 1/3s.

By placing two white pixels evenly placed on the LED strip light I was able to display 3 sections: Player 1's side of the court, the net, and Player 2's side of the court. These positions were calculated by dividing the total number of pixels by 3.

```C
pixels.setPixelColor(47, pixels.Color(255,255,255));
pixels.setPixelColor(96, pixels.Color(255,255,255));
```

My plan now was to prevent the ball from being hit by any player in "the dead zone" or "the net", so I added more parameters to the swing conditions.

```C
if((message.Xval > 1.5 || message.Xval < -1.5
|| message.Yval > 1.5 || message.Yval < -1.5
|| message.Zval > 1.5 || message.Zval < -1.5)
&& (pixel > hitPixel+47 || pixel < hitPixel-47)
&& (pixel < 47 || pixel > 96))
```

Ensuring that the ball was smaller than 47 pixels OR grater than 96 pixels prevented myself from hitting the ball within the dead zone.

### Evidence: [See SWING-02.MOV]

Through iterations of user testing I can now use this tool to determine the best possible swing condition values, drawing this experiment to its completion.

## Next Step
The next step in the project now is to setup a second accelerometer, changing my single player solution to function with two players. This may involve adjusting the swing conditions and the type of data which the receiving feather board needs.

## Circuit Diagrams
### Receiver Circuit Diagram
![Receiver](<circuit receiver.png>)

### Transmitter Circuit Diagram
![Transmitter](<circuit transmitter.png>)

## References
https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts
https://arduinogetstarted.com/tutorials/arduino-neopixel-led-strip