#include <EEPROM.h>

#define EEPROM_SIZE 300
#define EEPROM_SSID 0
#define EEPROM_PASS 100
#define EEPROM_CODE 200
#define MAX_EEPROM_LEN 50  // Max length to read ssid/passwd

void writeStringEEPROM(char add, String data);

String readStringEEPROM(char add);
String SSIDFromEeprom();
String PwdFromEeprom();

void startEeprom();