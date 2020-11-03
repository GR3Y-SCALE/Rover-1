#include <PinChangeInterrupt.h>

/*
 * Define pins used to provide RC PWM signal to Arduino
 * Pins 8, 9 and 10 are used since they work on both ATMega328 and 
 * ATMega32u4 board. So this code will work on Uno/Mini/Nano/Micro/Leonardo
 * See PinChangeInterrupt documentation for usable pins on other boards
 */
const byte channel_pin[] = {2, 7, 8};
volatile unsigned long rising_start[] = {0, 0, 0};
volatile long channel_length[] = {0, 0, 0};
const int motor1 = 6;
const int motor2 = 5;
const int enable_m1 = 11;
const int enable_m2 = 10;

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

  //motor 1
  
  if (channel_length[1] >= 1600) { 
    digitalWrite(enable_m1, HIGH); //enable pin low means motor direction is forward
    digitalWrite(motor1, HIGH);
    
  }else if (channel_length[1] <= 1400) {
    digitalWrite(enable_m1, HIGH);
    digitalWrite(motor1, LOW);
    
  }else {
    digitalWrite(enable_m1, LOW);
    digitalWrite(motor1, LOW);
  }
  
//motor 2
  
  if (channel_length[0] >= 1600) {
    digitalWrite(enable_m2, HIGH);
    digitalWrite(motor2, LOW);
    
  }else if (channel_length[0] <= 1400) {
    digitalWrite(enable_m2, HIGH);
    digitalWrite(motor2, HIGH);
    
  }else {
    digitalWrite(enable_m2, LOW);
    digitalWrite(motor2, LOW);
  }
}
