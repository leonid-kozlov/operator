#define BTN_PIN 3
#define LED_PIN 13
#define LED_RED_PIN 8
#define LED_GREEN_PIN 10
#define RELAY_PIN 11

void setup() {
  pinMode(BTN_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
  uint8_t state;
  state = digitalRead(BTN_PIN);
  if (state == HIGH) {
    uint8_t led_state = digitalRead(LED_PIN);
    digitalWrite(LED_PIN, !led_state);
    if (led_state) {
      digitalWrite(RELAY_PIN, HIGH);
      digitalWrite(LED_GREEN_PIN, HIGH);
      digitalWrite(LED_RED_PIN, LOW);
    } else {
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_GREEN_PIN, LOW);
      digitalWrite(LED_RED_PIN, HIGH);
    }
    delay(200);
  }
  delay(50);
}
