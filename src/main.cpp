#include <Arduino.h>
#include "esp32platform_eeprom.h"

void setup() {
    Serial.begin(115200);
    Serial.println();

    startEeprom();
    String SSIDFromEeprom = SSIDFromEeprom();
    String PwdFromEeprom = PwdFromEeprom();

    
}

void loop() {
  // put your main code here, to run repeatedly:
}