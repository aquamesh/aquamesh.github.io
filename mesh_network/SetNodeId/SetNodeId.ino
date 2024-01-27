//EEPROM Writer
/*
  Future considerations: 
  - Secure connection/verification? Make sure only OUR boards connect, and in 
    the future, only a certain SUBSET OF BOARDS connect to each other 
    (e.g. two  clients in same location don't want mixed data). 
    Verification refers to the fact that individual nodes have individualized
    maps of other nodes... and each node should probably know the location
    (or at least the existence) of other nodes. This is much more intensive
    in terms of both processing and memory limitations of our boards.
*/
#include <EEPROM.h>

// change this to be the ID of your node in the mesh network
uint8_t nodeId = 3;
unit8_t networkId = 1; 



void setup() {
  Serial.begin(57600); //can i make this 
  while (!Serial) ; // Wait for serial port to be available

  Serial.println("setting nodeId...");

  EEPROM.write(0, nodeId);
  EEPROM.write(1, networkId);

  Serial.print(F("set nodeId = "));
  Serial.println(nodeId);

  Serial.print(F("set network id = "));
  Serial.println(networkId);

  uint8_t readVal = EEPROM.read(0);
  uint8_t readValNetwork = EEPROM.read(1);  

  Serial.print(F("read nodeId: "));
  Serial.println(readVal);

  Serial.print(F("read networkId: "));
  Serial.println(readValNetwork);

  if (nodeId != readVal) {
    Serial.println(F("*** FAILED AT SETTING NODE ID ***"));
  } else {
    Serial.println(F("SUCCEEDED AT SETTING NODE ID"));
  }

  if (networkId != readValNetwork) {
    Serial.println(F("*** FAILED AT SETTING NETWORK ID ***"));
  } else {
    Serial.println(F("SUCCEEDED AT SETTING NETWORK ID"));
  }
}

void loop() {

}
