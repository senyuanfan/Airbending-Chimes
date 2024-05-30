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

// motor pin and variable setup
#define stp 13
#define dir 14
#define MS1 15
#define MS2 18
#define EN  19

char user_input;
int x;
int y;
int state;

// set the number of steps for each motion
int num_steps = 300;
int num_moves = 3;

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
  resetEDPins();
}


float threshold = 30;

void loop() {

  d1 = measureDistance(trigPin1, echoPin1);
  d1 = min(d1, 100);
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
  // if( gatedavg < 40 ){
  //   ForwardBackwardStep();
  //   resetEDPins();
  // }
  // if( detectHandWave(distance1, prevDistance1, 50) ){
  //   digitalWrite(EN, LOW); //Pull enable pin low to allow motor control
  //   ForwardBackwardStep();
  //   resetEDPins();
  //   delay(5000); // Delay for 100 milliseconds
  // }

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

bool detectHandWave(float currentDistance, float& previousDistance, float threshold) {
  // Check if the difference between the current and previous distance is greater than the threshold
  if (abs(currentDistance - previousDistance) > threshold) {
    // Serial.println("Hand wave detected!");
    // Reset the previous distance to the current distance
    // previousDistance = currentDistance;
    return true;
  } else {
    // Update the previous distance to the current distance
    previousDistance = currentDistance;
    return false;
  }
}

void resetEDPins()
{
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(EN, HIGH);
}

// 1/8th microstep foward mode function, modified for forward and backward step mode
void SmallStepMode()
{
  Serial.println("Stepping at 1/8th microstep mode.");
  digitalWrite(dir, LOW); //Pull direction pin low to move "forward"
  digitalWrite(MS1, HIGH); //Pull MS1, and MS2 high to set logic to 1/8th microstep resolution
  digitalWrite(MS2, HIGH);
  for(x= 0; x<num_steps; x++)  //Loop the forward stepping enough times for motion to be visible
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
  for(x= 1; x<num_moves; x++)  //Loop the forward stepping enough times for motion to be visible
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
    
    for(y=0; y<num_steps; y++)
    {
      digitalWrite(stp,HIGH); //Trigger one step
      delay(1);
      digitalWrite(stp,LOW); //Pull step pin low so it can be triggered again
      delay(1);
    }

    // put a delay before reversing direction to prevent gliding 
    delay(500);
  }
}
