#include <Servo.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

const byte ROWS = 4;
const byte COLS = 4;
const int passwordLength = 4;

//const int txPin = 8;
//const int rxPin = 9;
//SoftwareSerial mySerial(rxPin, txPin);

#define SOUND_SENSOR_PIN A2
#define SERVO_PIN 6

Servo servoMotor;

int soundThreshold = 300;
int servoPosition = 0;

int i = 0;
int debounceDelay = 50; 
unsigned long lastDebounceTime = 0;
bool soundDetected = false;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 11, 10};
byte colPins[COLS] = {5, 4, 3, 2};

char password[passwordLength + 1] = "";
char input[passwordLength + 1] = "";
int position = 0;
bool passwordSet = false;

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  //pinMode(rxPin, INPUT);
  //pinMode(txPin, OUTPUT);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  pinMode(SOUND_SENSOR_PIN, INPUT);
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(0); 
  delay(500); 
}

int melody[] = {
  262, 294, 330, 349, 392, 440, 494, 523, 494, 440, 392, 349, 330, 294, 262  
};

int noteDurations[] = {
  400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 400, 800
};

void loop() {
  char key = keypad.getKey();
  //mySerial.write('1');
  
  Serial.write(key);
  
  if (key) {
    if (!passwordSet && position < passwordLength) {

      password[position++] = key;

      if (position == passwordLength) {
        password[position] = '\0';
        passwordSet = true;
        position = 0;
        Serial.println("Password set!");
        for (int i = 0; i < 15; i++) {
          int noteDuration = noteDurations[i];
          tone(7, melody[i], noteDuration);
          delay(noteDuration * 1.3); 
        }
      }

    } else if (passwordSet && position < passwordLength) {

      input[position++] = key;

      if (position == passwordLength) {
        input[position] = '\0';  
        position = 0;

        if (strcmp(password, input) == 0) {
          Serial.println("Password correct! Unlocking...");
          Serial.println("Sending unlock signal...");
          delay(1200);

          digitalWrite(8, HIGH);
        

          if (servoPosition < 10 && servoPosition > -10) { 
            openCradle(); 
          } 
          else {
            closeCradle();
          }

        } 
        else if (strcmp('AAAA', input) == 0) {
          digitalWrite(8, LOW);
        }
        else {
          tone(7, 500, 500);
          delay(1000);
          Serial.println("Incorrect password. Try again.");
          digitalWrite(8, LOW);
          digitalWrite(9, HIGH);
          delay(500);
          digitalWrite(9, LOW);
          digitalWrite(8, HIGH);
        }

      }
    }
  }


  

  int soundValue = analogRead(SOUND_SENSOR_PIN);
  Serial.println(soundValue);
  Serial.println(servoPosition);
  unsigned long currentTime = millis();


  if (soundValue > soundThreshold && (currentTime - lastDebounceTime) > debounceDelay) {
    soundDetected = true;
    lastDebounceTime = currentTime;
  }

  if (soundDetected) {
    soundDetected = false; 

    if (i % 2 == 0) {
      openCradle();
    } else {
      closeCradle();
    }

    i++;
  }
}

void openCradle() {
  for (servoPosition = 0; servoPosition <= 90; servoPosition++) {
    servoMotor.write(servoPosition);
    delay(15);
  }
}

void closeCradle() {
  for (servoPosition = 90; servoPosition >= 0; servoPosition--) {
    servoMotor.write(servoPosition);
    delay(15);
  }
}
