/*
J McKenna
8x8x8 LED Cube

//description


===Software===
Program Version: 1.1
Last Update: 19/02/2015


Revision Notes:

Ver     Date     Notes
0.1 - 20/06/14 - Created program
0.2 - 24/06/14 - Implemented Layer update timer
0.3 - 24/06/14 - Added effect functions and the rain effect
0.4 - 01/07/14 - Added Text scroll effect and related functions, Cleaned up other sections
0.5 - 01/07/14 - Added top down effect + tidied up code in effect and background functions
0.6 - 26/09/14 - Fixed Shit function, Corrected pin assignments, completed top down effect*, fixed multiplexer, inverted layer latch, Fixed CubeRowToInt() Function        *axis=2 is being a bitch and wont work
0.7 - 04/11/14 - WORKING. Effect rain effect text and Effect top down and related func changed and re mostly working
0.8 - 08/11/14 - Added Effect_UpDown_Suspend, fixed z=7 displaying on layer above by adding cubesize+1 in interrupt
0.9 - 08/11/14 - Added Effect_ShootRandPixel, Played with time delays on functions
===RELEASE===
1.0 - 09/11/14 - Added Effect_Fireworks, tidyed up code, Made demo calls and timings 
1.1 - 19/02/15 - Added EffectShootText + addTextToCubeLayer + Function_Demo, fixed EffectBoxGrow iterations + loop endings, Moved the demo code to Function_demo and called function in main to tidy code


===Hardware===
Arduino Board: Mega R3


      TOP VEIW (Data)
  
  00-01-02-03-04-05-06-07
  08-09-10-11-12-13-14-15
  16-17-18-19-20-21-22-23
  24-25-26-27-28-29-30-31
  32-33-34-35-36-37-38-39
  40-41-42-43-44-45-46-47
  48-49-50-51-52-53-54-55
  56-57-58-59-60-61-62-63

  
  SIDE VIEW (Layer)
  
  ----7----(H)----
  ----6----(G)----
  ----5----(F)----
  ----4----(E)----
  ----3----(D)----
  ----2----(C)----
  ----1----(B)----
  ----0----(A)----


   Cord sys
  
	  Y
	  |
	  :--X
	 /
	Z

*/

//
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>



//---Pin definitions---
const int LED = 13;

const int D0 = 48;
const int D1 = 50;
const int D2 = 46;
const int D3 = 44;
const int D4 = 40;
const int D5 = 42;
const int D6 = 38;
const int D7 = 52;

const int A = 5;
const int B = 4;
const int C = 3;
const int E = 6;

const int RD0 = 23;
const int RD1 = 29;
const int RD2 = 31;
const int RD3 = 37;
const int RD4 = 35;
const int RD5 = 33;
const int RD6 = 27;
const int RD7 = 25;
const int RE = 41;


//---Reference Constant Definitions---
#define cubeSize 8
#define textPathLength 34


//---Storage arrays---

