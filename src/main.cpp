#include <Arduino.h>

// put function declarations here:

const int potPin = A0; //green
const int ledPin = 11; //red
const int resetPin = 9; //orange
const int pausePin = 6; //yellow
const int modePin = 2; //blue

bool pause = false;
bool mode = false; // false = light control | true =  timer control

long seconds = 0;
volatile bool printSeconds = false;

bool lastModeState = LOW;
bool lastPauseState = HIGH;
bool lastResetState = HIGH;
int timer = 10;
int brightness = 100;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(resetPin, INPUT_PULLUP); // Reset button
  pinMode(pausePin, INPUT_PULLUP); // Start button
  pinMode(modePin, INPUT_PULLUP); // Mode button

  TCCR1A = 0;
  TCCR1B = 0;

  TCCR1B |= (1 << WGM12); // Set CTC mode

  OCR1A = 15624;

  TCCR1B |= (1 << CS12)|(1 << CS10); // Prescaler 256


  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt

  sei();
  

}

void loop() {
  int potValue = analogRead(potPin);

  // Mode button edge detection
  bool currentModeState = digitalRead(modePin);
  if (currentModeState == HIGH && lastModeState == LOW) {
    mode = !mode;
    delay(50); // debounce
  }
  lastModeState = currentModeState;

  // Pause button edge detection
  bool currentPauseState = digitalRead(pausePin);
  if (currentPauseState == LOW && lastPauseState == HIGH) {
    pause = !pause;
    delay(50); // debounce
  }
  lastPauseState = currentPauseState;

  // Reset button edge detection
  bool currentResetState = digitalRead(resetPin);
  if (currentResetState == LOW && lastResetState == HIGH) {
    seconds = 0;
    pause = true;
    delay(50); // debounce
  }
  lastResetState = currentResetState;

  // Always set timer
  if (mode) {
    timer = map(potValue, 0, 1023, 60, 20);
    
  } else {
    brightness = map(potValue, 0, 1023, 0, 100);
    
    analogWrite(ledPin, brightness);
  }

  if (mode) {
    if (seconds >= timer) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }

  if (printSeconds) {
    Serial.print("Seconds: ");
    
    Serial.print("Timer set to: ");
    Serial.println(timer);
    
    Serial.print("Brightness set to: ");
    Serial.println(brightness);
    
    Serial.println(seconds);
    printSeconds = false;
  }
}

ISR(TIMER1_COMPA_vect) {
  if (!pause) {
    seconds++;
    printSeconds = true;
  }
}