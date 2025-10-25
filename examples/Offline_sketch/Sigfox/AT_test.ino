//AT COMMAND TALKER WITH Sigfox modem
//AT$I=10 ==> ID
//AT$I=10 ==> PAC
//Used for device registration
//AT commands list: https://www.lpwan.cz/LPWAN_sigfox_node_datasheet_v1.pdf
#include <SoftwareSerial.h>

//SW Uart for Sigfox MODEM WISOL
#define TX 5
#define RX 4
SoftwareSerial Sigfox(RX, TX);

void setup() {
  Sigfox.begin(9600); //SoftwareSerial
  Serial.begin(115200); //HW SERIAL
}

void loop() {
  if (Sigfox.available()) {
    Serial.write(Sigfox.read());
  }
  if (Serial.available()) {
    Sigfox.write(Serial.read());
  }
}
