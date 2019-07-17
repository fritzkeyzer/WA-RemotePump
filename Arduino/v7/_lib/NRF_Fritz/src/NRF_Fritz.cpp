#include "NRF_Fritz.h"


// ---------------------------------- PUBLIC

CommsUnit::CommsUnit(uint8_t _id, uint8_t _cePin, uint8_t _csnPin) 	: MY_ID(_id), PIN_RADIO_CE(_cePin), PIN_RADIO_CSN(_csnPin)
{
	_TX_radioData.FromRadioId = MY_ID;
}

void CommsUnit::init(void)
{
	_radio.init(MY_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, CHANNEL);
}

void CommsUnit::update(void)
{
	if (_radio.hasData())
	{
		getPacket();
		messageCallbackMethod(_RX_radioData.Message);
	}
}

void CommsUnit::sendMessage(uint8_t _to, CommsUnit::message_e _msg)
{
	_TX_radioData.ToRadioId = _to;
	_TX_radioData.Message = _msg;
	//_msg.toCharArray(_TX_radioData.Message, sizeof(_TX_radioData.Message ));
	sendPacket();
}

void CommsUnit::attachMessageCallback(void func(CommsUnit::message_e _msg))
{
	messageCallbackMethod = func;
}

// ---------------------------------- PRIVATE

void CommsUnit::sendPacket(void)
{
	_radio.send(_TX_radioData.ToRadioId, &_TX_radioData, sizeof(_TX_radioData), NRFLite::NO_ACK);
}

void CommsUnit::getPacket(void)
{
	_radio.readData(&_RX_radioData);
}