//Array contains characters stored in 5 x 8bit sections
volatile const unsigned char chracterStorageArray[455] = {
	0x00,0x00,0x00,0x00,0x00, 0x00,0x5f,0x5f,0x00,0x00,	//  !
	0x00,0x03,0x00,0x03,0x00, 0x14,0x7f,0x14,0x7f,0x14,	// "#
	0x24,0x2a,0x7f,0x2a,0x12, 0x23,0x13,0x08,0x64,0x62,	// $%
	0x36,0x49,0x55,0x22,0x50, 0x00,0x05,0x03,0x00,0x00,	// &'
	0x00,0x1c,0x22,0x41,0x00, 0x00,0x41,0x22,0x1c,0x00,	// ()`
	0x14,0x08,0x3e,0x08,0x14, 0x08,0x08,0x3e,0x08,0x08,	// *+
	0x00,0x50,0x30,0x00,0x00, 0x08,0x08,0x08,0x08,0x08,	// ,-
	0x00,0x60,0x60,0x00,0x00, 0x20,0x10,0x08,0x04,0x02,	// ./
	0x3e,0x51,0x49,0x45,0x3e, 0x00,0x42,0x7f,0x40,0x00,	// 01
	0x42,0x61,0x51,0x49,0x46, 0x21,0x41,0x45,0x4b,0x31,	// 23
	0x18,0x14,0x12,0x7f,0x10, 0x27,0x45,0x45,0x45,0x39,	// 45
	0x3c,0x4a,0x49,0x49,0x30, 0x01,0x71,0x09,0x05,0x03,	// 67
	0x36,0x49,0x49,0x49,0x36, 0x06,0x49,0x49,0x29,0x1e,	// 89
	0x00,0x36,0x36,0x00,0x00, 0x00,0x56,0x36,0x00,0x00,	// :;
	0x08,0x14,0x22,0x41,0x00, 0x14,0x14,0x14,0x14,0x14,	// <=
	0x00,0x41,0x22,0x14,0x08, 0x02,0x01,0x51,0x09,0x06,	// >?
	0x32,0x49,0x79,0x41,0x3e, 0x7e,0x11,0x11,0x11,0x7e,	// @A
	0x7f,0x49,0x49,0x49,0x36, 0x3e,0x41,0x41,0x41,0x22,	// BC
	0x7f,0x41,0x41,0x22,0x1c, 0x7f,0x49,0x49,0x49,0x41,	// DE
	0x7f,0x09,0x09,0x09,0x01, 0x3e,0x41,0x49,0x49,0x7a,	// FG
	0x7f,0x08,0x08,0x08,0x7f, 0x00,0x41,0x7f,0x41,0x00,	// HI
	0x20,0x40,0x41,0x3f,0x01, 0x7f,0x08,0x14,0x22,0x41,	// JK
	0x7f,0x40,0x40,0x40,0x40, 0x7f,0x02,0x0c,0x02,0x7f,	// LM
	0x7f,0x04,0x08,0x10,0x7f, 0x3e,0x41,0x41,0x41,0x3e,	// NO
	0x7f,0x09,0x09,0x09,0x06, 0x3e,0x41,0x51,0x21,0x5e,	// PQ
	0x7f,0x09,0x19,0x29,0x46, 0x46,0x49,0x49,0x49,0x31,	// RS
	0x01,0x01,0x7f,0x01,0x01, 0x3f,0x40,0x40,0x40,0x3f,	// TU
	0x1f,0x20,0x40,0x20,0x1f, 0x3f,0x40,0x38,0x40,0x3f,	// VW
	0x63,0x14,0x08,0x14,0x63, 0x07,0x08,0x70,0x08,0x07,	// XY
	0x61,0x51,0x49,0x45,0x43, 0x00,0x7f,0x41,0x41,0x00,	// Z[
	0x02,0x04,0x08,0x10,0x20, 0x00,0x41,0x41,0x7f,0x00,	// \]
	0x04,0x02,0x01,0x02,0x04, 0x40,0x40,0x40,0x40,0x40,	// ^_
	0x00,0x01,0x02,0x04,0x00, 0x20,0x54,0x54,0x54,0x78,	// `a
	0x7f,0x48,0x44,0x44,0x38, 0x38,0x44,0x44,0x44,0x20,	// bc
	0x38,0x44,0x44,0x48,0x7f, 0x38,0x54,0x54,0x54,0x18,	// de
	0x08,0x7e,0x09,0x01,0x02, 0x0c,0x52,0x52,0x52,0x3e,	// fg
	0x7f,0x08,0x04,0x04,0x78, 0x00,0x44,0x7d,0x40,0x00,	// hi
	0x20,0x40,0x44,0x3d,0x00, 0x7f,0x10,0x28,0x44,0x00,	// jk
	0x00,0x41,0x7f,0x40,0x00, 0x7c,0x04,0x18,0x04,0x78,	// lm
	0x7c,0x08,0x04,0x04,0x78, 0x38,0x44,0x44,0x44,0x38,	// no
	0x7c,0x14,0x14,0x14,0x08, 0x08,0x14,0x14,0x18,0x7c,	// pq
	0x7c,0x08,0x04,0x04,0x08, 0x48,0x54,0x54,0x54,0x20,	// rs
	0x04,0x3f,0x44,0x40,0x20, 0x3c,0x40,0x40,0x20,0x7c,	// tu
	0x1c,0x20,0x40,0x20,0x1c, 0x3c,0x40,0x30,0x40,0x3c,	// vw
	0x44,0x28,0x10,0x28,0x44, 0x0c,0x50,0x50,0x50,0x3c,	// xy
	0x44,0x64,0x54,0x4c,0x44 				            // z
};

volatile unsigned char textPath[textPathLength] = {0};


//-------Cube storage array and update variables-------

//Cube array [Layer][Row][Bit] - [Y][X][Z]
boolean cube[8][8][8] = {false}; 
volatile int layer = 1;
volatile int prevLayer = 0;


//-------Voltage lim/error-------

//Set the voltage lim that will cause a Voltage error
int battLim = 520;


//-------General Use Variables-------
int i = 0;
int j = 0;
int k = 0;
int temp = 0;
boolean first = false;

//TO FIX GCC SHITNESS
int helloworld = 4500;


//========================================================================
//===================================SETUP================================
//========================================================================

void setup()
{
	//set Pin modes
	pinMode(D0, OUTPUT);
	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);
	pinMode(D5, OUTPUT);
	pinMode(D6, OUTPUT);
	pinMode(D7, OUTPUT);
  
	pinMode(RD0, OUTPUT);
	pinMode(RD1, OUTPUT);
	pinMode(RD2, OUTPUT);
	pinMode(RD3, OUTPUT);
	pinMode(RD4, OUTPUT);
	pinMode(RD5, OUTPUT);
	pinMode(RD6, OUTPUT);
	pinMode(RD7, OUTPUT);
  
	pinMode(A, OUTPUT);
	pinMode(B, OUTPUT);
	pinMode(C, OUTPUT);
	pinMode(E, OUTPUT);
  
	pinMode(A7, INPUT);
  
	//Testing
	pinMode(LED, OUTPUT);
  
	delay(1000);
		
	//Debugging
	Serial.begin(57600);
		
	//Enable Global interrupts
	sei();
    
	//
	Serial.println("Cube ready");
	
	//
	//62500 - 0.25Hz
	//15625 - 1Hz
	//3906 - 4Hz
	//1953 - 8Hz 
	//977 - 16Hz
	//781 - 20Hz
	//500 - 31.25Hz
	//391 - 40Hz
	layerUpdateTimer(40);//35
	
	//turn all the cube layers off
	clearAll();
	
	//
	delay(1000);
}


//=============================================================================
//==================================MAIN LOOP==================================
//=============================================================================

//
void loop()
{		
	
	Function_Demo();
	
}


