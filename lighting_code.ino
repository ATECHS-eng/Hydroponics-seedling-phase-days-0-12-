#include <Wire.h>
#include <RTClib.h>
#include <EEPROM.h>

const int ON_DURATION_HOURS = 16; 

const int RELAY_ON_LEVEL  = LOW;  
const int RELAY_OFF_LEVEL = HIGH; 

const int RELAY_PIN  = 9; 
const int BUTTON_PIN = 3; 

const int ADDR_ACTIVE     = 0; 
const int ADDR_START_HOUR = 1; 
const int ADDR_START_MIN  = 2; 

RTC_DS1307 rtc;

bool isSystemActive = false;
int startHour = 0;
int startMinute = 0;
unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(9600);
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, RELAY_OFF_LEVEL);
    pinMode(BUTTON_PIN, INPUT_PULLUP); 
    
    // Start the RTC
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC! Check your SDA (A4) and SCL (A5) wiring.");
        Serial.flush();
        while (1) delay(10);
    }
    if (!rtc.isrunning()) {
        Serial.println("RTC Battery Dead! Resetting time to computer time...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    if (EEPROM.read(ADDR_ACTIVE) == 1) {
        isSystemActive = true;
        startHour = EEPROM.read(ADDR_START_HOUR);
        startMinute = EEPROM.read(ADDR_START_MIN);
        Serial.println(">>> POWER RESTORED: Resuming Cycle <<<");
    } else {
        Serial.println(">>> STANDBY: Press Button (D3) to Start <<<");
    }
}

void loop() {
    DateTime now = rtc.now();
    if (digitalRead(BUTTON_PIN) == LOW) {
        delay(50); 
        if (digitalRead(BUTTON_PIN) == LOW) {

            startHour = now.hour();
            startMinute = now.minute();
            isSystemActive = true;

            EEPROM.write(ADDR_ACTIVE, 1);
            EEPROM.write(ADDR_START_HOUR, startHour);
            EEPROM.write(ADDR_START_MIN, startMinute);

            Serial.println("\n>>> NEW 16/8 CYCLE STARTED! <<<");

            for(int i=0; i<3; i++) {
                digitalWrite(RELAY_PIN, RELAY_ON_LEVEL); delay(100);
                digitalWrite(RELAY_PIN, RELAY_OFF_LEVEL); delay(100);
            }
        }
        while(digitalRead(BUTTON_PIN) == LOW);
    }

    if (isSystemActive) {
        long currentMins = (long)now.hour() * 60 + now.minute();
        long startMins   = (long)startHour * 60 + startMinute;
        long diff = currentMins - startMins;
        if (diff < 0) diff += 1440; 
        bool shouldBeOn = (diff < (ON_DURATION_HOURS * 60));
        
        if (shouldBeOn) {
            digitalWrite(RELAY_PIN, RELAY_ON_LEVEL);
        } else {
            digitalWrite(RELAY_PIN, RELAY_OFF_LEVEL);
        }
        if (millis() - lastUpdate > 2000) {
            lastUpdate = millis();
            printStatus(now, shouldBeOn, diff);
        }
    }
    
    delay(100);
}
void printStatus(DateTime now, bool on, long minsPassed) {
    Serial.print("[");
    if(now.hour() < 10) Serial.print("0"); Serial.print(now.hour()); Serial.print(":");
    if(now.minute() < 10) Serial.print("0"); Serial.print(now.minute()); Serial.print(":");
    if(now.second() < 10) Serial.print("0"); Serial.print(now.second()); 
    Serial.print("] ");

    if (on) {
        Serial.print("LIGHTS: ON  | Time ON: ");
        Serial.print(minsPassed / 60); Serial.print("h ");
        Serial.print(minsPassed % 60); Serial.println("m");
    } else {
        Serial.println("LIGHTS: OFF | Resting...");
    }
}