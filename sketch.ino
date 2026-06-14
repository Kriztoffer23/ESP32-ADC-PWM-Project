// Pins
#define POT_PIN 34
#define LDR_PIN 32
#define NTC_PIN 33
#define LED_PIN 25

const int freq = 5000;
const int resolution = 8;

void setup() {
  Serial.begin(115200);

  // NEW ESP32 PWM
  ledcAttach(LED_PIN, freq, resolution);
}

void loop() {

  int potValue = analogRead(POT_PIN);
  int ldrValue = analogRead(LDR_PIN);
  int ntcValue = analogRead(NTC_PIN);

  int brightness = map(potValue, 0, 4095, 0, 255);

  ledcWrite(LED_PIN, brightness);

  Serial.print("Pot: ");
  Serial.print(potValue);

  Serial.print(" | LDR: ");
  Serial.print(ldrValue);

  Serial.print(" | NTC: ");
  Serial.print(ntcValue);

  Serial.print(" | LED: ");
  Serial.println(brightness);

  delay(500);
}