//========================================================================
//=================================LATCH DATA=============================
//========================================================================

//
void latchData(int multiplex, int LatchData)
{
	digitalWrite(E, HIGH);
	setData(LatchData);
	setMultiplexer(multiplex);
	digitalWrite(E,LOW);
}

//
volatile void latchLayer(int Data, bool level)
{
	digitalWrite(RE, HIGH);
	setLayer(level, Data);
	delayMicroseconds(2);
	digitalWrite(RE, LOW);
}


//========================================================================
//================================SET I/O=================================
//========================================================================

//Set the multiplexer channel
//
//The multiplexer controls the latch pin on the 8 data latch IC's
void setMultiplexer(int pos)
{
	digitalWrite(A, HIGH && (pos & 0x01));
	digitalWrite(B, HIGH && (pos & 0x02));
	digitalWrite(C, HIGH && (pos & 0x04));
}


//Set the common data lines on the data Latch IC's
//
//===Invert the data to account for cube construction being active low===
void setData(int Ldata)
{
	digitalWrite(D0, !(HIGH && (Ldata & 0x01)));
	digitalWrite(D1, !(HIGH && (Ldata & 0x02)));
	digitalWrite(D2, !(HIGH && (Ldata & 0x04)));
	digitalWrite(D3, !(HIGH && (Ldata & 0x08)));
	digitalWrite(D4, !(HIGH && (Ldata & 0x10)));
	digitalWrite(D5, !(HIGH && (Ldata & 0x20)));
	digitalWrite(D6, !(HIGH && (Ldata & 0x40)));
	digitalWrite(D7, !(HIGH && (Ldata & 0x80)));
}


// Set the data lines on the layer Latch IC
//
// data:
// True - Turn Layer off
// False - Turn Layer on
//
// Port:
// 0-7 - Select the individual port
// 8 - Set all the data pins
void setLayer(bool data, int Port)
{
	switch(Port)
	{
		case 0:
			if (data)
			{
				digitalWrite(RD0, HIGH);
			}
			else
			{
				digitalWrite(RD0, LOW);
			}
		break;
		
		case 1:
			if (data)
			{
				digitalWrite(RD1, HIGH);
			}
			else
			{
				digitalWrite(RD1, LOW);
			}
		break;
		
		case 2:
			if (data)
			{
				digitalWrite(RD2, HIGH);
			}
			else
			{
				digitalWrite(RD2, LOW);
			}
		break;
		
		case 3:
			if (data)
			{
				digitalWrite(RD3, HIGH);
			}
			else
			{
				digitalWrite(RD3, LOW);
			}
		break;
		
		case 4:
			if (data)
			{
				digitalWrite(RD4, HIGH);
			}
			else
			{
				digitalWrite(RD4, LOW);
			}
		break;
		
		case 5:
			if (data)
			{
				digitalWrite(RD5, HIGH);
			}
			else
			{
				digitalWrite(RD5, LOW);
			}
		break;
		
		case 6:
			if (data)
			{
				digitalWrite(RD6, HIGH);
			}
			else
			{
				digitalWrite(RD6, LOW);
			}
		break;
		
		case 7:
			if (data)
			{
				digitalWrite(RD7, HIGH);
			}
			else
			{
				digitalWrite(RD7, LOW);
			}
		break;
		
		case 8:
			if (data)
			{
				digitalWrite(RD0, HIGH);
				digitalWrite(RD1, HIGH);
				digitalWrite(RD2, HIGH);
				digitalWrite(RD3, HIGH);
				digitalWrite(RD4, HIGH);
				digitalWrite(RD5, HIGH);
				digitalWrite(RD6, HIGH);
				digitalWrite(RD7, HIGH);
			}
			else
			{
				digitalWrite(RD0, LOW);
				digitalWrite(RD1, LOW);
				digitalWrite(RD2, LOW);
				digitalWrite(RD3, LOW);
				digitalWrite(RD4, LOW);
				digitalWrite(RD5, LOW);
				digitalWrite(RD6, LOW);
				digitalWrite(RD7, LOW);
			}
		break;
		
		default:
			//Do nothing, something has gone wrong
		break;
	}
}


// ===TESTING FUNCTION===
//
// Will prob use for V rail monitoring in the future
// Leaving the code here for now as im lazy and cant be asked re-writing it later
//
//========================================================================
//==============================PATTERN UPDATE============================
//========================================================================

//Timer 4
void patternUpdateTimer(int reload)
{
	if (reload == 0)
	{
		TCCR4B = 0x00;
		return;
	}
	
	TCCR4B |= (1 << WGM42);
	OCR4A = reload;
	TIMSK4 |= (1 << OCIE4A);
	TCCR4B |= (1 << CS40) | (1 << CS42);//clk/1024
	
}


//
ISR(TIMER4_COMPA_vect)
{
	
}


//========================================================================
//================================LAYER UPDATE============================
//========================================================================


//Timer 5
//Layer update Timer
// 0 - Turn off timer
// 1 - 65565 Reload value, alters freq of related ISR
void layerUpdateTimer(int reload)
{
	if (reload == 0)
	{
		TCCR5B = 0x00;
		return;
	}
	
	TCCR5A = 0x00;
	TCCR5B = 0x00;
	
	TCCR5B |= (1 << WGM52);
	OCR5A = reload;
	TIMSK5 |= (1 << OCIE5A);
	TCCR5B |= (1 << CS50) | (1 << CS52);//clk/1024
	
}


