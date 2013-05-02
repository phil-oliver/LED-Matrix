#include <crc_table.h>

#define latchPin    10 // Enable shift register after data refresh
#define PIN_SCK     13 // SPI clock
#define PIN_MISO    12 // SPI data input
#define PIN_MOSI    11 // SPI data output

#define NUM_ROWS  8
#define NUM_COLS  16

// forward definitions for ADC rate setting
#define FASTADC 1
#ifndef cbi
  #define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
  #define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


void n64Setup();

// buffers for the LEDs
byte buffer_r[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // buffer for the red LEDs (8 bytes correspond to 8 columns)
byte buffer_g[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // buffer for the green LEDs (8 bytes correspond to 8 columns)
byte buffer_b[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // buffer for the blue LEDs (8 bytes correspond to 8 columns)






// setup procedure prepares the Î¼-Controller prior to termination - The output pins are defined as digital OUTPUTS
void setup()   
{               
  // pins to input and output set and assigned a start value
  pinMode(latchPin, OUTPUT);
  pinMode(PIN_SCK,  OUTPUT);       
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_MISO,  INPUT);
  digitalWrite(latchPin,  LOW);
  digitalWrite(PIN_SCK,   LOW);
  digitalWrite(PIN_MOSI,  LOW);
  digitalWrite(PIN_MISO, HIGH);
  
  // prepare SPI Communication
  SPCR = B01110000; 
  
  // SPI Control Register (SPCR)
  
  // | 7    | 6    | 5    | 4    | 3    | 2    | 1    | 0    |
  // | SPIE | SPE  | DORD | MSTR | CPOL | CPHA | SPR1 | SPR0 |

  // SPIE - Enables the SPI interrupt when 1
  // SPE  - Enables the SPI when 1
  // DORD - Sends data least Significant Bit First when 1, most Significant Bit first when 0
  // MSTR - Sets the Arduino in master mode when 1, slave mode when 0
  // CPOL - Sets the data clock to be idle when high if set to 1, idle when low if set to 0
  // CPHA - Samples data on the falling edge of the data clock when 1, rising edge when 0
  //SPR1 and SPR0 - Sets the SPI speed, 00 is fastest (4MHz) 11 is slowest (250KHz)
  
  SPSR = B00000000; // SPI Status Register
  
  // ADC clock speed increase  
  #if FASTADC
    sbi(ADCSRA,ADPS2);
    cbi(ADCSRA,ADPS1);
    cbi(ADCSRA,ADPS0);
  #endif
  
  Serial.begin(9600);
  // snake variables
  srand(millis());  // seeding random nums generator for apple display
  int initialsnakeLen = 4; // change this value with whatever length you want
  buildInitialSnake (initialsnakeLen);  // creating small snake to start with
  displaySnake();
  
  // initialize serial read of the N64 controller data
  n64Setup();
}

String str = "Hello, World!";
int choice = 0; 
void loop()
{
  if(choice == 4)
  {
     choice = 0; 
  }
  switch (choice) {
      
    case 0:
      snake(100);
      choice++;
      break;   
      
    case 1:
      colorShift(3000, 50);
      choice++;
      break;
      
     case 3:
       Display_String(50, str, str.length());
       choice++;
       break;      
  }
}

