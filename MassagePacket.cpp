#include "MassagePacket.h"

MassagePacket::MassagePacket(){
	MessageTx.sync = SYNC_PAKAGE;
	MessageTx.len = 0;
	MessageTx.destid = 0;
	MessageTx.srcid = 0;
	MessageTx.cmdid = 0;
	MessageTx.msgid = 0;
	MessageTx.crc = 0;

	MessageRx.sync = 0;
	MessageRx.len = 0;
	MessageRx.destid = 0;
	MessageRx.cmdid = 0;
	MessageRx.msgid = 0;
	MessageRx.crc = 0;
	
	

	_nodeID = 0;
	_rejNodeID = 0;
	_avoidLEN =0;
		
	MassageRx_Status.parse_state = 0;
	MassageRx_Status.download_state = 0;
	
}

void MassagePacket::begin(){
	begin(SERAILPORT0,115200,0x01); //master 0x01
}

void MassagePacket::begin(uint8_t sysID){
  _nodeID = sysID;		
}





uint8_t MassagePacket::getSystemID(){
	return MessageRx.destid;
}

uint8_t MassagePacket::getCommandID(){
	return MessageRx.cmdid;	
}

uint8_t MassagePacket::getMessageID(){
	return MessageRx.msgid;
}

uint8_t MassagePacket::getPayLoadLength(){
	return MessageRx.len;
}

boolean MassagePacket::parseByte(uint8_t ch){
	switch(MassageRx_Status.parse_state){
		case 0:{//got idle
			if(ch==SYNC_PAKAGE){
				MessageRx.sync = ch;
				MassageRx_Status.parse_state++;			
			}else{
				MassageRx_Status.parse_state = 0;
				MessageRx.sync = 0;
			}					
			break;
		}
		case 1:{ //got sync
			if(_nodeID == ch){ //reject packet
				MessageRx.destid = ch;
				_rejNodeID = false;
			}else{
				_rejNodeID = true;
			}
			MassageRx_Status.parse_state++;
			break;
		}
		case 2:{ //got sytem id
			if(_rejNodeID != true){ //reject packet
				MessageRx.srcid = ch;
			}		
			MassageRx_Status.parse_state++;
			break;
		}				
		case 3:{ //got sytem id
			if(_rejNodeID != true){ //reject packet
				MessageRx.cmdid = ch;
			}		
			MassageRx_Status.parse_state++;
			break;
		}
		case 4:{ //got command
			if(_rejNodeID != true){ //reject packet
				MessageRx.msgid = ch;
			}
			MassageRx_Status.parse_state++;
			break;
		}
		case 5:{ //got message id
			if(_rejNodeID != true){ //reject packet
				MessageRx.len = ch;
				_avoidLEN = 0;
			}else{
				_avoidLEN = ch;
			}
			MassageRx_Status.parse_state++;
			break;
		}		
		case 6:{ //got len
			if(_rejNodeID != true){ //reject packet
				if(MassageRx_Status.download_state < MessageRx.len){
					MessageRx.payload[MassageRx_Status.download_state] = ch;
					MassageRx_Status.download_state++;
					if(MassageRx_Status.download_state == MessageRx.len){
						MassageRx_Status.download_state = 0;
						MassageRx_Status.parse_state++;
					}
				}else{
					MassageRx_Status.parse_state = 0;	
				}
			}else{
				_avoidLEN--;
				if(_avoidLEN==0 || _avoidLEN > 250){ //self error
					MassageRx_Status.parse_state++;
				}
			}			
			break;
		}
		case 7:{//got playload	
			if(_rejNodeID != true){ //reject packet
				if(ch == calculateChecksum(&MessageRx)){
					MessageRx.crc = ch;
					MassageRx_Status.parse_state = 0;	
					return true;
				}else{
					MassageRx_Status.parse_state = 0;
					_rejNodeID = false;
					clearPayLoad(MessageRx.len);
				}
			}else{
				MassageRx_Status.parse_state = 0;
				_rejNodeID = false;
				clearPayLoad(MessageRx.len);	
			}
			break;
		}
	}		
	return false;
}

