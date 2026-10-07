#include "pico/time.h"

const int LED = LED_BUILTIN;

repeating_timer_t timer;

bool timerCallback(repeating_timer_t *rt) //vai chamar esta função de 500ms em 500ms
{
  digitalWrite(LED, !digitalRead(LED));

  return true;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  Serial.begin(115200);

  delay(3000);
  Serial.println("Starting demo program: no interrupt");

  add_repeating_timer_ms(500, timerCallback, NULL, &timer); 
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // simulate other operations
  Serial.println("Simulate other work using delay(3000), i.e., 3000ms");
  delay(3000);
} // main loop