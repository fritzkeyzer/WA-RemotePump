#ifndef _NRF_FRITZ_h_
#define _NRF_FRITZ_h_

#include <Arduino.h>
#include <SPI.h>
#include <NRFLite.h>


/*
Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND
*/

class CommsUnit 
{

	public:
		//enum message_e
		//{
		//	PUMP_TRANSFER_START,
		//	PUMP_TRANSFER_STOP,
		//	PUMP_RIVER_RUNNING,
		//	PUMP_RIVER_STOPPED,
		//	PUMP_TRANSFER_RUNNING,
		//	PUMP_TRANSFER_STOPPED,
		//	TIME_DAY,
		//	TIME_NIGHT,
		//	LOWFLOW_OK,
		//	LOWFLOW_WARNING,
		//	LOWFLOW_ALARM,
		//};
		
		enum lowflowState
		{
			OK,
			WARNING,
			ALARM,
		};
		
		struct message_s
		{
			bool pumpIntention;
			bool isDay;
			bool riverPumpStatus;
			bool transferPumpStatus;
			lowflowState lowflowStatus;
		};
		
		
		CommsUnit(uint8_t _id, uint8_t _cePin, uint8_t _csnPin);
		void init(void);
		void update(void);
		void sendMessage(uint8_t _to, CommsUnit::message_s _msg);
		void attachMessageCallback(void func(CommsUnit::message_s _msg));


	private:
	
		
		
		struct RadioPacket						// Note the max packet size is 32 bytes
		{
			uint8_t FromRadioId;
			uint8_t ToRadioId;
			message_s Message;
			//char Message[16];    				//careful on size here
		};
		
		
		
		const uint8_t CHANNEL = 100;
		const uint8_t MY_ID = 1;             	// Our radio's id.
		const uint8_t PIN_RADIO_CE = 9;
		const uint8_t PIN_RADIO_CSN = 8;
		
		NRFLite _radio;
		RadioPacket _TX_radioData;
		RadioPacket _RX_radioData;
		
		//callback
		typedef void (*callbackFuncType)(CommsUnit::message_s);
		CommsUnit::callbackFuncType messageCallbackMethod;
		
		
		//methods:
		void sendPacket(void);				//sends 	_TX_radioData
		void getPacket(void);
		
};

#endif
