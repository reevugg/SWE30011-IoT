#include <Servo.h>

#define TRIGGER_PIN 2
#define ECHO_PIN 3
const int ledPin = 8;
int command=0;
int gate=13;

Servo servoMotor; // Create a servo object

void setup() {
  servoMotor.attach(9); // Attach the servo to pin 9

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  long duration, distance;
  if (Serial.available() > 0) {
                                command = Serial.parseInt(); // Read the input value
                                }
  if(command==21){gate =11;}
  else if(command==22){gate =12;}
  else if(command==23){gate=13;}

  // Send a short pulse to the trigger pin
  digitalWrite(TRIGGER_PIN, LOW);       
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Measure the duration of the pulse on the echo pin
  duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance in centimeters
  distance = duration * 0.034 / 2;

  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.print("cm ");
  

  // Control the servo based on the distance
  if (distance <= 10 && gate == 13) {
                                     servoMotor.write(0); // Rotate the servo to 0 degrees
                                      digitalWrite(ledPin, HIGH);
                                      Serial.print("LIGHT: On ");
                                   } 
  if(distance >= 10 && gate == 13) 
                                      {servoMotor.write(180);
                                       digitalWrite(ledPin, LOW);
                                      Serial.print("LIGHT: Off ");
                                      }
 if(gate==11){servoMotor.write(180);Serial.print("LIGHT: On ");digitalWrite(ledPin, HIGH);}
 if(gate==12){servoMotor.write(0);Serial.print("LIGHT: Off ");digitalWrite(ledPin, LOW);}
 if(gate==13){Serial.println("LIGHTSTATE: Auto");}
 else{Serial.println("LIGHTSTATE: Manual");}
                                      
                      delay(500); // Delay between each measurement
                      
}
