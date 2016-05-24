#include <SoftwareSerial.h>
#include <Servo.h>

// Pin assignments for the Arduino, change this if you want to move the pins
const int txPin = 12; //SoftwareSerial TX pin, connect to JY-MCU RX pin
const int rxPin = 13; //SoftwareSerial RX pin, connect to JY-MCY TX pin
SoftwareSerial bluetoothInput(rxPin, txPin); // RX, TX

// Pin assignment and servo declarations
Servo leftServo;
Servo rightServo;
int leftServoPin = 9;
int rightServoPin = 8;

// LED pin assignments
int ledEyeRight = 11;  // LED eye (right)
int ledEyeLeft = 10;   // LED eye (left)
int ledFrontRight = 5; // LED front (right)
int ledFrontLeft = 6;  // LED front (left)
int ledBackRight = 3;  // LED back (right)
int ledBackLeft = 4;   // LED back (left)

//variables for blink and noMoreIncoming functions that require time keeping
unsigned long currentTime = 0;
unsigned long timeSinceLastInput = 0;

//blinkFunc() variables
const long interval = 300;    // the interval between blinks 
unsigned long blinkTimer = 0; // keeps track of the last time it blinked
int blinkType = 0;           // Variable to determine various blinking protocols
int blinkState = 0;            // 0 if lights are on, 1 if lights are off

int reset = 0;                // used by the arduino to determine if the IamSAM app has stopped sending input

byte state[] = {90, 90};      //initializes the state variable that stores incoming bluetooth signals

void setup() {
  bluetoothInput.begin(9600); // Sets rate of data transmission of Bluetooth module to 9600 bits/second. See https://www.arduino.cc/en/serial/begin for more details
  Serial.begin(9600);         // Used to debug when plugged into computer via USB 
  //Initializing components
  leftServo.attach(leftServoPin); 
  rightServo.attach(rightServoPin);
  pinMode(ledEyeRight, OUTPUT);
  pinMode(ledEyeLeft, OUTPUT);
  pinMode(ledFrontRight, OUTPUT); 
  pinMode(ledFrontLeft, OUTPUT);
  pinMode(ledBackRight, OUTPUT);
  pinMode(ledBackLeft, OUTPUT);
  //Turns on all LEDs
  digitalWrite(ledEyeRight, HIGH);
  digitalWrite(ledEyeLeft, HIGH);
  digitalWrite(ledFrontRight, HIGH);
  digitalWrite(ledFrontLeft, HIGH);
  digitalWrite(ledBackRight, HIGH);
  digitalWrite(ledBackLeft, HIGH);
 }
 
 
void loop() {
  //Checks to see if there is incoming data
  if (bluetoothInput.available() > 0) {
    // if there is incoming data, read the first number
    bluetoothInput.readBytes(state, 1);
    // output the bluetoothInput to serial for testing
    Serial.println(state[0]);
    //Checks to see where data is coming from and calls the corresponding code
    if (state[0] == 180){
      bluetoothInput.readBytes(state, 2);
      fromPhone(state[0], state[1]);
      // output the bluetoothInput to serial for testing
      Serial.print(state[0]);
      Serial.print("   ");
      Serial.println(state[1]);
    }
    else{
       fromComputer(state[0]);     
    }
  }

  // blinkFunc controls the blinking of the LED lights
  blinkFunc();

  // stops the vehicle if there is no new input after set time from the phone
  noMoreIncoming();
}