// Layer update ISR
//
// Used to update the individual layers of the cube
// 1 at a time quickly.
//
// This interrupt controls the persistence of vision
// The faster this runs the less the LED's flicker
//
// !!!WARNING!!!
// Run this interrupt as slow as possible
// Running this interrupt to fast will result in artifacting on the next level
ISR(TIMER5_COMPA_vect)
{
	//turn previous layer off
	prevLayer = layer;
	latchLayer(prevLayer, true);
	
	
	if (layer == 7)
	{
		layer = 0;
	}
	else
	{
		layer++;
	}
	
	//set data latches 
	for (int xxx = 0 ; xxx < cubeSize+1 ; xxx++)            
	{
		temp = cubeRowToInt(layer,xxx);
		latchData(xxx, temp);
	}
	
	
	//turn current layer on
	latchLayer(layer, false);
}




//========================================================================
//=============================EFFECT FUNCTIONS===========================
//========================================================================


void Effect_rain(int iterations, int itterationDelay)
{
	clearAll();
	
	//
	int xx = 0;
	int yy = 0;
	
	//   Cord sys
	//
	//   Y
	//   |
	//   :--X
	//  /
	// Z
	//
	// [Y][X][Z]
	for (i = 0 ; i < iterations ; i++)
	{
		// y axis, Top to bottom
		shift(0, 1);
		
		//
		setCubeLayer(0,7,false);
		
		//
		int tempVal = random(10,20);
		
		//
		for (j = 0 ; j < tempVal ; j++)
		{
			//
			xx = random(0, 8);
			yy = random(0, 8);
			
			//
			setPixel(7, xx, yy, true);
		}
		
		//Delay from
		delay(itterationDelay);
	}
	
	//
	for(i = 0 ; i < 8 ; i++)
	{
		shift(0, 1);
	}
	
	//
	setCubeLayer(0, 7, false);
}


//
void Effect_topDown(int iterations, int seperation, int axis, int itterationDelay)
{
	clearAll();
	//
	//y-axis
	if (axis == 0)
	{
		setCubeLayer(0, 7, true);
		//setPixel(7,0,0,true);
	}
	
	//x-axis
	if (axis == 1)
	{
		setCubeLayer(1, 7, true);
	}
	
	//z-axis
	if (axis == 2)
	{
		setCubeLayer(2, 7, true);
	}
	//
	delay(itterationDelay);
	
	
	for (j = 0 ; j < iterations ; j++)
	{
		
		for (i = 0 ; i < seperation ; i++)
		{
			//y-axis
			if (axis == 0)
			{
				shift(axis,1);
				setCubeLayer(0, 7, false);
			}
			
			//x-axis
			if (axis == 1)
			{
				shift(axis,1);
				setCubeLayer(1, 7, false);
			}
			
			//z-axis
			if (axis == 2)
			{
				shift(axis,1);
				setCubeLayer(2, 7, false);
			}
			
			if (i == (seperation-1))
			{
				//y-axis
				if (axis == 0)
				{
					setCubeLayer(0, 7, true);
				}
				
				//x-axis
				if (axis == 1)
				{
					setCubeLayer(1, 7, true);
				}
				
				//z-axis
				if (axis == 2)
				{
					setCubeLayer(2, 7, true);
				}
			}
			
			delay(itterationDelay);
		}
		
	}
	
	if (seperation < 8)
	{
		for (i = 8 - seperation ; i < cubeSize ; i++)
		{
			//y-axis
			if (axis == 0)
			{
				shift(axis,1);
			}
			
			//x-axis
			if (axis == 1)
			{
				shift(axis,1);
			}
			
			//z-axis
			if (axis == 2)
			{
				shift(axis,1);
			}
		}
	}
	
}


//
void Effect_textScroll(int iterations, String inputstr, int delayTime)
{
	clearAll();
	//
	String inputString = inputstr;
	int stringLength = inputstr.length();
	
	//
	resetTextPath();
	
	//
	for (i = 0; i < iterations ; i++)
	{
		
		//
		for (j = 0 ; j < stringLength ; j++)
		{
			//
			char currentChr = inputString[j];
			unsigned char chrPattern[5] = {0};
			getCharPattern(currentChr, chrPattern);
			
			//
			for (k = 0 ; k < 5 ; k++)
			{
				//
				addChrToPath(chrPattern[k], 0);
				addPathToCube();
				delay(delayTime);
				incrementPath();
				addPathToCube();
			}
			
			//Add space between characters
			delay(delayTime);
			incrementPath();
			addPathToCube();
		}
		
		for (int k = 0 ; k < textPathLength - 4 ; k++)
		{
			delay(delayTime);
			incrementPath();
			addPathToCube();
		}
		
		
	}
}


//
// Cord sys
//
//   Y
//   |
//   :--X
//  /
// Z
//
// [Y][X][Z]
void Effect_ShootText(int iterations, String inputStr, int axis, int delayTime, int shiftDelayTime)
{
	//
	clearAll();
	
	String inputString = inputStr;
	int stringLength = inputStr.length();
	
	for (i = 0 ; i < iterations ; i++)
	{
		for (j = 0 ; j < stringLength ; j++)
		{
			//get the current char to be displayed
			char currentChr = inputString[j];
			
			//Clear the Top letter still in the cube
			setCubeLayer(axis, 7, false);
			
			//add text to the cube
			addTextToCubeLayer(currentChr, axis);
			
			//delay for position 0
			delay(shiftDelayTime);
			
			//move text through the cube
			for (k = 1 ; k < cubeSize ; k++)
			{
				shift(axis, -1);
				setCubeLayer(axis, k-1, false);
				delay(shiftDelayTime);
			}
			
			delay(delayTime);
		}
	}
	
}



