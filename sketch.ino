//Embedded Systems 
//FAct2
//Kriztoffer Ryan Hernandez
//Lanze Ruzzel Amil

#include <math.h>

#define POT_PIN 34
#define LDR_PIN 35
#define NTC_PIN 32
#define LED_PIN 16

const int freq = 5000;
const int resolution = 8;

// ===== READ AVERAGE (Prevents simulation jitter) =====
int readAvg(int pin) {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(2); // Short delay for ADC stability
  }
  return sum / 10;
}

// ===== WOKWI-ACCURATE NTC TEMPERATURE CALCULATION =====
float getTempC(int adc) {
  if (adc == 0) return -273.15; // Prevent division by zero
  
  // Wokwi's default NTC attributes: Beta = 3950, R25 = 10000
  const float BETA = 3950;             
  const float ROOM_TEMP = 298.15;      // 25 °C in Kelvin
  const float RES_AT_ROOM_TEMP = 10000.0;
  
  // Calculate resistance based on Wokwi's standard VCC pull-up configuration
  float resistance = RES_AT_ROOM_TEMP / (4095.0 / (float)adc - 1.0);
  
  // Steinhart-Hart / Beta Equation
  float steinhart;
  steinhart = resistance / RES_AT_ROOM_TEMP;     // (R/Ro)
  steinhart = log(steinhart);                    // ln(R/Ro)
  steinhart /= BETA;                             // 1/B * ln(R/Ro)
  steinhart += 1.0 / ROOM_TEMP;                  // + (1/To)
  steinhart = 1.0 / steinhart;                   // Invert
  steinhart -= 273.15;                           // Convert Kelvin to Celsius
  
  return steinhart;
}

void setup() {
  Serial.begin(115200);

  // Configure ESP32 ADC attenuation (0-3.3V range)
  analogSetAttenuation(ADC_11db);

  // ESP32 Web IDE / Wokwi compatible PWM Setup
  ledcAttach(LED_PIN, freq, resolution);
}

void loop() {

  // ================= A1: POTENTIOMETER =================
  int pot = readAvg(POT_PIN);
  int potBrightness = map(pot, 0, 4095, 0, 255);

  // ================= A2: LDR (Photoresistor) =================
  int ldr = readAvg(LDR_PIN);
  int finalBrightness = 0;

  // Wokwi LDR Behavior: 
  // High Lux (Bright) = Low ADC value (~140)
  // Low Lux (Dark) = High ADC value (~4000)
  // If it's dark (ADC > 2000), let the Potentiometer control the LED.
  if (ldr > 1500) {
    finalBrightness = potBrightness;   // Dark -> Controlled by Potentiometer
  } else {
    finalBrightness = 0;               // Bright -> Forced OFF
  }

  // Write duty cycle to LED (0 to 255)
  ledcWrite(LED_PIN, finalBrightness);

  // ================= A3: NTC TEMP =================
  int ntc = readAvg(NTC_PIN);
  float tempC = getTempC(ntc);

  // ================= SERIAL OUTPUT =================
  Serial.print("Pot: ");
  Serial.print(pot);
  Serial.print(" (PWM: ");
  Serial.print(potBrightness);
  Serial.print(")");

  Serial.print(" | LDR: ");
  Serial.print(ldr);

  Serial.print(" | Temp: ");
  Serial.print(tempC, 1); // 1 Decimal place tracking
  Serial.print(" °C");

  Serial.print(" | LED Out: ");
  Serial.println(finalBrightness);

  delay(500); // 500ms update rate for readable serial data
}
