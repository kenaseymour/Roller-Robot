#include "WProgram.h"
void setup();
static void RampUp (int out1, int out2, int step, int delay1);
static void RampDown (int out1, int out2, int step, int delay1);
static void InitialRampUp ();
static void TurnLeft ();
static void TurnRight ();
static int Speed (int closenessCenter);
void loop();
int motor1Enable = 2;
int motor1Forward = 6;
int motor1Reverse = 5;

int motor2Enable = 3;
int motor2Forward = 10;
int motor2Reverse = 9;

int SensorLeft = 0;
int SensorFront = 1;
int SensorRight = 2;

int max = 300;
int speed = 0;
int lSpeed = 0;
int rSpeed = 0;

enum RollerState { Initial, Stopped, Moving };
RollerState state = Initial;

void setup()                    // run once, when the sketch starts
{
  pinMode (motor1Enable, OUTPUT);
  pinMode (motor1Forward, OUTPUT);
  pinMode (motor1Forward, OUTPUT);
  pinMode (motor2Enable, OUTPUT);
  pinMode (motor2Forward, OUTPUT);
  pinMode (motor2Forward, OUTPUT);

  delay (1500);

  // Get both motors ready
  digitalWrite (motor1Enable, HIGH);
  digitalWrite (motor1Forward, LOW);
  digitalWrite (motor1Reverse, LOW);
  digitalWrite (motor2Enable, HIGH);
  digitalWrite (motor2Forward, LOW);
  digitalWrite (motor2Reverse, LOW);
  
  Serial.begin (9600);
}

static void RampUp (int out1, int out2, int step, int delay1)
{
  int val;
  for (val = 0; val < max; val += step)
  {
    analogWrite (out1, val);
    analogWrite (out2, val);
    delay (delay1);
  }
}

static void RampDown (int out1, int out2, int step, int delay1)
{
  int val;

  for (val = max; val >= 0; val -= step)
  {
    analogWrite (out1, val);
    analogWrite (out2, val);
    delay (delay1);
  }
}

static void InitialRampUp ()
{
  int step = 50;
  int val;
  int delay1 = 350;
  
  for (val = 0; val < max; val += step)
  {
    analogWrite (motor1Forward, val);
    analogWrite (motor2Forward, val);
    delay (delay1);
  }
}

static void TurnLeft ()
{
  RampUp (motor1Forward, motor2Reverse, 10, 10);
  RampDown (motor1Forward, motor2Reverse, 10, 10);
}

static void TurnRight ()
{
  RampUp (motor1Reverse, motor2Forward, 10, 10);
  RampDown (motor1Reverse, motor2Forward, 10, 10);
}

static int Speed (int closenessCenter)
{
    if (closenessCenter < 100)
      return max;

    if (closenessCenter < 120)
      return 150;

    if (closenessCenter < 200)
      return 75;

    if (closenessCenter < 300)
      return 30;

    return 0;
}

void loop()
{
  int closenessCenter = analogRead (SensorFront);
  int leftCloseness = analogRead (SensorLeft);
  int rightCloseness = analogRead (SensorRight);
  int newSpeed = 0;
  int deltaLeft = 0, deltaRight = 0;
  char msg[80];
  
//  sprintf (msg, "state: %d, speed: %d, closenessCenter: %d", state, speed, closenessCenter);
//  Serial.println (msg);

  switch (state)
  {
  case Initial:
     if (closenessCenter < 300)
        state = Stopped;
     break;
    
  case Stopped:
     if (closenessCenter >= 300)
     {
       // TODO: add backup logic
       
        sprintf (msg, "leftCloseness: %d, rightCloseness: %d", leftCloseness, rightCloseness);
        Serial.println (msg);

       if (leftCloseness > rightCloseness)
       {
         TurnRight ();
       }
       else
       {
         TurnLeft ();
       }
       
        delay (200);
        return;
     }
     else
        state = Moving;
     return;

  case Moving:
    newSpeed = Speed (closenessCenter);
    if (newSpeed == 0)
       state = Stopped;

    // I need to investigate PID loops.
    if ((newSpeed - speed) > 100)
       newSpeed = speed + (newSpeed - speed) / 2;
      
#if 1
    // Adjust right/left speed
    
    int adjust = 0, b = 30;
    if (leftCloseness > rightCloseness)
    {
      adjust = (leftCloseness - rightCloseness) / b;
      
      lSpeed = newSpeed + adjust;
      rSpeed = newSpeed - adjust;
    }
    else
    {
      adjust = (rightCloseness - leftCloseness) / b;
      
      lSpeed = newSpeed - adjust;
      rSpeed = newSpeed + adjust;
    }

  sprintf (msg, "closenessCenter: %d, lSpeed: %d, rSpeed: %d", closenessCenter, lSpeed, rSpeed);
  Serial.println (msg);

    analogWrite (motor1Forward, lSpeed);
    analogWrite (motor2Forward, rSpeed);
#else
   // old way
    
    speed = newSpeed; // be more gradual later   

    analogWrite (motor1Forward, speed);
    analogWrite (motor2Forward, speed);
#endif
    break;
  }

  delay (250);
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

