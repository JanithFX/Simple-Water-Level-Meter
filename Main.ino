#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int level1 = A0; // Bottom (20%)
const int level2 = A1; // 40%
const int level3 = A2; // 60%
const int level4 = A3; // 80%
const int level5 = 10; // Top (100%)
const int relay = 6; // Pin connected to the relay
const int ct = 9; // PWM pin for controlling brightness
const int z = 30; // Adjust based on sensor calibration

// Define the OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define the I2C address for the OLED display
#define OLED_ADDRESS 0x3C // Common address for many SSD1306 displays

// Create an instance of the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // -1 indicates no reset pin

// State variable for relay control
bool relayOn = false; // Track whether the relay is currently on

void setup() {
    pinMode(level1, INPUT);
    pinMode(level2, INPUT);
    pinMode(level3, INPUT);
    pinMode(level4, INPUT);
    pinMode(level5, INPUT);
    pinMode(relay, OUTPUT);
    analogWrite(ct, 50); // Set brightness if using PWM for LCD

    // Initialize Serial communication for debugging
    Serial.begin(9600);

    // Initialize the OLED display
    display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS); // Correct the call to display.begin
    display.clearDisplay();
    display.display();
}

void loop() {
    // Read sensor values
    int sensor1 = analogRead(level1); // Bottom sensor (20%)
    int sensor2 = analogRead(level2); // 40% sensor
    int sensor3 = analogRead(level3); // 60% sensor
    int sensor4 = analogRead(level4); // 80% sensor
    int sensor5 = analogRead(level5); // Top sensor (100%)

    // Water level variable
    int waterLevel = 0;

    // Print sensor values to Serial Monitor
    Serial.print("Sensor1 (20%): "); Serial.print(sensor1);
    Serial.print(" | Sensor2 (40%): "); Serial.print(sensor2);
    Serial.print(" | Sensor3 (60%): "); Serial.print(sensor3);
    Serial.print(" | Sensor4 (80%): "); Serial.print(sensor4);
    Serial.print(" | Sensor5 (100%): "); Serial.println(sensor5);

    // Determine water level based on sensor readings
    if (sensor5 > z) {
        waterLevel = 100; // Top sensor, tank is full
    } else if (sensor4 > z) {
        waterLevel = 80; // 80% full
    } else if (sensor3 > z) {
        waterLevel = 60; // 60% full
    } else if (sensor2 > z) {
        waterLevel = 40; // 40% full
    } else if (sensor1 > z) {
        waterLevel = 20; // 20% full
    } else {
        waterLevel = 0; // Empty
    }

    // Relay control logic
    if (waterLevel == 0 && !relayOn) {
        // Turn on the relay (and thus the pump) when the tank is empty
        relayOn = true;
        digitalWrite(relay, HIGH); // Activate relay
    } else if (waterLevel == 100 && relayOn) {
        // Turn off the relay when the tank is full
        relayOn = false;
        digitalWrite(relay, LOW); // Deactivate relay
    }

    // Display water level on the OLED
    display.clearDisplay();
    display.setTextSize(1.5);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("Water Level Meter");
    display.setCursor(0, 20);

    if (waterLevel == 100) {
        display.print("Tank: FULL ");
    } else if (waterLevel == 80) {
        display.print("Tank: 80%  ");
    } else if (waterLevel == 60) {
        display.print("Tank: 60%  ");
    } else if (waterLevel == 40) {
        display.print("Tank: 40%  ");
    } else if (waterLevel == 20) {
        display.print("Tank: 20%  ");
    } else {
        display.print("Tank: EMPTY");
    }

    // Display relay status on the OLED
    display.setCursor(0, 40);
    display.print("Pump: ");
    display.print(relayOn ? "ON     " : "OFF    ");

    // Refresh the display
    display.display();

    // Delay to slow down updates
    delay(1000);
}
