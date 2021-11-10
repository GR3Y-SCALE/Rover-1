#include <PinChangeInterrupt.h>

/* This sketch was made by Gr3y, feel free to use this sketch for any applicable project
 * this sketch uses pins 2, 7, 8 for signal in from a suitable RC receiver and interprets the signal for use with a H-Bridge motor controller.
 * chhannel length is the signals position, i.e 1500 is half way and 2000 is full forward.
 * sketch also uses pwm to control the speed of the motors and the use of a failsafe to stop accidental activation of motors
 *  please message me @ gr3yscl@protonmail.com for any questions about the project or even your own adaptations
 */
const byte channel_pin[] = {2, 7, 8};
volatile unsigned long rising_start[] = {0, 0, 0};
volatile long channel_length[] = {0, 0, 0};
const int motor1 = 6; // motor direction pins
const int motor2 = 5; // motor direction pins
const int enable_m1 = 11; // motor enable pins
const int enable_m2 = 10; // motor enable pins
int motor1VAL = 0;
int motor2VAL =0;
void setup() {
  Serial.begin(57600);

  pinMode(channel_pin[0], INPUT);
  pinMode(channel_pin[1], INPUT);
  pinMode(channel_pin[2], INPUT);
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(enable_m1, OUTPUT);
  pinMode(enable_m2, OUTPUT);

  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(channel_pin[0]), onRising0, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(channel_pin[1]), onRising1, CHANGE);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(channel_pin[2]), onRising2, CHANGE);
}

void processPin(byte pin) {
  uint8_t trigger = getPinChangeInterruptTrigger(digitalPinToPCINT(channel_pin[pin]));

  if(trigger == RISING) {
    rising_start[pin] = micros();
  } else if(trigger == FALLING) {
    channel_length[pin] = micros() - rising_start[pin];
  }
}

void onRising0(void) {
  processPin(0);
}

void onRising1(void) {
  processPin(1);
}

void onRising2(void) {
  processPin(2);
}

void loop() {
  Serial.print(channel_length[0]);
  Serial.print(" | ");
  Serial.print(channel_length[1]);
  Serial.print(" | ");
  Serial.print(channel_length[2]);
  
  Serial.println("");
  delay(100);

   if (channel_length[2] > 1500) { //failsafe, must be below the value in order for the motors to opperate
    
      if (channel_length[1] >= 1600) { // if the channel position is above middle
      digitalWrite(motor1, HIGH); //motor pin low means motor direction is forward
      motor1VAL = map(channel_length[1], 1600, 2020, 0, 255); //maps the input from the appropiate channel to enable variable speed
      analogWrite(enable_m1, motor1VAL); // writes the speed value (pwm) to the motor enable pin
      
      }else if (channel_length[1] <= 1400) {
      digitalWrite(motor1, LOW);
      motor1VAL = map(channel_length[1], 1400, 950, 0, 255);
      analogWrite(enable_m1, motor1VAL);
      
      }else { // there is a 200 position dead zone for the middle channel position
      digitalWrite(enable_m1, LOW);
      digitalWrite(motor1, LOW);
      motor1VAL = 0;
    }
    
  //motor 2
    
    if (channel_length[0] >= 1600) {
      digitalWrite(motor2, HIGH); //motor pin is high so the motor direction is reverse
      motor2VAL = map(channel_length[0], 1600, 2020, 0, 255);
      analogWrite(enable_m2, motor2VAL);
      
      }else if (channel_length[0] <= 1400) {
      digitalWrite(motor2, LOW);
      motor2VAL = map(channel_length[0], 1400, 950, 0, 255);
      analogWrite(enable_m2, motor2VAL);
      
      }else {
      digitalWrite(enable_m2, LOW);
      digitalWrite(motor2, LOW);
      motor2VAL = 0;
    }
  //failsafe not below certain value
   }else {
    motor1VAL = 0;
    motor2VAL = 0;
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
    digitalWrite(enable_m2, LOW);
    digitalWrite(enable_m1, LOW);
   }

 
}
