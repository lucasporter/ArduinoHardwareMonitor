#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// button pin
const int switchPin = 6;

// button state tracking
int switchState = HIGH;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // ms

// metric tracking
int metricIndex = 0; // 0 = CPU, 1 = GPU, 2 = CASE

// sensor readings
float cpuTemp = 0.0;
float gpuTemp = 0.0;
float caseTemp = 0.0;

// min/max tracking
float cpuMax = 0.0;
float gpuMax = 0.0;
float caseMax = 0.0;

// incoming serial buffer
String incomingData = "";

// timers
unsigned long lastUpdateTime = 0;
unsigned long lastAutoScrollTime = 0;
const unsigned long updateInterval = 1000; // 1 second
const unsigned long autoScrollInterval = 15000; // 15 seconds

void setup() {
  lcd.begin(16, 2);
  pinMode(switchPin, INPUT_PULLUP);
  Serial.begin(9600);

  lcd.print("Hardware Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // 1. Handle incoming Serial
  while (Serial.available()) {
    char incomingChar = Serial.read();
    if (incomingChar == '\n') {
      parseSensorData(incomingData);
      incomingData = "";
    } else {
      incomingData += incomingChar;
    }
  }

  // 2. Read the button (debounced)
  int reading = digitalRead(switchPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // reset debounce timer
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && switchState == HIGH) {
      // button was just pressed
      metricIndex++;
      if (metricIndex > 2) {
        metricIndex = 0;
      }
      lcd.clear();
      lastAutoScrollTime = millis(); // reset auto scroll when user presses button
    }
    switchState = reading;
  }

  lastButtonState = reading;

  // 3. Auto scroll if no button pressed
  if (millis() - lastAutoScrollTime >= autoScrollInterval) {
    metricIndex++;
    if (metricIndex > 2) {
      metricIndex = 0;
    }
    lcd.clear();
    lastAutoScrollTime = millis();
  }

  // 4. Update LCD every second
  if (millis() - lastUpdateTime >= updateInterval) {
    updateDisplay();
    lastUpdateTime = millis();
  }
}

void parseSensorData(String data) {
  int cpuIndex = data.indexOf("CPU:");
  int gpuIndex = data.indexOf("GPU:");
  int caseIndex = data.indexOf("CASE:");

  if (cpuIndex != -1 && gpuIndex != -1 && caseIndex != -1) {
    cpuTemp = data.substring(cpuIndex + 4, gpuIndex - 1).toFloat();
    gpuTemp = data.substring(gpuIndex + 4, caseIndex - 1).toFloat();
    caseTemp = data.substring(caseIndex + 5).toFloat();

    // Track maximums
    if (cpuTemp > cpuMax) cpuMax = cpuTemp;
    if (gpuTemp > gpuMax) gpuMax = gpuTemp;
    if (caseTemp > caseMax) caseMax = caseTemp;
  }
}

void updateDisplay() {
  lcd.setCursor(0, 0);
  switch (metricIndex) {
    case 0:
      lcd.print("CPU:");
      lcd.print(cpuTemp, 2);
      lcd.print((char)223);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Max:");
      lcd.print(cpuMax, 2);
      lcd.print((char)223);
      lcd.print("C");
      break;
    case 1:
      lcd.print("GPU:");
      lcd.print(gpuTemp, 2);
      lcd.print((char)223);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Max:");
      lcd.print(gpuMax, 2);
      lcd.print((char)223);
      lcd.print("C");
      break;
    case 2:
      lcd.print("Case Air:");
      lcd.print(caseTemp, 2);
      lcd.print((char)223);
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("Max:");
      lcd.print(caseMax, 2);
      lcd.print((char)223);
      lcd.print("C");
      break;
  }
}