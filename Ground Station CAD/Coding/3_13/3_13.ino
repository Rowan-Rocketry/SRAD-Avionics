#include "pitches.h"
#define BUZZER_PIN 6
const int trig_pin = 10;
const int echo_pin = 11;
const int motorPin = 9;
const int ledPin = 8;
const int ledPin2 = 7;
int buzzer = 6;
float timing = 0.0;
float distance = 0.0;
int melody[] = {
  NOTE_B4, NOTE_B5, NOTE_FS5, NOTE_DS5,
  NOTE_B5, NOTE_FS5, NOTE_DS5, NOTE_C5,
  NOTE_C6, NOTE_G6, NOTE_E6, NOTE_C6, NOTE_G6, NOTE_E6,
  
  NOTE_B4, NOTE_B5, NOTE_FS5, NOTE_DS5, NOTE_B5,
  NOTE_FS5, NOTE_DS5, NOTE_DS5, NOTE_E5, NOTE_F5,
  NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_B5
};

int durations[] = {
  16, 16, 16, 16,
  32, 16, 8, 16,
  16, 16, 16, 32, 16, 8,
  
  16, 16, 16, 16, 32,
  16, 8, 32, 32, 32,
  32, 32, 32, 32, 32, 16, 8
};

void setup()
{
  pinMode(motorPin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(trig_pin, LOW);
    
  Serial.begin(9600);
}

void loop()
{
  digitalWrite(trig_pin, LOW);
  delay(2);
  
  digitalWrite(trig_pin, HIGH);
  delay(10);
  digitalWrite(trig_pin, LOW);
  
  timing = pulseIn(echo_pin, HIGH);
  distance = (timing * 0.034) / 2;
  
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print("cm | ");
  Serial.print(distance / 2.54);
  Serial.println("in");
  
  if (distance <= 20) {
    digitalWrite(motorPin, HIGH);
    
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, HIGH);

    int size = sizeof(durations) / sizeof(int);

    for (int note = 0; note < size; note++) {
      // Recalculate distance while playing the song
      digitalWrite(trig_pin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig_pin, LOW);
      timing = pulseIn(echo_pin, HIGH);
      distance = (timing * 0.034) / 2;
      
      if (distance > 20) {
        noTone(BUZZER_PIN);
        break; // Stop playing the song immediately
      }

      int duration = 1000 / durations[note];
      tone(BUZZER_PIN, melody[note], duration);

      int pauseBetweenNotes = duration * 1.30;
      delay(pauseBetweenNotes);
      
      noTone(BUZZER_PIN);
    }
    
  } else {
    digitalWrite(motorPin, LOW);
    noTone(BUZZER_PIN);
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, LOW);
  }
  
  delay(100);
}
