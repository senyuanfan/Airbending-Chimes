#include <Bounce.h>

// motor pin and variable setup
#define stp 13
#define dir 14
#define MS1 15
#define MS2 18
#define EN  19

#define SLP 22

// ultrasonic pin and variable sensor setup
#define trigPin1 3
#define echoPin1 4
#define trigPin2 5
#define echoPin2 6


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

void setup() {

  // ultrasonic setup
  Serial.begin(9600);  // Initialize serial communication at 9600 baud
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  // pinMode(trigPin2, OUTPUT);
  // pinMode(echoPin2, INPUT);

  // motor setup
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);

  // pinMode(SLP, OUTPUT);
  // digitalWrite(SLP, HIGH);

  resetEDPins();
}


void loop() {

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

    resetEDPins();
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

  // Serial.print("Idle: ");
  // Serial.println(idle);

  pd1 = d1;

  delay(100);

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

void resetEDPins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);

  // set MS pings to HIGH-HIGH for 1/8 step mode
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);

  digitalWrite(EN, HIGH);
}

// 1/8th microstep foward mode function, modified for forward and backward step mode
void SmallStepMode()
{
  Serial.println("Stepping at 1/8th microstep mode.");
  digitalWrite(dir, LOW); //Pull direction pin low to move "forward"
  digitalWrite(EN, LOW); //Pull enable pin low to allow motor control

  digitalWrite(MS1, HIGH); //Pull MS1, and MS2 high to set logic to 1/8th microstep resolution
  digitalWrite(MS2, HIGH);
  for(x= 0; x<numsteps; x++)  //Loop the forward stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step forward
    delayMicroseconds(mdelay);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delayMicroseconds(mdelay);
  }
  Serial.println("Enter new option");
  Serial.println();
}

void ForwardStep(int steps)
{
  Serial.print("Moving forward at default steps:");
  Serial.println(steps);

  digitalWrite(EN, LOW); //Pull enable pin low to allow motor control
  digitalWrite(dir, LOW); //Pull direction pin low to move "forward"
  for(x= 0; x<steps; x++)  //Loop the forward stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step forward
    delayMicroseconds(mdelay);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delayMicroseconds(mdelay);
  }
}

//Reverse default microstep mode function
void ReverseStep(int steps)
{
  Serial.print("Moving in reverse at steps:");
  Serial.println(steps);

  digitalWrite(EN, LOW); //Pull enable pin low to allow motor control
  digitalWrite(dir, HIGH); //Pull direction pin high to move in "reverse"
  for(x= 0; x<steps; x++)  //Loop the stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step
    delayMicroseconds(mdelay);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delayMicroseconds(mdelay);
  }
}

//Forward/reverse stepping function
void ForwardBackwardStep()
{
  Serial.println("Alternate between stepping forward and reverse.");
  digitalWrite(EN, LOW); //Pull enable pin low to allow motor control

  for(x= 0; x<numswings; x++)  //Loop the forward stepping enough times for motion to be visible
  {
    //Read direction pin state and change it
    state=digitalRead(dir);
    if(state == HIGH)
    {
      digitalWrite(dir, LOW);
    }
    else if(state ==LOW)
    {
      digitalWrite(dir,HIGH);
    }
    
    if( x == (numswings - 1) ){
      numsteps = numsteps_base;
    }
    else if( x == 0 ){
      numsteps = numsteps_base;
    }
    else{
      numsteps = numsteps_base * 2;
    }
    Serial.println(numsteps);
    for(y=0; y<numsteps; y++)
    {
      digitalWrite(stp,HIGH); //Trigger one step
      delay(1);
      digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
      delay(1);
    }
    delay(200);
  }
  
}
