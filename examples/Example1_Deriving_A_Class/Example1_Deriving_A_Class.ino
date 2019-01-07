/*

This example shows how to use the HyperDisplay ILI9163 mid-level library
to derive a class to control your custom display using an ILI9163C driver

Author: Owen Lyke
Modified: 1/7/19

This software is open source. Use it how you like, just don't hurt people.
*/
#include "HyperDisplay_ILI9163C.h" // Get it here: https://github.com/sparkfun/HyperDisplay_ILI9163C_ArduinoLibrary or here: (insert arduino lib man link)

#define SERIAL_PORT Serial // This allows you to easily change the target of print commands (for example SerialUSB on SAMD21 boards)

/*
Now, because the ILI9163 library is an intermediate library you can't use it to control a display directly.
Instead you will need to derive a custom class for your particular display that uses an ILI9163 driver IC.

This example sketch will demonstrate the concepts of how to do that.

Before we continue: if you are using a SparkFun product then you don't need to do this - we will have already 
provided a fully configured library. Follow the hookup guide on that product to get started. 

Lastly if you aren't using a SparkFun product and you want to save some development time consider buying a board!
*/

/*
Virtual Functions:

The HyperDisplay (top level) and ILI9163 (mid-level) libraries use pure-virtual functions to force the user to provide 
functionality that the upper layer requires. Any class that contains a pure-virtual function cannot be "instatniated"
Try uncommenting this line to see what error it generates at compile:
*/
//ILI9163C myILI9163( 10, 20, ILI9163C_INTFC_4WSPI); // Usually this would create an ILI9163C object called myILI9163C. However this class contains one or more pure-virtual functions so it is illegal to declare an object

/*
To make a class that we can use (by instantiating an object) we need to provide definitions of all the pure virtual functions.
Let's look at all the pure virtual functions that exist in HyperDisplay and ILI9163C header files:
HyperDisplay:
- virtual void    hwpixel(hd_hw_extent_t x0, hd_hw_extent_t y0, color_t data = NULL, hd_colors_t colorCycleLength = 1, hd_colors_t startColorOffset = 0) = 0; 
                  * implementing hwixel allows HyperDisplay to draw anything on the screen, pixel-by-pixel. But without explicit instructions from you it wouldn't know how *
- virtual color_t getOffsetColor(color_t base, uint32_t numPixels) = 0;
                  * implementing this function is required for HyperDisplay to handle custom color types, one of its many flexible options *

ILI9163C
- virtual ILI9163C_STAT_t writePacket(ILI9163C_CMD_t* pcmd = NULL, uint8_t* pdata = NULL, uint16_t dlen = 0) = 0;
                  * this function exists because the ILI9163 has several possible data interfaces such as 3/4 Wire SPI, or 6800/8080 parallel MCU, but no matter the itnerface all operation can be condensed into the concept of a 'packet' *

*** By the way, there are other *virtual* functions in these classes, however for many a default implementation could be provided,
    for example hwxline uses hwpixel over and over by default. The functions with no defualt implementation are designated 'pure' 
    virtual by the ' = 0;' after their definition
*/

/*
All of the above functions need implementations, however the ILI9163 library takes care of the two functions frm HyperDisplay on its own. 
This leaves only the writePacket function to take care of.

To do this we will make a new class derived from ILI9163C that includes a function with the same signature as 'writePacket' but not virtual.

For the sake of transparency our 'writePacket' function will just print the packet data to the serial port

And of course, you can include any other specific data you might want in the class
*/

class CUSTOM_ILI9163C : public ILI9163C{
private:
protected:
public:
  const char* dispName; // Example of additional data you might include in a class

  CUSTOM_ILI9163C( void ); // Constructor
  ILI9163C_STAT_t writePacket(ILI9163C_CMD_t* pcmd = NULL, uint8_t* pdata = NULL, uint16_t dlen = 0);
};

// Then we need to provide the definition of the functions:
CUSTOM_ILI9163C::CUSTOM_ILI9163C( void ) : hyperdisplay( 10, 20 ), ILI9163C( 10, 20, ILI9163C_INTFC_4WSPI){
  // Nothing for this constructor, but note how the constructor for the parent was called with specific values. 
  // This could also be done using variables passed into the custom constructor
  _pxlfmt = ILI9163C_PXLFMT_18; // This line is needed to actually show our color being printed
}

ILI9163C_STAT_t CUSTOM_ILI9163C::writePacket(ILI9163C_CMD_t* pcmd, uint8_t* pdata, uint16_t dlen)
{
  // If the command pointer is not null we will send one byte from that location
  if(pcmd != NULL){
    // Implement whatever code here for your particular interface
    SERIAL_PORT.print("cmd: 0x"); SERIAL_PORT.println(*(pcmd), HEX);  
  }

  // Make sure a valid data pointer is provided AND that the length is specified as nonzero
  if((pdata != NULL) && (dlen != 0)){
    // Implement your particular method of sending data bytes
    for(uint8_t indi = 0; indi < dlen; indi++){
      SERIAL_PORT.print("dat["); SERIAL_PORT.print(indi); SERIAL_PORT.print("], 0x"); SERIAL_PORT.println(*(pdata + indi), HEX);  
    }
  }
  SERIAL_PORT.println();
}

// Now it is possible to declare an object of our new class because it has defined all pure-virtual functions from all higher classes
CUSTOM_ILI9163C ourCustomDriverObject;

#define DATLEN 4

void setup() {
  SERIAL_PORT.begin(9600);
  while(!Serial){};

  SERIAL_PORT.println("ILI9163 HyperDisplay Library (mid-level) Example1: Deriving a Class");

  

  // Let's test out the function we wrote directly:
  ILI9163C_CMD_t cmd = (ILI9163C_CMD_t)0xAA;
  uint8_t dat[DATLEN] = {0xDE, 0xAD, 0xBE, 0xEF};
  ourCustomDriverObject.writePacket(&cmd, dat, sizeof(dat));


  SERIAL_PORT.println();
  SERIAL_PORT.println("==================================================================");
  SERIAL_PORT.println();


  // Now let's try to see how it is used in the broader context: we can use any of the hyperdisplay functions like xline
  ILI9163C_color_18_t myColor = {
//    .r = 0xED,
//    .g = 0xEE,
//    .b = 0xBE,
    0xED, 0xEE, 0xBE
  };
  ourCustomDriverObject.xline(1, 4, 3, &myColor);
  // The default implementation of xline is to draw each and every pixel individually, so we will see a pattern repeat three times in the serial monitor
  // Each pixel draw is made up of these commands:
  // 0x2A - Set column address
  // 0x2B - Set row address
  // 0x2C - write to ram with N bytes (3 bytes in this case)

  // Because we used xline the column address will increment each time and the row address will stay the same. 
  // You should also notice that our color was fiathfully transmitted after the 'write ram' command (0x2C)

  
  // And there you have it, if you had an ILI9163 driver that communicated over the serial port then you'd already be 
  // drawing pictures! Don't forget to check out a few pre-defined interface classes in the ILI9163 header file such 
  // as the Aduino 4WSPI interface which only requires you to define the dimensions of the display
  SERIAL_PORT.println("Done!");
}

void loop(){};
