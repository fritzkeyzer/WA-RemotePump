# SimpleThread
SimpleThread uses [millis()](https://www.arduino.cc/en/Reference/Millis) as a time base, so it becomes possible to emulate simple threads or perform everyday tasks without using [delay()](https://www.arduino.cc/en/Reference/Delay), so the processor is releasing to perform other tasks in that interval. Tested with ESP8266-12(LoLin) and Arduino-Uno.

Compatible with the Arduino IDE 1.8.4 Library [SimpleThread](https://github.com/lucasmaziero/SimpleThread)

Inspiration Library [Metro](https://github.com/thomasfredericks/Metro-Arduino-Wiring/tree/master/Metro)

## Installation
Create a new folder called "SimpleThread" under the folder named "libraries" in your Arduino sketchbook folder.
Create the folder "libraries" in case it does not exist yet. Place all the files in the "SimpleThread" folder.

## Usage
To use the library in your on sketch, select it from *Sketch > Import Library*.

## Code example

Code basic example for ESP8266 or ARDUINO see more here [examples](examples)
```c++
#include <SimpleThread.h> // Include the SimpleThread library

SimpleThread serialThread(500); // Instantiate an instance

void setup()
{
  Serial.begin(115200); // Start the Serial communication
}

void loop()
{
  if (serialThread.check())
    // check if the metro has passed it's interval .
  {
    // Output all the analog readings seperated by a space character
    Serial.print("Value read: ");
    Serial.println(analogRead(A0));
  }
}
```
## License

The content is licensed under the MIT license. See [License File](LICENSE) for more information.
