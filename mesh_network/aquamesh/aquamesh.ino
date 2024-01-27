#include <SPI.h> //serial
#include <RH_RF95.h> //transceiver
#include <EEPROM.h> //flash mem
#include <RHRouter.h> //addressed datagrams
#include <RHMesh.h> //dynamic datagrams, extension of RHRouter

//PINS
#define LEDPIN 9
#define CHIPSELECTPIN 10
#define INTERRUPTPIN 2


//SERIAL CONTROLS
/******************************************************************************************************/
#define SERIAL_EN //comment out to disable serial debugging! improves performance!
#ifdef SERIAL_EN
  #define SERIAL 57600
  #define DEBUG(input) {Serial.print(input; delay(1);)}
  #define DEBUGln(input) {Serial.println(input; delay(1);)}
#else
  #define DEBUG(input); 
  #define DEBUGln(input);
  #define SERIALFLUSH();
#endif
/******************************************************************************************************/

//EXTRA TIDBITS
#define N_NODES 6
#define BROADCAST_ID 255

uint8_t nodeId;
uint8_t networkId; 
uint8_t routes[N_NODES]; // full routing table for mesh
int16_t rssi[N_NODES]; // signal strength info
char buf[RH_MESH_MAX_MESSAGE_LEN-3]; //temp internal message buffer


struct Message{ //buffer to send/receive
  uint8_t netId;
  uint8_t sourceId;
  uint8_t destId;
  char data[RH_RF95_MAX_MESSAGE_LEN-3]; //-3 because we have 3 extra chars in message
}

Message message; 

// Singleton instance of the radio driver
RH_RF95 rf95;

// Class to manage message delivery and receipt, using the driver declared above
RHMesh *manager;

// message buffer                                                                  //We need more message buffers. But how many? And how do we format them? 
                                                                                   

int freeMem() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  randomSeed(analogRead(0));
  Serial.begin(57600);
  while (!Serial) ; // Wait for serial port to be available

  nodeId = EEPROM.read(0);
  message.netId = EEPROM.read(1); //should be permanent, doesn't need separate var
  
  Serial.print(F("initializing node "));

  manager = new RHMesh(rf95, nodeId);

  if (!manager->init()) {
    Serial.println(F("init failed"));
  } else {
    Serial.println("done");
  }
  rf95.setTxPower(23, false);
  rf95.setFrequency(915.0);
  rf95.setCADTimeout(500);

  // Possible configurations:
  // Bw125Cr45Sf128 (the chip default)
  // Bw500Cr45Sf128
  // Bw31_25Cr48Sf512
  // Bw125Cr48Sf4096

  // long range configuration requires for on-air time
  boolean longRange = false;
  if (longRange) {
    RH_RF95::ModemConfig modem_config = {
      0x78, // Reg 0x1D: BW=125kHz, Coding=4/8, Header=explicit
      0xC4, // Reg 0x1E: Spread=4096chips/symbol, CRC=enable
      0x08  // Reg 0x26: LowDataRate=On, Agc=Off.  0x0C is LowDataRate=ON, ACG=ON
    };
    rf95.setModemRegisters(&modem_config);
    if (!rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096)) {
      Serial.println(F("set config failed"));
    }
  }

  Serial.println("RF95 ready");

  for(uint8_t n=1;n<=N_NODES;n++) {
    routes[n-1] = 0;
    rssi[n-1] = 0;
  }

  Serial.print(F("mem = "));
  Serial.println(freeMem());
}


const __FlashStringHelper* getErrorString(uint8_t error) {
  switch(error) {
    case 1: return F("invalid length");
    break;
    case 2: return F("no route");
    break;
    case 3: return F("timeout");
    break;
    case 4: return F("no reply");
    break;
    case 5: return F("unable to deliver");
    break;
  }
  return F("unknown");
}


void updateRoutingTable() {
  for(uint8_t n=1;n<=N_NODES;n++) {
    RHRouter::RoutingTableEntry *route = manager->getRouteTo(n);
    if (n == nodeId) {
      routes[n-1] = 255; // self
    } else {
      routes[n-1] = route->next_hop;
      if (routes[n-1] == 0) {
        // if we have no route to the node, reset the received signal strength
        rssi[n-1] = 0;
      }
    }
  }
}
/*
// Create a JSON string with the routing info to each node
void getRouteInfoString(char *p, size_t len) {
  p[0] = '\0';
  strcat(p, "[");
  for(uint8_t n=1;n<=N_NODES;n++) {
    strcat(p, "{\"n\":");
    sprintf(p+strlen(p), "%d", routes[n-1]);
    strcat(p, ",");
    strcat(p, "\"r\":");
    sprintf(p+strlen(p), "%d", rssi[n-1]);
    strcat(p, "}");
    if (n<N_NODES) {
      strcat(p, ",");
    }
  }
  strcat(p, "]");
}

void printNodeInfo(uint8_t node, char *s) {
  Serial.print(F("node: "));
  Serial.print(F("{"));
  Serial.print(F("\""));
  Serial.print(node);
  Serial.print(F("\""));
  Serial.print(F(": "));
  Serial.print(s);
  Serial.println(F("}"));
}
*/

void loop() {

  uint8_t reading = analogRead(0);
  message.data = reading;
  message.sourceId = nodeId;

  for(uint8_t n = 1; n <= N_NODES; n++) {
    if (n == nodeId) continue; // self

    updateRoutingTable();
    //getRouteInfoString(buf, RH_MESH_MAX_MESSAGE_LEN);
    /*
    Serial.print(F("->"));
    Serial.print(n);
    Serial.print(F(" :"));
    Serial.print(buf);
    */
    // send an acknowledged message to the target node
    
    uint8_t error = manager->sendtoWait((uint8_t *)buf, strlen(buf), n);
    if (error != RH_ROUTER_ERROR_NONE) {
      Serial.println();
      Serial.print(F(" ! "));
      Serial.println(getErrorString(error));
    } else {
      Serial.println(F(" OK"));
      // we received an acknowledgement from the next hop for the node we tried to send to.
      RHRouter::RoutingTableEntry *route = manager->getRouteTo(n);
      if (route->next_hop != 0) {
        rssi[route->next_hop-1] = rf95.lastRssi();
      }
    }
    //if (nodeId == 1) printNodeInfo(nodeId, buf); // debugging

    // listen for incoming messages. Wait a random amount of time before we transmit
    // again to the next node
    unsigned long nextTransmit = millis() + random(3000, 5000);
    while (nextTransmit > millis()) {
      int waitTime = nextTransmit - millis();
      uint8_t len = sizeof(buf);
      uint8_t from;
      if (manager->recvfromAckTimeout((uint8_t *)buf, &len, waitTime, &from)) {
        buf[len] = '\0'; // null terminate string
        Serial.print(from);
        Serial.print(F("->"));
        Serial.print(F(" :"));
        Serial.println(buf);
        if (nodeId == 1) printNodeInfo(from, buf); // debugging
        // we received data from node 'from', but it may have actually come from an intermediate node
        RHRouter::RoutingTableEntry *route = manager->getRouteTo(from);
        if (route->next_hop != 0) {
          rssi[route->next_hop-1] = rf95.lastRssi();
        }
      }
    }
  }

}
