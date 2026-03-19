#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 
#define SCREEN_ADDRESS 0x3C

#define DHTPIN 2     
#define DHTTYPE DHT11


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);


  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }

 
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
}
void loop() {
  delay(2000); 

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  unsigned long allSeconds = millis() / 1000;
  int runDays = allSeconds / 86400;
  int secsRemaining = allSeconds % 86400;
  int runHours = secsRemaining / 3600;
  secsRemaining %= 3600;
  int runMinutes = secsRemaining / 60;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("ENVIRONMENT MONITOR"));
  
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print(F("Temp: ")); 
  display.print(t, 1); 
  display.print(F(" C"));

  display.setCursor(0, 28);
  display.print(F("Humid:")); 
  display.print(h, 0); 
  display.print(F(" %"));

  
  display.drawLine(0, 40, 128, 40, SSD1306_WHITE);

  display.setCursor(0, 44);
  display.print(F("System Uptime:"));

  display.setTextSize(1); 
  display.setCursor(0, 56);
  
  display.print(runDays);
  display.print(F("d "));
  
  display.print(runHours);
  display.print(F("h "));
  
  display.print(runMinutes);
  display.print(F("m"));

  display.display();
}