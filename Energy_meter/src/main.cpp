#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SENSOR_PIN 34
#define SAMPLES 1000
#define ADC_MAX 4095
#define ADC_VREF 3.3
#define CALIBRATION_FACTOR 0.0083 // Adjust based on your sensor and setup

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("AC Voltage Monitor"));
  display.display();
  delay(2000);
}

float readACVoltage() {
  float sumSquared = 0;
  int maxVal = 0;
  int minVal = ADC_MAX;
  
  // Take multiple samples over one AC cycle (50Hz = 20ms)
  for(int i = 0; i < SAMPLES; i++) {
    int sensorValue = analogRead(SENSOR_PIN);
    sumSquared += pow(sensorValue - (ADC_MAX/2), 2);
    if(sensorValue > maxVal) maxVal = sensorValue;
    if(sensorValue < minVal) minVal = sensorValue;
    delayMicroseconds(40); // Sample over ~20ms for 50Hz
  }
  
  // Calculate RMS voltage
  float meanSquared = sumSquared / SAMPLES;
  float rmsVoltage = sqrt(meanSquared) * ADC_VREF / (ADC_MAX/2);
  
  // Apply calibration factor and convert to AC voltage
  rmsVoltage = rmsVoltage / CALIBRATION_FACTOR;
  
  return rmsVoltage;
}

void loop() {
  float voltage = readACVoltage();
  
  // Display voltage on OLED
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print(F("AC Voltage:"));
  display.setCursor(0,20);
  display.print(voltage, 1);
  display.print(F(" V"));
  display.display();
  
  // Print to Serial for debugging
  Serial.print("AC Voltage: ");
  Serial.print(voltage, 1);
  Serial.println(" V");
  
  delay(500);
}