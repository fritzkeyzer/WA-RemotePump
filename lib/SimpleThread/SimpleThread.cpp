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

#include "SimpleThread.h"

/**
 * Constructor
 * @param unsigned long interval_millis 	interval of overflow timer
 */
SimpleThread::SimpleThread(unsigned long interval_millis) : _interval_millis(interval_millis) 
{
	
}

/**
 * Destructor
 */
SimpleThread::~SimpleThread()
{

}

/**
 * SetInterval set interval of overflow timer
 * @param unsigned long interval_millis 	interval of overflow timer
 */
void SimpleThread::setInterval(unsigned long interval_millis)
{
	_interval_millis = interval_millis;
}

/**
 * Start init timer
 */
void SimpleThread::start()
{	
	if (!_flagStart) reset();
	_flagStart = true;
}

/**
 * Stop timer
 */
void SimpleThread::stop()
{
	_flagStart = false;
}

/**
 * Check verify overflow timer
 * @return if true overflow if false not overflow
 */
bool SimpleThread::check()
{
	if (!_flagStart) return false;
	
	unsigned long now = millis();
	if (now - _previous_millis >= _interval_millis)
	{
		_previous_millis = now;
		return true;
	}
	return false;
}

/**
 * Reset timer
 */
void SimpleThread::reset()
{
	_previous_millis = millis();
}

/**
 * Long button click
 * @param int pin	pin of button
 * @param unsigned long intervalPress 	interval of long click
 * @return if true overflow of click
 */
bool SimpleThread::buttonIsPressTimeout(int pin, unsigned long intervalPress)
{
	if (!digitalRead(pin))
	{
		setInterval(intervalPress);
		start();
		if (check())
		{
			return(true);
		}
	}
	else
	{
		stop();
		return(false);
	}
}
