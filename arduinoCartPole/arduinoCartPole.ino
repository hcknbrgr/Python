#include <MPU6050.h>    //Korneliusz Jarzebski’s MPU6050 library from GitHub.
#include <Wire.h>

float testArray[3]= {1.0, 2.0, 3.0};
/*

  On Board LED 
    OnBoardLED: Pin13 - leave this pin empty!!!

  Ultrasonic sensor pins:
    VCC: +5VDC
    Trig : Trigger (Input) - Pin11
    Echo: Echo (OUtput) - Pin12
    GND: GND
  
  MPU 6050 gyroscope pins:
    VCC: +5VDC
    GND: GND
    SCL: SCL
    SDA: SDA

  Motor pinout
    Motor1
      OUT1: Pin1
      OUT2: Pin2
      ENA: PWMPin5
    Motor2
      OUT3: Pin7
      OUT4: Pin8
      ENB: PWMPin10
*/

// MOTOR PINS
int motor1pin1 = 3;
int motor1pin2 = 2;
int motor1PWM = 5;
int motor2pin1 = 7;
int motor2pin2 = 8;
int motor2PWM = 10;
int boardLED = 13;


// ULTRASONIC SENSOR PINS
int trigPin = 11;   // Trigger
int echoPin = 12;   // Echo

MPU6050 mpu;
int initializedCart = 0;   // initialize the distance for the cart
int speed = 0;  // 150 - 255
int motorDirection = 3; // Serial Input to determine which direction to send the motor in. 3 = stationary
int velocity = 0;

void moveCart(int direction)
{
    // 0-2 towards wall 0 - fast, 2 - slow
    // 3 stop
    // 4-6 away from wall, 4 slow, 6 fast
    
    if(direction == 0)  // 0-2 towards wall 0 - fast, 2 - slow
    {
      speed = 250;
      analogWrite(motor1PWM, speed);
      analogWrite(motor2PWM, speed);
      digitalWrite(motor1pin1, HIGH);
      digitalWrite(motor1pin2, LOW);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, HIGH);
    } 
    else if(direction == 1)  // 0-2 towards wall 0 - fast, 2 - slow
    {
      speed = 200;
      analogWrite(motor1PWM, speed);
      analogWrite(motor2PWM, speed);
      digitalWrite(motor1pin1, HIGH);
      digitalWrite(motor1pin2, LOW);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, HIGH);
    } 
    else if(direction == 2)  // 0-2 towards wall 0 - fast, 2 - slow
    {
      speed = 150;
      analogWrite(motor1PWM, speed);
      analogWrite(motor2PWM, speed);
      digitalWrite(motor1pin1, HIGH);
      digitalWrite(motor1pin2, LOW);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, HIGH);
    } 
    else if(direction == 4)  // 4-6 away from wall, 4 slow, 6 fast
    {
      speed = 150;
      analogWrite(motor1PWM, speed);
      analogWrite(motor2PWM, speed);
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, HIGH);
      digitalWrite(motor2pin1, HIGH);
      digitalWrite(motor2pin2, LOW);
    }
    else if(direction == 5)  // 4-6 away from wall, 4 slow, 6 fast
    {
      speed = 200;
      analogWrite(motor1PWM, speed);
      analogWrite(motor2PWM, speed);
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, HIGH);
      digitalWrite(motor2pin1, HIGH);
      digitalWrite(motor2pin2, LOW);
    }
    else if(direction == 6)  // 4-6 away from wall, 4 slow, 6 fast
    {
      speed = 250;
      analogWrite(motor1PWM, speed);
      analogWrite(motor2PWM, speed);
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, HIGH);
      digitalWrite(motor2pin1, HIGH);
      digitalWrite(motor2pin2, LOW);
    }
    else  // 3 = stationary
    {
      speed=0;
      analogWrite(motor1PWM, speed);
      analogWrite(motor2PWM, speed);
      digitalWrite(motor1pin1, LOW);
      digitalWrite(motor1pin2, LOW);
      digitalWrite(motor2pin1, LOW);
      digitalWrite(motor2pin2, LOW);
    }
}  // END MOVE