//
void Effect_UpDown_Suspend(int iterations, int SmallDelayTime, int LongDelayTime)
{
	
	unsigned char Desiredposition[64];
	int warpfactor;
	
	for (warpfactor = 0 ; warpfactor < iterations ; warpfactor++)
	{
		clearAll();
		
		for (i = 0; i < 64; i++)
		{
			Desiredposition[i] = random(0,8);
		}
		
		//fill layer 0
		setCubeLayer(0, 0, true);
		delay(SmallDelayTime * 2);
		
		//work up with delayTime, stop moving up when current = desired
		for (i = 1 ; i < cubeSize ; i++)
		{
			for (j = 0 ; j < cubeSize ; j++)
			{
				for (k = 0 ; k < cubeSize ; k++)
				{
					if (i <= Desiredposition[(j*8)+k])
					{
						setPixel(i, j ,k, true);
					}
					else
					{
						setPixel(i, j, k, false);
					}
					
					if (i-1 != Desiredposition[(j*8)+k])
					{
						setPixel(i-1, j, k, false);
					}
				}
			}
			delay(SmallDelayTime);
		}
		
		//long delay
		delay(LongDelayTime);
		
		//
		for (i = 1 ; i < cubeSize ; i++)
		{
			for (j = 0 ; j < cubeSize ; j++)
			{
				for (k = 0 ; k < cubeSize ; k++)
				{
					if (i < Desiredposition[(j*8)+k])
					{
						setPixel(i, j ,k, false);
					}
					else
					{
						setPixel(i, j, k, true);
					}
					
					setCubeLayer(0,i-1,false);
				}
			}
			delay(SmallDelayTime);
		}
		
		//long delay
		delay(LongDelayTime);
		
		//work up with delayTime, stop moving up when current = desired
		for (i = 6 ; i >= 0 ; i--)
		{
			for (j = 0 ; j < cubeSize ; j++)
			{
				for (k = 0 ; k < cubeSize ; k++)
				{
					if (i >= Desiredposition[(j*8)+k])
					{
						setPixel(i, j ,k, true);
					}
					else
					{
						setPixel(i, j, k, false);
					}
					
					if (i+1 != Desiredposition[(j*8)+k])
					{
						setPixel(i+1, j, k, false);
					}
				}
			}
			
			//
			delay(SmallDelayTime);
		}
		
		//long delay
		delay(LongDelayTime);

		//
		for (i = 6 ; i >= 0 ; i--)
		{
			for (j = 0 ; j < cubeSize ; j++)
			{
				for (k = 0 ; k < cubeSize ; k++)
				{
					if (i > Desiredposition[(j*8)+k])
					{
						setPixel(i, j ,k, false);
					}
					else
					{
						setPixel(i, j, k, true);
					}
					
					setCubeLayer(0,i+1,false);
				}
			}
			delay(SmallDelayTime);
		}
		
		//long delay
		delay(LongDelayTime);
	}
	
}

//
void Effect_ShootRandPixel(int iterations, int axis, int delayTimeSmall, int delayTimeLarge)
{
	//Clear any previous data in the cube array
	clearAll();
	
	//
	int shootDirec = 0;
	int xdir = 0;
	int ydir = 0;
	
	//Set the sides to true, Dependant on axis ? dont like the look initial
	//setCubeLayer(axis,0,true);
	//setCubeLayer(axis,7,true);
	
	//
	for (int i = 0 ; i < iterations ; i++)
	{
		shootDirec = random(0,2);
		
		if (shootDirec == 1)
		{
			xdir = random(0,8);
			ydir = random(0,8);
			
			for (j = 1 ; j < cubeSize ; j++)
			{
				if (axis == 0)
				{
					setPixel(j, xdir, ydir, true);
					setPixel(j-1, xdir, ydir, false);
					delay(delayTimeSmall);
				}
				
				if (axis == 1)
				{
					setPixel(xdir, j, ydir, true);
					setPixel(xdir, j-1, ydir, false);
					delay(delayTimeSmall);
				}
				
				if (axis == 2)
				{
					setPixel(xdir, ydir, j, true);
					setPixel(xdir, ydir, j-1, false);
					delay(delayTimeSmall);
				}
			}
		}
		else
		{
			xdir = random(0,8);
			ydir = random(0,8);
			
			for (j = 6 ; j >= 0 ; j--)
			{
				if (axis == 0)
				{
					setPixel(j, xdir, ydir, true);
					setPixel(j+1, xdir, ydir, false);
					delay(delayTimeSmall);
				}
				
				if (axis == 1)
				{
					setPixel(xdir, j, ydir, true);
					setPixel(xdir, j+1, ydir, false);
					delay(delayTimeSmall);
				}
				
				if (axis == 2)
				{
					setPixel(xdir, ydir, j, true);
					setPixel(xdir, ydir, j+1, false);
					delay(delayTimeSmall);
				}
			}
		}
		
		//
		delay(delayTimeLarge);
	}
}

