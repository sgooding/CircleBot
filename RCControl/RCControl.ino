
#include <AFMotor.h>
#include <Servo.h> 

Servo servo1;
AF_DCMotor right_motor(2);
AF_DCMotor left_motor(1);

#define DEBUG false

#define CH0 A4
#define CH1 A5
#define CH2 A3
#define MIN_DURATION 1100
#define MAX_DURATION 1900
#define DEADBAND 50
#define MAX_SPEED 200

boolean AUTO_ENABLED = false;
boolean SERVO_UP = false;
int GAIN = 10;

void setup() {
  // put your setup code here, to run once:
  // turn on motor
  right_motor.run(RELEASE);
  left_motor.run(RELEASE);

  pinMode(CH0,INPUT);
  pinMode(CH1,INPUT);
  pinMode(CH2,INPUT);
  servo1.attach(9);

  Serial.begin(9600);
  
}

int up_down_pwm = 0;
int left_right_pwm = 0;

int left_motor_speed = 0;
int right_motor_speed = 0;
int left_bump = 0;
int right_bump = 0;

void loop() {
  // put your main code here, to run repeatedly:
  

  // read up-down channel
  up_down_pwm = pulseIn(CH0, HIGH);
  left_right_pwm = pulseIn(CH1, HIGH);
  AUTO_ENABLED = pulseIn(CH2,HIGH) > 1500 ? true : false;

  // map to 0-255
  up_down_pwm = map(up_down_pwm, MIN_DURATION, MAX_DURATION, -255, 255);
  left_right_pwm = map(left_right_pwm, MIN_DURATION, MAX_DURATION, -255, 255);

   if( (abs(up_down_pwm) < DEADBAND) 
    or (up_down_pwm < -1000) )
   {
      up_down_pwm = 0;
   }

   
   if( (abs(left_right_pwm) < DEADBAND) 
    or (left_right_pwm < -1000) )
   {
      left_right_pwm = 0;
   }

  // mix command
  right_motor_speed = (up_down_pwm - left_right_pwm);
  left_motor_speed = (up_down_pwm + left_right_pwm);



  int left_dir, right_dir;
  left_dir = (left_motor_speed<0?FORWARD:BACKWARD);
  right_dir = (right_motor_speed<0?FORWARD:BACKWARD);

  left_motor_speed = constrain(abs(left_motor_speed),0,MAX_SPEED);
  right_motor_speed = constrain(abs(right_motor_speed),0,MAX_SPEED);


  if(Serial.available()>0)
  {
    char incomingByte = Serial.read();
    left_bump = 0;
    right_bump = 0;
    switch(incomingByte)
    {
      case 'L':
      left_bump = -GAIN;
      right_bump = GAIN;
      break;
      case 'R':
      right_bump = -GAIN;
      left_bump = GAIN;
      break;
      case 'U':
      SERVO_UP = true;
      break;
      case 'D':
      SERVO_UP = false;
      break;
    }
  }

  if( AUTO_ENABLED )
  {
    left_motor_speed += left_bump;
    right_motor_speed += right_bump;
    servo1.write(SERVO_UP?250:10);
  }
  left_motor.run(left_dir);
  left_motor.setSpeed(int(float(left_motor_speed)*0.9));
  right_motor.run(right_dir);
  right_motor.setSpeed(right_motor_speed);


  if(DEBUG)
  {
    Serial.println(" **************** ");
    Serial.print(" PWM CH0 : ");
    Serial.println( up_down_pwm );
    Serial.print(" PWM CH1 : ");
    Serial.println( left_right_pwm );
    Serial.print(" Left Motor Dir : ");
    Serial.println(left_dir);
    Serial.print(" Right Motor Dir : ");
    Serial.println(right_dir);
    Serial.print(" Left Motor Speed : ");
    Serial.println(left_motor_speed);
    Serial.print(" Right Motor Speed : ");
    Serial.println(right_motor_speed);
  }

  delay(100);
}
