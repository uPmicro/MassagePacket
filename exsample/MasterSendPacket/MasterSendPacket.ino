#include <MassagePacket.h>
MassagePacket PacketMonitor;   //class
MassagePacket_StructInfo PacketInfo;
MassagePacket_Status_t PacketStauts;


uint8_t payLoadCache[20];

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  // put your setup code here, to run once:
  PacketMonitor.begin(0x01);
  
}

void loop() {
  
  payLoadCache[0] = 0xAA;
  payLoadCache[1] = 0xBB;
  payLoadCache[2] = 0xCC;
  payLoadCache[3] = 0xDD;
  
 
  PacketMonitor.setPropertyTransmit(0x02,0x01,03,04); //master(0x01) to slave(0x02) cmdid(03) msgid(04)
  PacketMonitor.setPayloadTransmit(3,payLoadCache);  
  do{
    Serial1.write(PacketMonitor.transmitPacket(PacketStauts));
  }while(PacketMonitor.nextPacketTransmit());


  
  delay(1000);
}
