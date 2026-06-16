#define POT_PIN 34
#define LDR_PIN 35
#define NTC_PIN 32
#define LED_PIN 16

const int freq = 5000;
const int resolution = 8;

// ===== READ AVERAGE (stability) =====
int readAvg(int pin) {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  return sum / 10;
}

// ===== SIMPLE TEMP MAPPING (Wokwi-safe) =====
float getTempC(int adc) {
  float temp = map(adc, 0, 4095, 0, 100);
  return temp;
}

void setup() {
  Serial.begin(115200);

  analogSetAttenuation(ADC_11db);

  // NEW ESP32 PWM API
  ledcAttach(LED_PIN, freq, resolution);
}

void loop() {

  // ================= A1: POTENTIOMETER =================
  int pot = readAvg(POT_PIN);
  int potBrightness = map(pot, 0, 4095, 0, 255);

  // ================= A2: LDR =================
  int ldr = readAvg(LDR_PIN);
  int ldrBrightness;

  if (ldr < 2000) {
    ldrBrightness = 255;   // dark → ON
  } else {
    ldrBrightness = 0;     // bright → OFF
  }

  // combine both controls (blend behavior)
  int finalBrightness = (potBrightness + ldrBrightness) / 2;
  finalBrightness = constrain(finalBrightness, 0, 255);

  ledcWrite(LED_PIN, finalBrightness);

  // ================= A3: NTC TEMP =================
  int ntc = readAvg(NTC_PIN);
  float tempC = getTempC(ntc);

  // ================= SERIAL OUTPUT =================
  Serial.print("Pot: ");
  Serial.print(pot);

  Serial.print(" | LDR: ");
  Serial.print(ldr);

  Serial.print(" | Temp: ");
  Serial.print(tempC);
  Serial.print(" °C");

  Serial.print(" | LED: ");
  Serial.println(finalBrightness);

  delay(500);
}
