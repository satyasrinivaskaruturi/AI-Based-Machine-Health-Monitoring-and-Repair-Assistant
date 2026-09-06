#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// =====================================================
// AI-BASED MACHINE HEALTH MONITORING - ONLINE PROTOTYPE
// Reference: user's supplied ESP32 circuit diagram
//
// Physical sensors that are not available in Wokwi are
// represented by equivalent virtual inputs:
//   SW-420 vibration -> MPU6050 acceleration
//   ACS712 current   -> potentiometer
//   Sound sensor     -> potentiometer
// =====================================================

#define SDA_PIN       21
#define SCL_PIN       22
#define TEMP_PIN      33
#define CURRENT_PIN   34
#define NOISE_PIN     32

#define LED_PIN       19
#define BUZZER_PIN    18
#define RELAY_PIN     17

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

Adafruit_MPU6050 mpu;
OneWire oneWire(TEMP_PIN);
DallasTemperature tempSensor(&oneWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float temperatureC = 0.0;
float currentA = 0.0;
float noiseDb = 0.0;
float vibrationG = 0.0;

const float TEMP_LIMIT      = 75.0;
const float CURRENT_LIMIT   = 8.0;
const float NOISE_LIMIT     = 85.0;
const float VIBRATION_LIMIT = 1.50;

String statusText = "STARTING";

void drawDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("MACHINE HEALTH");
  display.println("----------------");

  display.print("Temp : ");
  display.print(temperatureC, 1);
  display.println(" C");

  display.print("Curr : ");
  display.print(currentA, 1);
  display.println(" A");

  display.print("Noise: ");
  display.print(noiseDb, 0);
  display.println(" dB");

  display.print("Vib  : ");
  display.print(vibrationG, 2);
  display.println(" g");

  display.print("STATUS: ");
  display.println(statusText);

  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed");
  }

  tempSensor.begin();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not detected.");
    while (true) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      delay(250);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("AI MACHINE HEALTH");
  display.println("ONLINE PROTOTYPE");
  display.println();
  display.println("ESP32 + SENSORS");
  display.display();

  delay(1500);
}

void loop() {
  // -------- 1. Temperature --------
  tempSensor.requestTemperatures();
  temperatureC = tempSensor.getTempCByIndex(0);

  // -------- 2. Vibration --------
  sensors_event_t a, g, imuTemp;
  mpu.getEvent(&a, &g, &imuTemp);

  float ax = a.acceleration.x / 9.80665;
  float ay = a.acceleration.y / 9.80665;
  float az = a.acceleration.z / 9.80665;

  // Acceleration magnitude used as a simple vibration indicator.
  vibrationG = sqrt(ax * ax + ay * ay + az * az);

  // -------- 3. Virtual ACS712 current sensor --------
  int currentRaw = analogRead(CURRENT_PIN);
  currentA = (currentRaw / 4095.0) * 10.0;

  // -------- 4. Virtual sound/noise sensor --------
  int noiseRaw = analogRead(NOISE_PIN);
  noiseDb = 30.0 + (noiseRaw / 4095.0) * 70.0;

  // -------- 5. Fault decision --------
  bool highTemp = temperatureC > TEMP_LIMIT;
  bool highCurrent = currentA > CURRENT_LIMIT;
  bool highNoise = noiseDb > NOISE_LIMIT;
  bool highVibration = vibrationG > VIBRATION_LIMIT;

  bool fault = highTemp || highCurrent || highNoise || highVibration;

  if (fault) {
    statusText = "FAULT";
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    statusText = "HEALTHY";
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
  }

  // -------- 6. Serial output --------
  Serial.println("================================");
  Serial.print("Temperature : ");
  Serial.print(temperatureC, 1);
  Serial.println(" C");

  Serial.print("Current     : ");
  Serial.print(currentA, 1);
  Serial.println(" A");

  Serial.print("Noise       : ");
  Serial.print(noiseDb, 0);
  Serial.println(" dB");

  Serial.print("Vibration   : ");
  Serial.print(vibrationG, 2);
  Serial.println(" g");

  Serial.print("STATUS      : ");
  Serial.println(statusText);

  if (highTemp)      Serial.println("FAULT: High temperature");
  if (highCurrent)   Serial.println("FAULT: High current");
  if (highNoise)     Serial.println("FAULT: High noise");
  if (highVibration) Serial.println("FAULT: High vibration");

  drawDisplay();
  delay(1000);
}
