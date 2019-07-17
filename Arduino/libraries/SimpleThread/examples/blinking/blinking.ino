/**************************************************************************
 This code will blink an LED attached to pin 2 on and off
***************************************************************************
 AUTOR: LUCAS MAZIERO - Eletrical Engineer                                                     
 E-MAIL: lucas.mazie.ro@hotmail.com 									  
 CIDADE: Santa Maria - Rio Grande do Sul - Brasil                                       
***************************************************************************
 Versão: 1.0                                                             
 Data: 26/03/2016                                                        
 Modificado: --/--/----                                                                                                                          
**************************************************************************/

#include <SimpleThread.h> //Include SimpleThread library

#define LED 2 // Define the led's pin

//Create a variable to hold theled's current state
int state = HIGH;

// Instanciate a metro object and set the interval to 250 milliseconds (0.25 seconds).
SimpleThread ledThread(250); 

void setup()
{
  pinMode(LED,OUTPUT);
  digitalWrite(LED,state);
}

void loop()
{

  if (ledThread.check()) // check if the metro has passed its interval .
  { 
    if (state==HIGH)  state=LOW;
    else state=HIGH;
    
    digitalWrite(LED,state);
  }
}
