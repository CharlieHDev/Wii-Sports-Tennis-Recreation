# 01 - Reading acceleration data and accounting for gravity.

## Experiment Description
My first step in creating this experience was setting up a feather board which could read acceleration data from an accelerometer. This experiment utilises the MPU6050 Accelerometer to read a set of data which determines how quickly the device is moving, in order to read the players movements and detect when the player is swinging their hand to hit the tennis ball.

This experiment also aims to solve the issue of gravity effecting the acceleration data. Previous experiments found that the data taken from the MPU6050 did not account for the gravity effecting the acceleration data.

## Components
### 1x Adafruit HUZZAH32 - ESP32 Feather

### 1x MPU6050 Accelerometer and Gyroscope Module

## Walkthrough (Record of Troubleshooting and Success)
To start this experiment, I had to import the required libraries for the accelerometer. Given that I was using feather boards to send this data from one board to the central point board I was able to take my previous code sample of a transmitter feather board and use it here to test the accelerometer. Being able to transmit the data from the feather board was not a priority at this point, so the ESP setup code was left untouched.

My initial concept for tackling the gravity issue was to use the rotational data taken from the gyroscope readings on the MPU6050. If I could determine what direction the accelerometer was facing, then I could apply this to the acceleration data. My thought process was to use the X, Y and Z axis of rotation to determine the devices facing direction.

I tried mapping the rotational values to a multiple of the gravity [1g = 9.81 m/s²]. I tried to sketch out my thought process. While on paper it made sense to me, the concept was flawed and not something I was able to replicate in code.

### Evidence: [See ACCEL-01.jpg]

This method was put aside, until I was presented with a solution which provided the desired results I was looking for which I could apply to the acceleration data to account for gravity.

I was introduced to the use of trigonometry calculations which at first I did not understand. With thorough research and breaking down step by step what these calculations were doing help me understand the values I was being given and how they could be used to benefit my solution.

To start, I had to convert the rotation value in degrees on the X and Y axis to radians, this is important as commonly in C/C++ programming it is expected when working with trigonometry calculations that the units are measured in radians.

Radians are the calculation of how many times a radius can fit around a circle. (e.g. 180 degrees = pi).

```C
float SinX = mpu.getAngleX()*(M_PI/180);
float SinY = mpu.getAngleY()*(M_PI/180);
```

Following this, using the X axis rotation and the Y axis rotation we can convert these values into a 3D vector which calculates which way the device is facing.

Sin and Cos are used here to help create a cartesian value. They map the rotation angle to coordinates on a circle for 2D or a sphere for 3D. This in effect provides the desired results I need to apply to the accelerometer data. Providing a value between -1, and 1 for each axis using these calculations.

```C
float xVal = cos(SinX) * cos(SinY);
float yVal = sin(SinX) * cos(SinY);
float zVal = sin(SinY);
```

With the provided values from these calculations, the last step was to apply them to the existing accelerometer data to in theory subtract the gravity factor from the data being read.

```C
message.Xval = mpu.getAccX()-xVal;
message.Yval = mpu.getAccY()-yVal;
message.Zval = mpu.getAccZ()-zVal;
```

### Evidence: [See ACCEL-02.MOV]

After attempting this solution, I found the results were all confusing, certain rotations effected the wrong axis. X and Y axis were producing the results of each other and I couldn't understand why. I flipped the calculations which fixed the Z value output, but the X axis was still off.

I discovered the reasoning behind these incorrect details was due to my misunderstanding of what these values were doing. The calculations provide a unit pointing in the direction gravity is pulling. So certain axis have different effects on others, such as gravity tilting on the X axis when the device is rotated on the Y axis.

The X value is added to as gravity is pulling the sensor down which is perceived as a negative value.

```C
message.Xval = mpu.getAccX()+zVal;
message.Yval = mpu.getAccY()-yVal;
message.Zval = mpu.getAccZ()-xVal;
```

With these calculations updated, I was able to produce promising results, with the values sitting at roughly 0, 0, 0 when sat on one side where gravity previously effected the data being read.

### Evidence: [See ACCEL-03.MOV]

## Circuit Diagrams
![Circuit Diagram](circuit.png)

## References
https://blog.truegeometry.com/calculators/Find_the_value_of_sin_for_30_calculation.html
https://www.mathsisfun.com/data/cartesian-coordinates.html
https://www.mathsisfun.com/algebra/trigonometry.html