// blinkFunc controls the blinking of the LED by using different states. interval defines the blinking interval.
void blinkFunc(){
  unsigned long currentTime = millis();   // keeps track of the current time

  // the blinkType variable can be modified in the main control code, and the function runs a different protocol according to the given state:
  if(blinkType == 0){              // the default state, it turns all non-eye LEDs off
    digitalWrite(ledFrontRight, LOW);
    digitalWrite(ledFrontLeft, LOW);
    digitalWrite(ledBackRight, LOW);
    digitalWrite(ledBackLeft, LOW);
  }
  else if(blinkType == 1){         // blinks the front LEDs and turns the back LEDs off
      if(currentTime - blinkTimer > interval){  // checks to see if the inverval set has been exceeded 
        blinkTimer = currentTime;               // resets the timer
        if(blinkState){                          // blinkState remembers the previous state and sets the next state according to the user defined settings

          // in this example, blinkState starts at 1, and turns the front LEDs on, then the blinkstate is set to 0
          digitalWrite(ledFrontRight, HIGH);
          digitalWrite(ledFrontLeft, HIGH);
          digitalWrite(ledBackRight, LOW);
          digitalWrite(ledBackLeft, LOW);
          blinkState = 0;
        }else{
          // when the blinkState is set to 0, turn all LEDs off and change blinkState to 1
          digitalWrite(ledFrontRight, LOW);
          digitalWrite(ledFrontLeft, LOW);
          digitalWrite(ledBackRight, LOW);
          digitalWrite(ledBackLeft, LOW);
          blinkState = 1;
        }
      }
  }
  else if(blinkType == 2){                // blinks the back LEDs and turns the front LEDs off
    if(currentTime - blinkTimer > interval){
      blinkTimer = currentTime;
      if(blinkState){
        digitalWrite(ledBackRight, HIGH);
        digitalWrite(ledBackLeft, HIGH);
        digitalWrite(ledFrontRight, LOW);
        digitalWrite(ledFrontLeft, LOW);
        blinkState = 0;
      }else{
        digitalWrite(ledBackRight, LOW);
        digitalWrite(ledBackLeft, LOW);
        digitalWrite(ledFrontRight, LOW);
        digitalWrite(ledFrontLeft, LOW);
        blinkState = 1;
      }
    }
  }
  else if(blinkType == 3){               //turns all LEDs on
    digitalWrite(ledBackRight, HIGH);
    digitalWrite(ledBackLeft, HIGH);
    digitalWrite(ledFrontRight, HIGH);
    digitalWrite(ledFrontLeft, HIGH);    
  }

  //Add your own blink types here
}


//Used to interpret the data that is taken in as a letter
void fromComputer (int incomingByte) {
  if(incomingByte == 'w'){
    // if 'f', then go forward
      rightServo.write(0);
      leftServo.write(180);
      blinkType = 1;
    }

  else if(incomingByte == 's') {
    // if 'b', then go backward
      rightServo.write(180);
      leftServo.write(0);
      blinkType = 2;
  }
  
  else if(incomingByte == 'a') {
    // if 'l', then go left
      rightServo.write(0);
      leftServo.write(0);
}
  else if(incomingByte == 'd') {
    // if 'r', then go right
      rightServo.write(180);
      leftServo.write(180);
  }
  else if(incomingByte == 'x') {
    // if 'x', then stop
      rightServo.write(90);
      leftServo.write(90);
   }
   
   ////SLOOOOOOOOW
   else if(incomingByte == 'i'){
    // if 'i', then go forward
      rightServo.write(45);
      leftServo.write(135);
    }

  else if(incomingByte == 'k') {
    // if 'k', then go backward
      rightServo.write(135);
      leftServo.write(45);
  }
  
  else if(incomingByte == 'j') {
    // if 'j', then go left
      rightServo.write(80);
      leftServo.write(80);
}
  else if(incomingByte == 'l') {
    // if 'l', then go right
      rightServo.write(100);
      leftServo.write(100);
    }

    else if(incomingByte == ' ') {
    // if '/space/', then stop
      rightServo.write(90);
      leftServo.write(90);
      blinkType = 0;
  }
  
}

//Interprets data from the phonne app
void fromPhone (int leftMotor, int rightMotor) {
  //Sets the speed of the motor to the position of the sliders in the app
  leftServo.write(leftMotor);
  rightServo.write(180- rightMotor);
  blinkType = 3;
  
  //These are the variables for the noMoreIncoming function
   timeSinceLastInput = millis();
   reset = 0;
   return;
}


void noMoreIncoming(){
  currentTime = millis(); 
  // if no data has come in for 150 millis, disable movement from the phone
  if (reset == 0 && currentTime - timeSinceLastInput >= 150){
    leftServo.write(90);
    rightServo.write(90);
    reset = 1;
  }
}

