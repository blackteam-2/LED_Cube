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
void latchLayer(int Data);
void setMultiplexer(int pos);
void setData(int data);
void setLayer(int data);
void patternUpdateTimer(int reload);
void layerUpdateTimer(int reload);
int cubeRowToInt(int layer, int Row);

#include "E:\Dropbox\Projects\arduino-1.0.5\hardware\arduino\variants\mega\pins_arduino.h" 
#include "E:\Dropbox\Projects\arduino-1.0.5\hardware\arduino\cores\arduino\arduino.h"
#include "E:\Dropbox\Projects\LED Cube\8x8x8\Arduino code\LED_cube_testing\LED_cube_testing.ino"
