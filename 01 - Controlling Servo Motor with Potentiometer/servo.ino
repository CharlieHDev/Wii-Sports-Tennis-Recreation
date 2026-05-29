/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo myservo;  // create Servo object to control a servo
// twelve Servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
}

void loop() {

  int sensorReading = analogRead(A0);

  int position = map(sensorReading, 0, 1023, 90, 0);

  Serial.println(position);

  if(pos < position) {
    pos++;
  } else if (pos > position) {
    pos--;
  }
  myservo.write(pos);


}
