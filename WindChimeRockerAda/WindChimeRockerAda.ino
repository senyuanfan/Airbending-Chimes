#include <Stepper.h>



#define S1 13
#define S2 14
#define S3 15
#define S4 18
#define EN  19

#define PWMA 22
#define PWMB 23

// ultrasonic pin and variable sensor setup
#define trigPin1 3
#define echoPin1 4
#define trigPin2 5
#define echoPin2 6

#define TOTAL_STEPS 100

// set the number of steps for each motion
int numsteps_base = 30;
int numsteps;
int numswings = 3;

int stepperSpeed = 60;// stepper motor speed in rpm

// create an instance of the stepper class, specifying
// the number of steps of the motor and the pins it's
// attached to
Stepper stepper(TOTAL_STEPS, S1, S2, S3, S4);

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

int trigger = 0;
int idle = 0;
int reset_timer = 30;


void setup()
{
  Serial.begin(9600);
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S4, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  digitalWrite(PWMA, HIGH); //Pull enable pin low to allow motor control
  digitalWrite(PWMB, HIGH); //Pull enable pin low to allow motor control
  // digitalWrite(EN, HIGH); //Pull enable pin low to allow motor control

  // set the speed of the motor to 30 RPMs
  stepper.setSpeed(stepperSpeed);
}

void loop(){
    // digitalWrite(EN, HIGH);

  // Serial.println("Forward");
  // stepper.step(STEPS);

  // delay(200);

  // Serial.println("Backward");
  // stepper.step(-STEPS);


  // delay(200);

  d1 = measureDistance(trigPin1, echoPin1);
  d1 = min(d1, gate);

  // Serial.println(distance1);
  // mvavg = (1 - lambda) * mvavg + lambda * d1;
  // gatedavg = min(mvavg, 50);

  pd1 = d1 - 0.95 * pd1;
  Serial.println(pd1);

  // if(abs(pd1) > threshold)
  if(pd1 < threshold)
  {

    Serial.print("Motion Detected, Value: ");
    Serial.println(pd1);
    
    if( trigger == 0 ){
      ForwardStep(numsteps_base);
      // stepper.step(STEPS);
    }
    else if( (trigger % 2) == 0 ){
      ForwardStep(numsteps_base * 2);
    }
    else{
      ReverseStep(numsteps_base * 2);
    }
    
    // ForwardBackwardStep();
    trigger += 1;
    idle = 0;

    // resetEDPins();
    // delay(100);
  }
  else{
    idle += 1;
  }

  if( (idle > reset_timer) && (trigger != 0) ){
    Serial.println("Reset");
    // reset striker location
    if( (trigger % 2) == 0 ){
      ForwardStep(numsteps_base);
    }
    else{
      ReverseStep(numsteps_base);
    }
    trigger = 0;
    idle = 0;
  }

  // Serial.print("Trigger: ");
  // Serial.println(trigger);

  // Serial.print("Idle:);
  // Serial.println(idle);

  pd1 = d1;

  delay(50);

}

void ForwardStep(int stp){
  digitalWrite(EN, HIGH); //Pull enable pin low to allow motor control
  Serial.print("Moving forward at steps:");
  Serial.println(stp);
  stepper.step(stp);
  digitalWrite(EN, LOW);
}

void ReverseStep(int stp){
  digitalWrite(EN, HIGH);
  Serial.print("Moving reverse at steps:");
  Serial.println(stp);
  stepper.step(-stp);
  digitalWrite(EN, LOW);
}

float measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}