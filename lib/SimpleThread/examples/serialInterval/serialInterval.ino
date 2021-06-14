/**************************************************************************
 This example sends a Serial message every 500 milliseconds       
***************************************************************************
 AUTOR: LUCAS MAZIERO - Electrical Engineer                                                     
 E-MAIL: lucas.mazie.ro@hotmail.com 									  
 IDADE: Santa Maria - Rio Grande do Sul - Brasil                                       
***************************************************************************
 Versão: 1.0                                                             
 Data: 26/03/2016                                                        
 Modificado: --/--/----                                                                                                                          
**************************************************************************/

#include <SimpleThread.h> // Include the SimpleThread library

SimpleThread serialThread(500);  // Instantiate an instance

void setup() 
{
	Serial.begin(115200); // Start the Serial communication
}

void loop() 
{
	if (serialThread.check()) // check if the metro has passed it's interval .
	{ 
		// Output all the analog readings seperated by a space character
		Serial.print("Value read: ");
		Serial.println(analogRead(A0));
	}
}

