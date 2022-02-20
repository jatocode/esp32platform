#include <EEPROM.h>

#define EEPROM_SIZE 300
#define EEPROM_SSID 0
#define EEPROM_PASS 100
#define EEPROM_CODE 200
#define MAX_EEPROM_LEN 50  // Max length to read ssid/passwd

void writeStringEEPROM(char add, String data) {
    int _size = data.length();
    Serial.println("Sparar");
    Serial.println(data);
    Serial.println(_size);
    if (_size > MAX_EEPROM_LEN) return;
    int i;
    for (i = 0; i < _size; i++) {
        EEPROM.write(add + i, data[i]);
    }
    EEPROM.write(add + _size,
                 '\0');  // Add termination null character for String Data
    EEPROM.commit();
}

String readStringEEPROM(char add) {
    char data[MAX_EEPROM_LEN + 1];
    int len = 0;
    unsigned char k;
    k = EEPROM.read(add);
    while (k != '\0' && len < MAX_EEPROM_LEN)  // Read until null character
    {
        k = EEPROM.read(add + len);
        data[len] = k;
        len++;
    }
    data[len] = '\0';
    Serial.println(String(data));
    return String(data);
}

String SSIDFromEeprom() {
    String SSIDFromEeprom = "";
    SSIDFromEeprom = readStringEEPROM(EEPROM_SSID);
    Serial.println(SSIDFromEeprom);
    return SSIDFromEeprom;
}

String PwdFromEeprom() {
    String PwdFromEeprom = "";
    PwdFromEeprom = readStringEEPROM(EEPROM_PASS);
    Serial.println(PwdFromEeprom);
    return PwdFromEeprom;
}

void startEeprom() {
    Serial.println("Reading settings from EEPROM");
    EEPROM.begin(EEPROM_SIZE);
}