void  MassagePacket::getPacket(Packet_StructInfo &packetInfo){
	packetInfo.destid = MessageRx.destid;
	packetInfo.srcid = MessageRx.srcid;
	packetInfo.cmdid = MessageRx.cmdid;
	packetInfo.msgid = MessageRx.msgid;
	packetInfo.len = MessageRx.len;	
	for(uint8_t MsgRx = 0;MsgRx < MessageRx.len;MsgRx++){
		packetInfo.payload[MsgRx] = MessageRx.payload[MsgRx];
	}	
}



void MassagePacket::setPropertyTransmit(uint8_t addrDest,uint8_t addrSrc,uint8_t cmd,uint8_t msgID){
	MessageTx.destid = addrDest;
	MessageTx.srcid = addrSrc;
	MessageTx.cmdid = cmd;
	MessageTx.msgid = msgID;
}

void MassagePacket::setPayloadTransmit(uint8_t size,uint8_t *param){
	for(uint8_t MsgTx = 0;MsgTx < size;MsgTx++){
		MessageTx.payload[MsgTx] = *(param + MsgTx);
	}	
	MessageTx.len = size;
	//sync-destid-srcid-cmdid-msgid-len-data[n]-crc
	packetTxStep = 6+size; //7-1 = 6 defualt packet(0..6+data n)
}

uint8_t MassagePacket::transmitPacket(){
//	MessageTx.sync	= SYNC_PAKAGE;
//	MessageTx.crc = 0;
//	SerialPort->write(MessageTx.sync);  MessageTx.crc = MessageTx.crc^MessageTx.sync;
//	SerialPort->write(MessageTx.destid); MessageTx.crc = MessageTx.crc^MessageTx.destid;
//	SerialPort->write(MessageTx.cmdid); MessageTx.crc = MessageTx.crc^MessageTx.cmdid;
//	SerialPort->write(MessageTx.msgid); MessageTx.crc = MessageTx.crc^MessageTx.msgid;
//	SerialPort->write(MessageTx.len);	MessageTx.crc = MessageTx.crc^MessageTx.len;
//	for(uint8_t MsgTx = 0;MsgTx < MessageTx.len;MsgTx++){
//		SerialPort->write(MessageTx.payload[MsgTx]); MessageTx.crc = MessageTx.crc^MessageTx.payload[MsgTx];
//	}
//	SerialPort->write(MessageTx.crc);
	uint8_t addrTx = ((MessageTx.len+6) - packetTxStep);
	packetTxStep--; if(packetTxStep < 0) packetTxStep = 0;
	return (uint8_t *)&(MessageTx+addr)
}

uint8_t sizePackectTransmit(){
	return	packetTxStep;
}

uint8_t MassagePacket::calculateChecksum(Message_StructInfo *msgInfo){
	uint8_t ret = 0;		
	ret = ret^msgInfo->sync;
	ret = ret^msgInfo->destid;
	ret = ret^msgInfo->srcid;
	ret = ret^msgInfo->cmdid;
	ret = ret^msgInfo->msgid;
	ret = ret^msgInfo->len;
	for(uint8_t crc = 0;crc<msgInfo->len;crc++){
		ret = ret^msgInfo->payload[crc];
	}
		
	return ret;
}

void MassagePacket::printInfo()
{
  Serial.print("Sync: ");
  Serial.println(MessageRx.sync ,HEX);
  Serial.print("Destination ID: ");
  Serial.println(MessageRx.destid,HEX);  
  Serial.print("Source ID: ");
  Serial.println(MessageRx.srcid,HEX);   
  Serial.print("Command ID: ");
  Serial.println(MessageRx.cmdid ,HEX);
  Serial.print("Message ID: ");
  Serial.println(MessageRx.msgid,HEX);
  Serial.print("Len ID: ");
  Serial.println(MessageRx.len,HEX);
  Serial.print("Payload: ");

  for(uint8_t MsgRx = 0;MsgRx < MessageRx.len;MsgRx++){
	Serial.print(MessageRx.payload[MsgRx],HEX);Serial.print(" ");
  }

  Serial.print("Crc: ");
  Serial.println(MessageRx.crc,HEX);
  Serial.println("------------");
}

void MassagePacket::clearPayLoad(uint8_t size){
  for(uint8_t MsgRx = 0;MsgRx < size; MsgRx++){
	MessageRx.payload[MsgRx]=0;
  }	
}