//
void Effect_BoxShrinkGrow(int iterations, int Dir, int type, int DelayTime)
{
	clearAll();
	
	//
	int iter = 0;
	//
	for (iter = 0 ; iter < iterations; iter++)
	{
		//Centre
		for (j = 3 ; j >= 0 ; j--)
		{
			clearAll();
			DrawWireframe(3-j, 3-j, 3-j, 4+j, 4+j, 4+j);
			delay(DelayTime);
		}
		
		for (j = 0 ; j < 4 ; j++)
		{
			clearAll();
			DrawWireframe(3-j, 3-j, 3-j, 4+j, 4+j, 4+j);
			delay(DelayTime);
		}
		
		
		//0,0,0
		for (j = 7 ; j >= 1 ; j--)
		{
			clearAll();
			DrawWireframe(0, 0, 0, j, j, j);
			delay(DelayTime);
		}
		
		for (j = 1 ; j < 7 ; j++)
		{
			clearAll();
			DrawWireframe(0, 0, 0, j, j, j);
			delay(DelayTime);
		}
		
		//0,7,7
		for (j = 7 ; j >= 1 ; j--)
		{
			clearAll();
			DrawWireframe(7, 0, 7, 7-j, j, 7-j);
			delay(DelayTime);
		}
		
		for (j = 1 ; j < 7 ; j++)
		{
			clearAll();
			DrawWireframe(7, 0, 7, 7-j, j, 7-j);
			delay(DelayTime);
		}
		
		//7,0,7
		for (j = 7 ; j >= 1 ; j--)
		{
			clearAll();
			DrawWireframe(0, 7, 7, j, 7-j, 7-j);
			delay(DelayTime);
		}
		
		for (j = 1 ; j < 7 ; j++)
		{
			clearAll();
			DrawWireframe(0, 7, 7, j, 7-j, 7-j);
			delay(DelayTime);
		}
		
		//7,7,0
		for (j = 7 ; j >= 1 ; j--)
		{
			clearAll();
			DrawWireframe(7, 7, 0, 7-j, 7-j, j);
			delay(DelayTime);
		}
		
		for (j = 1 ; j <= 7 ; j++)
		{
			clearAll();
			DrawWireframe(7, 7, 0, 7-j, 7-j, j);
			delay(DelayTime);
		}
	}
}

//
void Effect_Fireworks(int iterations, int n, int delayTime)
{
	clearAll();

	int ilk, f, e;

	float origin_x = 3;
	float origin_y = 3;
	float origin_z = 3;

	int rand_y, rand_x, rand_z;

	float slowrate, gravity;

	// Particles and their position, x,y,z and their movement, dx, dy, dz
	float particles[n][6];

	for (ilk = 0; ilk < iterations; ilk++)
	{

		origin_x = rand() % 4;
		origin_y = rand() % 4;
		origin_z = rand() % 2;
		origin_z += 5;
		origin_x += 2;
		origin_y += 2;

		// shoot a particle up in the air
		for (e = 0; e < origin_z; e++)
		{
			setPixel(e, origin_x, origin_y, true);
			delay(160 * e);
			clearAll();
		}

		// Fill particle array
		for (f = 0; f < n; f++)
		{
			// Position
			particles[f][0] = origin_x;
			particles[f][1] = origin_y;
			particles[f][2] = origin_z;
			
			rand_x = rand()%200;
			rand_y = rand()%200;
			rand_z = rand()%200;

			// Movement
			particles[f][3] = 1 - (float)rand_x / 100; // dx
			particles[f][4] = 1 - (float)rand_y / 100; // dy
			particles[f][5] = 1 - (float)rand_z / 100; // dz
		}

		// explode
		for (e = 0; e < 25; e++)
		{
			slowrate = 1 + tan((e + 0.1) / 20) * 10;
			
			gravity = tan((e + 0.1) / 20) / 2;

			for (f = 0; f < n; f++)
			{
				particles[f][0] += particles[f][3] / slowrate;
				particles[f][1] += particles[f][4] / slowrate;
				particles[f][2] += particles[f][5] / slowrate;
				particles[f][2] -= gravity;

				setPixel(particles[f][2],particles[f][0],particles[f][1],true);


			}

			delay(delayTime);
			clearAll();
		}

	}

}



//========================================================================
//===========================BACKGROUND FUNCTIONS=========================
//========================================================================


//----------------------------GENERAL USE-----------------------------


//Temporary function for getting serial data back
void serialCheck()
{
	
	if(Serial.available() > 0)
	{
		int z = Serial.read();
		
		if (z-48 == 0)
		{
			for (int fuckyou = 0 ; fuckyou < cubeSize ; fuckyou++)
			{
				Serial.println(fuckyou);
			}
		}
		
	}
	
}



//   Cord sys
//
//   Y
//   |
//   :--X
//  /
// Z
//
// Cube[Y][X][Z]
//
// Convert a row of data in the cube array to a BCD number
int cubeRowToInt(int layer, int Row)
{
	int tempV = 0;
	
	if (cube[layer][0][Row]){tempV = tempV + 1;}
	if (cube[layer][1][Row]){tempV = tempV + 2;}
	if (cube[layer][2][Row]){tempV = tempV + 4;}
	if (cube[layer][3][Row]){tempV = tempV + 8;}
	if (cube[layer][4][Row]){tempV = tempV + 16;}
	if (cube[layer][5][Row]){tempV = tempV + 32;}
	if (cube[layer][6][Row]){tempV = tempV + 64;}
	if (cube[layer][7][Row]){tempV = tempV + 128;}
	
	return tempV;
}


