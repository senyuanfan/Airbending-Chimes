#include <Stepper.h>

// change this to the number of steps on your motor
#define STEPS 200

#define S1 13
#define S2 14
#define S3 15
#define S4 18
#define EN  19

#define PWMA 22
#define PWMB 23

// ultrasonic pin and variable sensor setup
#define trigPin1 = 3;
#define echoPin1 = 4;
#define trigPin2 = 5;
#define echoPin2 = 6;

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(STEPS, S1, S2, S3, S4);

// motion sensing code
int x;
int y;
int state;

float d1;
float d2;

float pd1 = 0.0;
float pd2 = 0.0;

float total1 = 0;             // Sum of readings for sensor 1
float total2 = 0;             // Sum of readings for sensor 2
float average1 = 0;           // Average distance for sensor 1
float average2 = 0;           // Average distance for sensor 2

float lambda = 0.95;
float mvavg = 0.0;
float gatedavg = 0.0;

float gate = 100;
float threshold = -10;
int mdelay = 500;

// set the number of steps for each motion
int numsteps_base = 150;
int numsteps;
int numswings = 3;

int trigger = 0;
int idle = 0;
int reset_timer = 30;


void setup()
{
  Serial.begin(9600);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S4, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  digitalWrite(PWMA, HIGH); //Pull enable pin low to allow motor control
  digitalWrite(PWMB, HIGH); //Pull enable pin low to allow motor control
  digitalWrite(EN, HIGH); //Pull enable pin low to allow motor control


  Serial.println("Stepper test!");
  // set the speed of the motor to 30 RPMs
  stepper.setSpeed(60);
}

void loop()
{
  // digitalWrite(EN, HIGH);

  Serial.println("Forward");
  stepper.step(STEPS);

  delay(200);

  Serial.println("Backward");
  stepper.step(-STEPS);


  delay(200);
}