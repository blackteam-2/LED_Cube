/* 
	Editor: http://www.visualmicro.com
	        arduino debugger, visual micro +, free forum and wiki
	
	Hardware: Arduino Mega 2560 or Mega ADK, Platform=avr, Package=arduino
*/

#define __AVR_ATmega2560__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
//
void latchData(int multiplex, int Data);
volatile void latchLayer(int Data, bool level);
void setMultiplexer(int pos);
void setData(int data);
void setLayer(bool data, int Port);
void patternUpdateTimer(int reload);
void layerUpdateTimer(int reload);
int cubeRowToInt(int layer, int Row);
void VoltageCheck();
void VoltageError();
void setPixel(int lay, int row, int pix, bool level);
bool getPixel(int lay, int row, int pix);
bool inRange(int lay, int row, int pix);
void setLine(int axis, int layer, int pos, char data);
void setLayer(int axisa, int numa, boolean level);
void setAll();
void clearAll();
void shift(int axis, int dir);
void getCharPattern(char chr, unsigned char rtnChr[5]);
void addChrToPath(chr inputChr, int pos);
void incrementPath();
void addPathToCube();
void rainEffect(int iterations, int itterationDelay);
void textScrollEffect(int iterations, String inputstr, int delayTime);

#include "E:\Dropbox\Projects\arduino-1.0.5\hardware\arduino\variants\mega\pins_arduino.h" 
#include "E:\Dropbox\Projects\arduino-1.0.5\hardware\arduino\cores\arduino\arduino.h"
#include "E:\Dropbox\Projects\LED_Cube\8x8x8\Arduino code\LED_cube_testing\LED_cube_testing.ino"
