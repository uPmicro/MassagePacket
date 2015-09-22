#include <MassagePacket.h>
MassagePacket PacketMonitor;
MassagePacket_StructInfo PacketInfo;
MassagePacket_Status_t PacketStauts;


uint8_t payLoadCache[20];
uint8_t sizeTransmit;
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  // put your setup code here, to run once:
  PacketMonitor.begin(0x02);
  PacketStauts.parse_state = 0;
  PacketStauts.download_state = 0;	  
}

void loop() {
  uint8_t ch;
  while(Serial1.available()){ 
     ch = Serial1.read();
     if(PacketMonitor.parseByte(ch,PacketStauts)){
       PacketMonitor.printInfo();
     }
  }//end while

}