//Check the Bus voltage
//If outside lim -> voltage error
void VoltageCheck()
{
	temp = analogRead(A7);
	
	if (temp <= battLim)
	{
		VoltageError();
	}
}


//The power rail has a problem
//
//Turn off everything
//then flash an LED to indicate the error
void VoltageError()
{
	//Disable global interrupts
	cli();
	
	//
	while (1)
	{
		digitalWrite(LED, HIGH);
		delay(600);
		digitalWrite(LED, LOW);
		delay(600);
	}
}


//---------------------------- BACKGROUND EFFECT FUNCTIONS----------------------------------


#pragma region Effect Functions

//
void setPixel(int lay, int row, int pix, bool level)
{
	if (inRange(lay,row,pix))
	{
		cube[lay][row][pix] = level;
	}
}


//
boolean getPixel(int lay, int row, int pix)
{
	return cube[lay][row][pix];
}


//
boolean inRange(int lay, int row, int pix)
{
	if ((lay >= 0) && (lay < cubeSize) && (row >= 0) && (row < cubeSize) && (pix >= 0) && (pix < cubeSize))
	{
		return true;
	} 
	else
	{
		return false;
	}
}


// Set a line of pixels on the cube
//
// axis - 0(Y)
// layer - Row
// pos - Pix
//
// axis - 1(X)
// layer - Layer
// pos - Pix
//
// axis - 2(Z)
// layer - Layer
// pos - Row
void setLine(int axis, int layer, int pos, unsigned char data)
{
	//
	boolean tempBool = false;
	
	//
	for (int x = 0 ; x < cubeSize ; x++)
	{
		if (axis == 0)
		{
			tempBool = true && (data & 0x80);//added true &&
			data = data << 1;
			setPixel(x, layer, pos, tempBool);
		}
		
		if (axis == 1)
		{
			tempBool = (data & 0x01);
			data = data >> 1;
			setPixel(layer, x, pos, tempBool);
		}
		
		if (axis == 2)
		{
			tempBool = true && (data & 0x01);
			data = data >> 1;
			setPixel(layer, pos, x, tempBool);
		}	
	}	
}


//
// axis:
// 0 - Y axis
// 1 - X axis
// 2 - Z axis
void setCubeLayer(int axisa, int numa, boolean level)
{
	//
	for (int x = 0 ; x < cubeSize ; x++)
	{
		//
		for (int z = 0 ; z < cubeSize ; z++)
		{
			if (axisa == 0)
			{
				setPixel(numa, x, z, level);
			}
			
			if (axisa == 1)
			{
				setPixel(x, numa, z, level);
			}
			
			if (axisa == 2)
			{
				setPixel(x, z, numa, level);
			}
		}
	}
}


//
void setAll()
{
	for (int x = 0 ; x < cubeSize ; x++)
	{
		setCubeLayer(0, x, true);	
	}
}


//
void clearAll()
{
	for (int x = 0 ; x < cubeSize ; x++)
	{
		setCubeLayer(0, x, false);
	}
}


// Shift all the data in the cube array 
// 1 position along the selected axis in
// the indicated direction
//
// axis:
// 0 - Y axis
// 1 - X axis
// 2 - Z axis
//
// dir:
// 1 = 7 -> 0
// !1 = 0 -> 7
//
void shift(int axis, int dir)
{
	boolean tempPix = false;
	int layer1 = 0;
	int layer2 = 0;
	
	//
	for (int t = 1 ; t < cubeSize ; t++)
	{
		//
		if (dir == 1)
		{
			layer1 = t;
		} 
		else
		{
			layer1 = (7 - t);
		}
		
		//
		for (int y = 0 ; y < cubeSize ; y++)
		{
			//
			if (dir == 1)
			{
				layer2 = layer1 - 1;
			} 
			else
			{
				layer2 = layer1 + 1;
			}
			
			
			for (int u = 0 ; u < cubeSize ; u++)
			{
				if (axis == 0)
				{
					tempPix = getPixel(layer1,y,u);
					setPixel(layer2,y,u,tempPix);
				}
				
				if (axis == 1)
				{
					tempPix = getPixel(y,layer1,u);
					setPixel(y,layer2,u,tempPix);
				}
				
				if (axis == 2)
				{
					tempPix = getPixel(y,u,layer1);
					setPixel(y,u,layer2,tempPix);
				}
			}	
		}
	}
}

//
void CheckArgOrder(int in1, int in2, int *out1, int *out2)
{
	if (in1 > in2)
	{
		int tempVal = in1;
		in1 = in2;
		in2 = tempVal;
	}
	
	*out1 = in1;
	*out2 = in2;
}