int verticalInitialization = 0;
long distance = 0;
float poleAngle = 0;
float angularVelocity = 0;
float serialCommunication[4];  //[Cart Position, Cart velocity, Pole Angle, Pole Angular Velocity]

void setup() {
  // Serial Port begin
  Serial.begin(115200); 
  pinMode(boardLED, OUTPUT);
  digitalWrite(boardLED, LOW);  // turn the light off to check the MPU
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    delay(500);
  }
  mpu.calibrateGyro();
  digitalWrite(boardLED, HIGH); // LIGHT TURNS ON AFTER MPU IS ON, THEN YOU CAN RUN PYTHON SCRIPT
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
  // make sure the motors are off to start
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, LOW);
}

void loop() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse;

if(initializedCart == 0)
  {  
    moveCart(3);  // 1 is stop the cart
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    pinMode(echoPin, INPUT);
    distance = pulseIn(echoPin, HIGH);
    distance = (distance/2.0) / 29.1;  //int cm = (distance/2) / 29.1;
    Serial.println(distance);  
    Serial.flush();
    while(Serial.available()==0)
    {}
    if(Serial.available()>0) {
      motorDirection = Serial.read(); 
    }
    if(motorDirection == 1)
    {}
    else if(motorDirection == 3)  // 3 = motor pins are already in LOW/LOW, cart is initialized and ready to go
    {
      digitalWrite(boardLED, LOW);  // turn off the light that was turned on when the gyroscope was online
      initializedCart = 1;
      delay(500);
    }
  }  // END CART centered INITIALIZATION
  else if (initializedCart == 1)
  {
    
    Vector normAccel = mpu.readNormalizeAccel();
    poleAngle = (atan2(normAccel.XAxis, normAccel.ZAxis)*180.0)/M_PI;
    Serial.println(poleAngle);   
    Serial.flush();

    while(Serial.available()==0)
    {}

    if(Serial.available()>0) {
      verticalInitialization = Serial.read(); 
    }
    if(verticalInitialization == 0) {}
    else if (verticalInitialization == 1)
    {
      initializedCart = 2;
      digitalWrite(boardLED, HIGH);  // Cart is initialized and ready to go, turn on the board LED!
    }
    
  }
  else if (initializedCart == 2)  // CART IS INITIALIZED,
  {
    // Run the episode.  Send state information, execute action given from NN, repeat until get signal to end
    // The state for each step = [Cart position, cart velocity, pole angle, pole angle velocity]
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    pinMode(echoPin, INPUT);
    distance = pulseIn(echoPin, HIGH); // this is the cart position
    distance = (distance/2.0) / 29.1;  //int cm = (distance/2) / 29.1;
    if(motorDirection==0||motorDirection==1||motorDirection==2)
      velocity = -250+(motorDirection)*50;  
    else if(motorDirection==4||motorDirection==5||motorDirection==6)
      velocity = (motorDirection-1)*50;
    else
      velocity = 0;
    Vector normAccel = mpu.readNormalizeAccel();
    poleAngle = (atan2(normAccel.XAxis, normAccel.ZAxis)*180.0)/M_PI;
    Vector normGyro = mpu.readNormalizeGyro();
    angularVelocity=normGyro.YAxis;
    Serial.flush();
    Serial.print(distance);
    Serial.print('/');
    Serial.print(velocity);
    Serial.print('/');
    Serial.print(poleAngle);
    Serial.print('/');
    Serial.println(angularVelocity);
    Serial.flush();

    while(Serial.available()==0)
    {}
    if(Serial.available()>0) {
      motorDirection = Serial.read(); // incoming data = 0 towards wall, 1 away from wall, 2 End of episode
    }
    if(motorDirection == 9)
    {
      initializedCart = 0;
      moveCart(3); // end of episode, halt motor and restart
    }
    else
      moveCart(motorDirection);

  }
  
  delay(50);
  
}






