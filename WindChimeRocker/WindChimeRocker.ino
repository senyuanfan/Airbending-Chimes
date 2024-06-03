#include <Bounce.h>

// ultrasonic pin and variable sensor setup
const int trigPin1 = 3;
const int echoPin1 = 4;
const int trigPin2 = 5;
const int echoPin2 = 6;

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

float threshold = 50;

// motor pin and variable setup
#define stp 13
#define dir 14
#define MS1 15
#define MS2 18
#define EN  19

#define SLP 22

char user_input;
int x;
int y;
int state;

// set the number of steps for each motion
int numsteps_base = 150;
int numsteps;
int numswings = 3;

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

  pinMode(SLP, OUTPUT);

  digitalWrite(SLP, HIGH);
  digitalWrite(EN, LOW);

  resetEDPins();
}


void loop() {

  d1 = measureDistance(trigPin1, echoPin1);
  d1 = min(d1, 80);

  // Serial.println(distance1);
  mvavg = (1 - lambda) * mvavg + lambda * d1;
  gatedavg = min(mvavg, 50);

  pd1 = d1 - 0.9 * pd1;
  Serial.println(pd1);

  if(pd1 > threshold)
  {
    Serial.println("Motion Detected");
    digitalWrite(EN, LOW); //Pull enable pin low to allow motor control
    ForwardBackwardStep();
    resetEDPins();
    delay(2000);
  }
  pd1 = d1;

  delay(200);

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
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(EN, HIGH);
}

// 1/8th microstep foward mode function, modified for forward and backward step mode
void SmallStepMode()
{
  Serial.println("Stepping at 1/8th microstep mode.");
  digitalWrite(dir, LOW); //Pull direction pin low to move "forward"
  digitalWrite(MS1, HIGH); //Pull MS1, and MS2 high to set logic to 1/8th microstep resolution
  digitalWrite(MS2, HIGH);
  for(x= 0; x<numsteps; x++)  //Loop the forward stepping enough times for motion to be visible
  {
    digitalWrite(stp,HIGH); //Trigger one step forward
    delay(1);
    digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
    delay(1);
  }
  Serial.println("Enter new option");
  Serial.println();
}

//Forward/reverse stepping function
void ForwardBackwardStep()
{
  Serial.println("Alternate between stepping forward and reverse.");
  digitalWrite(MS1, LOW); //Pull MS1, and MS2 high to set logic to 1/8th microstep resolution
  digitalWrite(MS2, HIGH);

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
    delay(300);
  }
  
}
