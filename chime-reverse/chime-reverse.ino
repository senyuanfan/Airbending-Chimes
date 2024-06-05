#include <Bounce.h>
#include <TeensyThreads.h> // Using the [TeensyThreads](https://github.com/ftrias/TeensyThreads) library.

// Ultrasonic Sensor
#define trigPin 3
#define echoPin 4

// Motor Driver
#define stp 13
#define dir 14
#define MS1 15
#define MS2 18
#define EN  19

// Define radius
const int steps = 300;

// Define min and max speeds
const int minSpeed = 6000;
const int maxSpeed = 200;

// Define the number of samples for the moving average
const int numSamples = 2;

// Create an array to hold the samples
int samples[numSamples];

// Variables to keep track of the sum and the current index
int sum = 0;
int ind = 0;

// Number of elements currently in the samples array
int count = 0;

void resetEDPins() {
  digitalWrite(stp, LOW);
  digitalWrite(dir, LOW);
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(EN, HIGH);
}

float measureDistance(int tPin, int ePin) {
  digitalWrite(tPin, LOW);
  threads.delay_us(2);
  digitalWrite(tPin, HIGH);
  threads.delay_us(10);
  digitalWrite(tPin, LOW);

  long duration = pulseIn(ePin, HIGH);
  float distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

float movingAverage;

void read() {
  while (true) {
    // Read a value from analog pin A0
    float d = measureDistance(trigPin, echoPin);

    // Subtract the oldest sample value from the sum
    sum -= samples[ind];

    // Replace the oldest sample with the new value
    samples[ind] = d;

    // Add the new sample value to the sum
    sum += d;

    // Increment the index and wrap around if necessary
    ind = (ind + 1) % numSamples;

    // Update the count (max is numSamples)
    if (count < numSamples) {
      count++;
    }

    // Calculate the moving average
    movingAverage = (float)sum / count;

    // Delay for a while before the next loop iteration
    threads.delay(100);
  }
}

float dtos(float d) {
  return 1875 + 187.934 - exp(0.0812636 * d) + 187.934; 
}

void setup() {
  // Ultrasonic Setup
  Serial.begin(9600);  // Initialize serial communication at 9600 baud
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Motor Setup
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(EN, OUTPUT);
  resetEDPins();

  // Initialize the samples array to zero
  for (int i = 0; i < numSamples; i++) {
    samples[i] = 0;
  }

  threads.addThread(read);

  // Set up striker
  digitalWrite(EN, LOW);

  for (int i = 0; i < round(steps / 2); i++) {
    digitalWrite(stp, HIGH); // Trigger one step
    threads.delay(20);
    digitalWrite(stp, LOW); // Pull step pin low so it can be triggered again
    threads.delay(20);
  }
}

void loop() {
  threads.delay(300);

  digitalRead(dir) == HIGH ? digitalWrite(dir, LOW) : digitalWrite(dir, HIGH);

  for (int i = 0; i < steps; i++) {
    int speed = max(maxSpeed, min(minSpeed, round(dtos(movingAverage))));
    digitalWrite(stp, HIGH); // Trigger one step
    threads.delay_us(speed);
    digitalWrite(stp, LOW); // Pull step pin low so it can be triggered again
    threads.delay_us(speed);
  }
}
