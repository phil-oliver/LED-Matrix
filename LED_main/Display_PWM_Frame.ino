#define RED  color(7, 0, 0)
#define BLUE  color(0, 0, 3)
#define GREEN  color(0, 7, 0)

#define PURPLE  color(7, 0, 7)
#define CYAN  color(0, 7, 7)
#define YELLOW  color(7, 7, 0)

const byte COLORS[] = { RED, BLUE, GREEN, PURPLE, CYAN, YELLOW };

void displayFramePWM(byte frame[NUM_ROWS * NUM_COLS], unsigned long endTime) {  
  byte SR[] = {0,0,0,0,0,0}; // buffer for the 6 shift registers

  byte back =  0; // initialize variable for the return value of SPI
  byte index = 0; // LED index

  const byte LUT_rg[] = {B11111100, B10000000, B11000000, B10100000, B11010000, B10001010, B11100100, B10000000, B11010000, B10100000, B11001000, B10000000, B11110100, B10000000, B11000000, B10101010, B11010000, B10000000, B11100000, B10000000};
  const byte LUT_bl[] = {B00001110, B00001000, B00001100, B00001000, B00001110, B00001000, B00001100, B00001000, B00001110, B00001000, B00001100, B00001000, B00001110, B00001000, B00001100, B00001000, B00001110, B00001000, B00001100, B00001000};

  do
  {      
    // cylcle through rows, shifting them into shift registers, and displaying them       
    for(byte j=0; j<20; j++)
    {             
      for(byte column=0; column < NUM_ROWS; column++) // For all 8 rows
      {           
        // the RGB row (3 diodes * 16 columns = 48 bits) is divided into 6 8-bit shift registers
        index = NUM_COLS*column;
        
        // populate red shift registers with data
        for(byte k = 0; k < 8 ; k++)
        {
          bitWrite(SR[0], k, bitRead(LUT_rg[j], (byte)((frame[index + k] >> 5))));
        }
        for(byte k = 0; k < 8 ; k++)
        {
          bitWrite(SR[1],7 - k, bitRead(LUT_rg[j], (byte)((frame[index + 8 + k] >> 5))));
        }
        
        //populate green shift registers with data
        for(byte k = 0; k < 8 ; k++)
        {
          bitWrite(SR[2], k, bitRead(LUT_rg[j], (byte)(((byte)(frame[index + k] << 3)) >> 5) ));
        } 
        for(byte k = 0; k < 8 ; k++)
        {
          bitWrite(SR[3], 7 - k, bitRead(LUT_rg[j], (byte)(((byte)(frame[index + 8 + k] << 3)) >> 5)));
        }
        
        // poplulate blue shift registers with data
        for(byte k = 0; k < 8 ; k++)
        {
          bitWrite(SR[4], k, bitRead(LUT_bl[j], (byte)(((byte)(frame[index + k] << 6)) >> 6)));
        }
        for(byte k = 0; k < 8 ; k++)
        {
          bitWrite(SR[5], 7 - k, bitRead(LUT_bl[j], (byte)(((byte)(frame[index + 8 + k] << 6)) >> 6)));
        }     

        byte row = B0000000;
        
        // set the current row (this will be interpreted as connecting that row to ground)
        bitSet(row, column);

        // shift row data into shift registers   
        EnableSPI();    
        
        digitalWrite(latchPin, LOW); // latchPin to ground
        
        back = SendRecSPI(row); // byte for SR6 slide out
        
        // shift out data from RGB buffers
        for(byte l = 6; l > 0; l--)
        {
          back = SendRecSPI(SR[l - 1]);
        }
        
        digitalWrite(latchPin, HIGH); // latchPin to HIGH, thus asserting all shift registers
        DisableSPI();

        // this eliminates flicker that would otherwise occur here
        delayMicroseconds(10);       
      }   
    }
    
  } 
  while (millis() <= endTime); // repeat the current frame until endTime is reached

  EnableSPI();       
  digitalWrite(latchPin, LOW);                                                
  for (byte l = 0; l < 6; l++)
  {
    back = SendRecSPI(0);
  } 
  digitalWrite(latchPin, HIGH);                         
  DisableSPI();  
}

// definition of procedures for the SPI
void EnableSPI(void)
{
  SPCR |= 1 << SPE;
}
void DisableSPI(void)
{
  SPCR &= ~(1 << SPE);
}
void WaitSPIF(void)
{
  while (! (SPSR & (1 << SPIF)))
  {
    continue;
  }
}

byte SendRecSPI(byte Dbyte) 
{        
  SPDR = Dbyte;
  WaitSPIF();
  return SPDR;
}

//Procedure calculates the color
byte color(byte r, byte g, byte b)
{
  return 32*r+4*g+b;
}