//
void DrawWireframe(int XPos1, int YPos1, int ZPos1, int XPos2, int YPos2, int ZPos2)
{
	//
	CheckArgOrder(XPos1, XPos2, &XPos1, &XPos2);
	CheckArgOrder(YPos1, YPos2, &YPos1, &YPos2);
	CheckArgOrder(ZPos1, ZPos2, &ZPos1, &ZPos2);
	
	for (i = XPos1 ; i <= XPos2 ; i++)
	{
		setPixel(YPos1, i, ZPos1, true);
		setPixel(YPos1, i, ZPos2, true);
		setPixel(YPos2, i, ZPos1, true);
		setPixel(YPos2, i, ZPos2, true);
	}
	
	for (i = YPos1 ; i <= YPos2 ; i++)
	{
		setPixel(i, XPos1, ZPos1, true);
		setPixel(i, XPos1, ZPos2, true);
		setPixel(i, XPos2, ZPos1, true);
		setPixel(i, XPos2, ZPos2, true);
	}
	
	for (i = ZPos1 ; i <= ZPos2 ; i++)
	{
		setPixel(YPos1, XPos1, i, true);
		setPixel(YPos1, XPos2, i, true);
		setPixel(YPos2, XPos1, i, true);
		setPixel(YPos2, XPos2, i, true);
	}
	
}


//
void getCharPattern(char chr, unsigned char rtnChr[5])
{
	chr -= 32;
	int addr = (int)chr * 5; 
	int z = 0;
	
	for (int x = addr ; x < addr+5 ; x++)
	{
		rtnChr[z] = chracterStorageArray[x];
		z++;
	}
}

//
void resetTextPath()
{
	for (int juce = 0 ; juce < textPathLength ; juce ++)
	{
		textPath[juce] = 0;
	}
}


//
void addChrToPath(unsigned char inputChr, int pos)
{
	textPath[pos] = inputChr;
}


//
void incrementPath()
{
	for (int t = textPathLength - 2 ; t >= 0 ; t--)
	{ 
		textPath[t+1] = textPath[t];
	}
	textPath[0] = 0;
}


//
//
//  00-01-02-03-04-05-06-07
//  08-09-10-11-12-13-14-15
//  16-17-18-19-20-21-22-23
//  24-25-26-27-28-29-30-31
//  32-33-34-35-36-37-38-39
//  40-41-42-43-44-45-46-47
//  48-49-50-51-52-53-54-55
//  56-57-58-59-60-61-62-63
//                 <-----START
//
//   Cord sys
//   
//   Y
//   |
//   :--X
//  /
// Z
//
// [Y][X][Z]
//
// setLine(Axis, X, Z, Chr);
void addPathToCube()
{
	setLine(0,7,7,textPath[6]);//63
	setLine(0,6,7,textPath[7]);//62
	setLine(0,5,7,textPath[8]);//61
	setLine(0,4,7,textPath[9]);//60
	setLine(0,3,7,textPath[10]);//59
	setLine(0,2,7,textPath[11]);//58
	setLine(0,1,7,textPath[12]);//57
	setLine(0,0,7,textPath[13]);//56
	
	setLine(0,0,6,textPath[14]);//48
	setLine(0,0,5,textPath[15]);//40
	setLine(0,0,4,textPath[16]);//32
	setLine(0,0,3,textPath[17]);//24
	setLine(0,0,2,textPath[18]);//16
	setLine(0,0,1,textPath[19]);//08
		
	setLine(0,0,0,textPath[20]);//00
	setLine(0,1,0,textPath[21]);//01
	setLine(0,2,0,textPath[22]);//02
	setLine(0,3,0,textPath[23]);//03
	setLine(0,4,0,textPath[24]);//04
	setLine(0,5,0,textPath[25]);//05
	setLine(0,6,0,textPath[26]);//06
	setLine(0,7,0,textPath[27]);//07
	
	setLine(0,7,1,textPath[28]);//15
	setLine(0,7,2,textPath[29]);//23
	setLine(0,7,3,textPath[30]);//31
	setLine(0,7,4,textPath[31]);//39
	setLine(0,7,5,textPath[32]);//47
	setLine(0,7,6,textPath[33]);//55
}



// axis:
// 0 - Y axis
// 1 - X axis
// 2 - Z axis
//
void addTextToCubeLayer(char inputChar, int axis)
{
	unsigned char chrPattern[5] = {0};
	getCharPattern(inputChar, chrPattern);
	
	int loopCount = 0;
	
	switch(axis)
	{
		// y-axis
		case 0:
			for (loopCount = 0 ; loopCount < 5 ; loopCount++)
			{
				setLine(2, 0, loopCount + 1, chrPattern[loopCount]);	
			}
		break;
		
		//x-axis
		case 1:
			for (loopCount = 0 ; loopCount < 5 ; loopCount++)
			{
				setLine(0, 0, loopCount +1 , chrPattern[ 4 - loopCount]);
			}
		break;
		
		//z-axis
		case 2:
			for (loopCount = 0 ; loopCount < 5 ; loopCount++)
			{
				setLine(0, loopCount + 1, 0, chrPattern[loopCount]);
			}
		break;
		
		default:
		//FUUUCCCCKKKKK
		break;
	}
}


#pragma endregion Effect Functions

void Function_Demo()
{
	//Effect functions
	Effect_rain(120,100);
	Effect_topDown(5,3,0,500);
	Effect_ShootText(1,"9876543210", 2, 600, 50);
	Effect_Fireworks(10, 20, 200);
	Effect_textScroll(2,"This is An LED Cube /?@#$", 160);
	Effect_UpDown_Suspend(5,120,1000);
	Effect_ShootRandPixel(40,0,50,1);
	Effect_ShootText(1, "ABCDEFGHIJ", 2, 180, 50);
	Effect_ShootText(1, "KLMNOPQRSTUVWX", 1, 180, 50);
	Effect_ShootText(1, "YZ1234567890", 0, 180, 50);
	Effect_BoxShrinkGrow(2, 2, 2, 200);
}

