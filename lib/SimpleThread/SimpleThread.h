/**************************************************************************
LIBRARY THREAD(DELAY NOT LOCKED) FOR ESP8266 OR LINE AVR
***************************************************************************
AUTHOR: Lucas Maziero - Electrical Engineer
EMAIL: lucas.mazie.ro@hotmail.com or lucasmaziero@foxiot.com.br
CITY: Santa Maria - Rio Grande do Sul - Brasil
FUNTATION: Fox IoT (www.foxiot.com.br)
***************************************************************************
Versão: 1.0.2
Data: 26/03/2016
Modificado: 05/11/2017
***************************************************************************
Code base: BASE DO CODIGO: https://github.com/thomasfredericks/Metro-Arduino-Wiring
***************************************************************************
CHANGELOG:
* 26/03/2016 (1.0.0v):
    -> Initial version not have log :)
* 24/03/2017 (1.0.1v):
    -> Add new function "start", "stop" and "setInterval"
    -> Add new function "buttonIsPressTimeout" for use with
       button anti-debounce
* 05/11/2017 (1.0.2v):
    -> Add new versioning style x.x.x
    -> Some improvements in general

***************************************************************************
Copyright(c) by: Lucas Maziero.
**************************************************************************/

#ifndef SimpleThread_h
#define SimpleThread_h

#include "Arduino.h"

class SimpleThread
{
	public:
	/**************************************************************************
	Functions public
	***************************************************************************/
	SimpleThread(unsigned long interval_millis = 1000);
	virtual ~SimpleThread();
	void setInterval(unsigned long interval_millis);
	bool check();
	void start();
	void stop();
	void reset();
	bool buttonIsPressTimeout(int pin, unsigned long intervalPress);
	/**************************************************************************
	variables public
	***************************************************************************/
	
	private:
	/**************************************************************************
	Functions private
	***************************************************************************/

	/**************************************************************************
	variables private
	***************************************************************************/
	unsigned long _previous_millis, _interval_millis;
	bool _flagStart = true;
	
};

#endif /* SimpleThread_h */
