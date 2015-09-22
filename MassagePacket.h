#ifndef MassagePacket_h
#define MassagePacket_h
#include <Arduino.h>


// cmdid 01 -> brocade   //scan port
//		 02	-> setting       
//		 03	-> request
// 		 04 -> acknowledge
//		 05	-> reply	

#define CMDID_BROCADE 1
#define CMDID_SETTING 2
#define CMDID_REQUEST 3
#define CMDID_ACKNOWLEDGE 4
#define CMDID_REPLY 5

#define PACKAGE_PLYLOAD_SIZE 32
#define SYNC_PAKAGE 0xF5

typedef struct MassagePacket_StructInfo{
	uint8_t destid;       //Destination id
	uint8_t srcid;    //Source id
	uint8_t cmdid;    // command id
	uint8_t msgid;    // ID of message in payload
	uint8_t len;      // Length of payload		
	uint8_t payload[PACKAGE_PLYLOAD_SIZE];		
};

typedef struct MassagePacket_Status_t{
	uint8_t parse_state;
	uint8_t download_state;	
	uint8_t upload_state;
	uint8_t transmit_state;	
};

class MassagePacket{
	public:			
	    MassagePacket();
	    
	    void begin();
	    void begin(uint8_t sysID);
	    uint8_t getSystemID();
	    uint8_t getCommandID();
	    uint8_t getMessageID();
	    uint8_t getPayLoadLength();
	    boolean parseByte(uint8_t ch,MassagePacket_Status_t &MassageRx_Status); 
	    void  getPacket(MassagePacket_StructInfo &packetInfo);
	    void printInfo();
	    
	    //uint32_t ToInt32(uint8_t *param);
	    void setPropertyTransmit(uint8_t addrDest,uint8_t addrSrc,uint8_t cmd,uint8_t msgID);
	    void setPayloadTransmit(uint8_t size,uint8_t *param);
	    uint8_t transmitPacket(MassagePacket_Status_t &MassageTx_Status);
	    uint8_t sizePackectTransmit();
	    boolean nextPacketTransmit();
	    
	// |-sync-|-destid-|-srcid-|-cmdid-|-msgid-|-len-|-payload-|-crc-|
	// cmdid 01 -> brocade   //scan port
	//		 02	-> setting       
	//		 03	-> request
	// 		 04 -> acknowledge
	//		 05	-> reply								
	// crc = destid^cmdid^msgid^payload;	  
	//
	// exsample
	//		 sync = F5
	//		 cmdid = 02    //setting
	//		 msgid = 01    //program eeprom
	//		 len = 04	
	//		 data[0] =  12 //address of eeprom 
	//		 data[1] = 2   //size of data (byte = 1) (int = 2) (float = 4)
	//		 data[3] = parameter[0];   
	//		 data[4] = parameter[1];
	//	 	 crc = destid^cmdid^msgid^payload[..n]
	
	
    private:
		
	typedef struct Message_StructInfo{
	    uint8_t sync;     // protocol start of packet
		uint8_t destid;       //Destination id
		uint8_t srcid;    //Source id
		uint8_t cmdid;    // command id
		uint8_t msgid;    // ID of message in payload
	    uint8_t len;      // Length of payload			    
		uint8_t crc; // sent at end of packet
		uint8_t payload[PACKAGE_PLYLOAD_SIZE];
	};
	Message_StructInfo MessageRx,MessageTx;
	
	struct{
		uint8_t parse_state;
		uint8_t download_state;
	}MassageRx_Status;
	

	

	volatile uint8_t _nodeID;
	volatile uint8_t _avoidLEN;
	volatile boolean _rejNodeID;
	volatile boolean _nextTransmit;
	
	
	
	uint8_t calculateChecksum(Message_StructInfo *msgInfo);
	void clearPayLoad(uint8_t size);
		
};
#endif
