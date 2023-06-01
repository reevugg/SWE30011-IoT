#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>
#include <Servo.h>
#define DHTPIN 6 // Pin to which DHT11 sensor is connected
#define DHTTYPE DHT11 // DHT sensor type
DHT dht(DHTPIN, DHTTYPE);
#define RST_PIN 9    // RST pin for MFRC522 RFID module
#define SS_PIN 10    // SS pin for MFRC522 RFID module
#define TRIGGER_PIN 2 //SR
#define ECHO_PIN 3 //SR
Servo servoMotor; // Create a servo object
const int ledPin = 8;
const int led=7;
int protect=0;
int mprotect=0;
const int buzz=5;
String uidString;
long duration, distance;
float temperature;
float humidity;
int thefti=0;
int auth=0;
int acd=0;
int lightd=0;
int command=0;
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create an instance of the MFRC522 RFID module
                                     

void setup() {

  Serial.begin(9600);   // Start serial communication
   dht.begin();

  SPI.begin();          // Initialize SPI bus
  mfrc522.PCD_Init();   // Initialize MFRC522 RFID module
  servoMotor.attach(4); // Attach the servo to pin 9

  //Serial.println("Scan your RFID card/tag...");
   pinMode(ledPin, OUTPUT);
    pinMode(led, OUTPUT);
    pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(buzz, OUTPUT);
  
}
void temp(){temperature = dht.readTemperature();humidity= dht.readHumidity();}

void card()
         { // Check for new RFID cards/tags
             if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
              // Get the UID of the card/tag
                uidString = "";
              for (byte i = 0; i < mfrc522.uid.size; i++) {
               uidString += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
                 uidString += String(mfrc522.uid.uidByte[i], HEX);
                 
    }

    // Print the UID
    //Serial.print("RFID UID: ");
    //Serial.println(uidString);

    mfrc522.PICC_HaltA();        // Halt the card/tag
    mfrc522.PCD_StopCrypto1();   // Stop encryption on the card/tag

    delay(2000);  // Wait for 2 seconds before scanning for the next card/tag
          
          }
         }

void motion()
             {digitalWrite(TRIGGER_PIN, LOW);
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
               //Serial.println(" cm");
               if(distance<=10){mprotect=1;}
              
              }
void buzzd(){digitalWrite(buzz, HIGH);delay(200);digitalWrite(buzz, LOW);}
void lock()
          { if(uidString== "6ace2fae" ){gate(1);auth=1;}
            else if(uidString=="dade123f"){gate(2);auth=2;}
            else{digitalWrite(ledPin, LOW);auth=0;}

            }
void gate(int fuck)
                   {if(fuck==1){digitalWrite(ledPin, HIGH);buzzd();uidString="0";protect=0;mprotect=0; auth=1; servoMotor.write(0);} // Rotate the servo to 0 degrees
                   if(fuck==2){auth=2; buzzd();uidString="0";protect=1;servoMotor.write(180);} // Rotate the servo to 0 degree
                    }
void theft(){if(mprotect==1 && protect==1){buzzd();thefti=1;}else{thefti=0;}}
void sout()
           {
            if(thefti==1){Serial.print("ON ");}else{Serial.print("OFF ");}
            if(auth==1){Serial.print("YES ");}
            else if(auth==2){Serial.print("NO ");}else{{Serial.print("NULL ");}}
             if(auth==1){Serial.print("OPEN ");delay(200);}else{Serial.print("CLOSE ");}
             if(acd==1){Serial.print("ON ");}else{Serial.print("OFF ");}
             if(lightd==1){Serial.print("ON ");}else{Serial.print("OFF ");}
             //Serial.print("Temperature: ");
             Serial.print(temperature);
             //Serial.print(" Humidity: ");
             Serial.print(" ");
             Serial.println(humidity);
            //Serial.println(auth);
             
             
            }


void sein()
          {                     if (Serial.available() > 0) {
                                                               command = Serial.parseInt(); // Read the input value
                                                           }
                                if(command==1){mprotect=0;protect=0;}
                                if(command==2){gate(1);}
                                if(command==3){gate(2);}
                                if(command==4){lightd=1;digitalWrite(led, HIGH);}
                                if(command==5){lightd=0;digitalWrite(led, LOW);}
                                if(command==6){acd=1;}
                                if(command==7){acd=0;}
            
            }
void loop() { 
               sein();
               sout();
              motion();
               card();
               lock();
              theft();
              temp();
              
              //delay(500);
            }
