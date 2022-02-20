#include <Arduino.h>
#include "esp32platform_eeprom.h"
#include "esp32platform_ota.h"

void setup() {
    Serial.begin(115200);
    Serial.println();

    startEeprom();
    startOTA();

    String savedSSID = SSIDFromEeprom();
    String savedPASS = PwdFromEeprom();

}

void loop() {
  // put your main code here, to run repeatedly:
}