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
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
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
    d = min(d, 80);

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
}

void loop() {
  digitalWrite(EN, LOW);

  digitalRead(dir) == HIGH ? digitalWrite(dir, LOW) : digitalWrite(dir, HIGH);

  for (int i = 0; i < 50; i++) {
    int speed = round(map(movingAverage, 1, 80, 1, 20));
    digitalWrite(stp, HIGH); // Trigger one step
    threads.delay(speed);
    digitalWrite(stp, LOW); // Pull step pin low so it can be triggered again
    threads.delay(speed);
  }

  threads.delay(300);
}
