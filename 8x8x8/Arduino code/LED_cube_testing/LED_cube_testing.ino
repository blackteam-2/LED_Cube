/*
J McKenna
8x8x8 LED Cube

//


Software
Program Version: 0.1
Last Update: 20/06/2014


Revision Notes:
0.1-Created program


Hardware
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

//Pin definitions
const int LED = 13;

const int D0 = 52;
const int D1 = 38;
const int D2 = 42;
const int D3 = 40;
const int D4 = 44;
const int D5 = 46;
const int D6 = 50;
const int D7 = 48;

const int A = 5;
const int B = 4;
const int C = 3;
const int E = 6;

const int RD0 = 37;
const int RD1 = 31;
const int RD2 = 29;
const int RD3 = 22;
const int RD4 = 25;
const int RD5 = 27;
const int RD6 = 33;
const int RD7 = 35;
const int RE = 39;

//Cube array [Layer][Row][Bit]|[Y][X][Z]
boolean cube[8][8][8] = {0}; 
int layer = 0;

//General Use Variables
int i = 0;
int j = 0;
int k = 0;
int temp = 0;



//========================================================================
//===================================SETUP================================
//========================================================================

//
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
  
  //
  //15625 - 1Hz
  //781 - 20Hz
  //500 - 31.25Hz
  layerUpdateTimer(15625);
  
  //Debugging
  Serial.begin(9600);
}


//========================================================================
//==================================MAIN LOOP=============================
//========================================================================

//
void loop()
{
  //Serial
  if(Serial.available() > 0)
  {
    i = Serial.read();
    if((i >= 0) || (i <= 9))
    {
      digitalWrite(LED, HIGH);
	  
      //latchData(1,i-48);
	  latchData(i-48,i-48);
      Serial.print(i);
    }
    delay(500);
    digitalWrite(LED, LOW);
  }
  
  
  
}


//========================================================================
//================================SET LATCHES============================
//========================================================================

//
void latchData(int multiplex, int Data)
{
 digitalWrite(E, HIGH);
 delay(1);
 setData(Data);
 delay(1);
 setMultiplexer(multiplex);
 delay(1000);
 digitalWrite(E,LOW);
}

//
void latchLayer(int Data)
{
	digitalWrite(RE, HIGH);
	delay(1);
	latchLayer(Data);
	delay(1);
	digitalWrite(RE, LOW);
}


//========================================================================
//==================================SET I/O===============================
//========================================================================

//
void setMultiplexer(int pos)
{
  digitalWrite(A, HIGH && (pos & 0x01));
  digitalWrite(B, HIGH && (pos & 0x02));
  digitalWrite(C, HIGH && (pos & 0x04));
}

//
void setData(int data)
{
  digitalWrite(D0, HIGH && (data & 0x01));
  digitalWrite(D1, HIGH && (data & 0x02));
  digitalWrite(D2, HIGH && (data & 0x04));
  digitalWrite(D3, HIGH && (data & 0x08));
  digitalWrite(D4, HIGH && (data & 0x10));
  digitalWrite(D5, HIGH && (data & 0x20));
  digitalWrite(D6, HIGH && (data & 0x40));
  digitalWrite(D7, HIGH && (data & 0x80));
}

//
void setLayer(int data)
{
	digitalWrite(RD0, HIGH && (data & 0x01));
	digitalWrite(RD1, HIGH && (data & 0x02));
	digitalWrite(RD2, HIGH && (data & 0x04));
	digitalWrite(RD3, HIGH && (data & 0x08));
	digitalWrite(RD4, HIGH && (data & 0x10));
	digitalWrite(RD5, HIGH && (data & 0x20));
	digitalWrite(RD6, HIGH && (data & 0x40));
	digitalWrite(RD7, HIGH && (data & 0x80));
}


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
void layerUpdateTimer(int reload)
{
	if (reload == 0)
	{
		TCCR5B = 0x00;
		return;
	}
	
	TCCR5B |= (WGM52);
	OCR5A = reload;
	TIMSK5 |= (OCIE5A);
	TCCR5B |= (CS50) | (1 << CS52);//clk/2014
	
}

//
ISR(TIMER5_COMPA_vect)
{
	//turn prev layer off
	
	
	
	//set data latches 
	for (i = 0 ; i < 8 ; i++)
	{
		temp = cubeRowToInt(layer,i);
		latchData(i, temp);
	}
	
	
	//turn layer on
	
	
	
}

int cubeRowToInt(int layer, int Row)
{
	temp = 0;
	if (cube[]){}
	return temp;
}