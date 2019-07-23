#include <Arduino.h>
#include <FastLED.h>
#include <M5Stack.h>
#include "DHT12.h"
#include <Wire.h>     //The DHT12 uses I2C comunication.

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRac.h>
#include <IRsend.h>
#include <IRutils.h>

#define LED_PIN 15
#define NUM_LEDS 12
#define COLOR_ORDER GRB         // RGB Mask color order (Green/Red/Blue)
#define CHIPSET     SK6812

#define I2S_CLK 0
#define I2S_WS 13
#define I2S_BCK 5
#define I2S_IN 2
#define I2S_OUT 2

#define IR_RECEIVE  35
#define IR_SEND     12

#define hueTime 30
#define senseTime 10000

unsigned long currentMillis; // store current loop's millis value
unsigned long hueMillis; // store time of last hue change
unsigned long senseMillis; // store time of last sensor reading
byte cycleHue = 0;
byte cycleHueCount;

const uint16_t kCaptureBufferSize = 1024;
const uint8_t kTimeout = 15;
const uint16_t kMinUnknownSize = 12;

CRGB leds[NUM_LEDS];
DHT12 dht12;

IRrecv irrecv(IR_RECEIVE, kCaptureBufferSize, kTimeout, true);
decode_results results;

IRsend irsend(IR_SEND);

uint16_t rawData[67] = {9000, 4500, 650, 550, 650, 1650, 600, 550, 650, 550,
                        600, 1650, 650, 550, 600, 1650, 650, 1650, 650, 1650,
                        600, 550, 650, 1650, 650, 1650, 650, 550, 600, 1650,
                        650, 1650, 650, 550, 650, 550, 650, 1650, 650, 550,
                        650, 550, 650, 550, 600, 550, 650, 550, 650, 550,
                        650, 1650, 600, 550, 650, 1650, 650, 1650, 650, 1650,
                        650, 1650, 650, 1650, 650, 1650, 600};

void setup() {
  M5.begin();
  Wire.begin(21, 22);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  irrecv.enableIRIn();
  irsend.begin();
}

void loop() {
  currentMillis = millis();

  if (currentMillis - hueMillis > hueTime) {
    hueMillis = currentMillis;
    hueCycle(1); // increment the global hue value
  }
  if (currentMillis - senseMillis > senseTime) {
    senseMillis = currentMillis;
    readSensors();
    irsend.sendRaw(rawData, 67, 38); // Send a raw data at 38kHz.
  }

  fill_rainbow( leds, NUM_LEDS, cycleHue, 255/NUM_LEDS );

  if (irrecv.decode(&results)) {
      // Display a crude timestamp.
      uint32_t now = millis();
      Serial.printf("Timestamp : %06u.%03u\n", now / 1000, now % 1000);
      // Display the basic output of what we found.
      Serial.print(resultToHumanReadableBasic(&results));
      // Output the results as source code
      Serial.println(resultToSourceCode(&results));
      Serial.println();    // Blank line between entries

  }

  M5.update();
  FastLED.show();
}








void hueCycle(byte incr) {
    cycleHueCount = 0;
    cycleHue+=incr;
}

void readSensors(){
  //Read temperature with preset scale.
  Serial.print("Temperature: ");
  M5.Lcd.print("Temperature: ");
  Serial.print(dht12.readTemperature());
  M5.Lcd.print(dht12.readTemperature());

  //Read humidity.
  Serial.print("*C  Humidity: ");
  M5.Lcd.print("*C  Humidity: ");
  Serial.println(dht12.readHumidity());
  M5.Lcd.println(dht12.readHumidity());

}
