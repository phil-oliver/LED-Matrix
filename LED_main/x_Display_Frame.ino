void displayFrame (byte buffer_r[], byte buffer_g[], byte buffer_b[], unsigned long end_time)
{
   byte SR[] = {0,0,0,0,0,0}; //Buffer for the 6 shift registers
   
   byte back = 0; // the return value of the SPI call
    
   do{                   
        for (byte column=0; column < NUM_ROWS; column++) // For all 8 rows
        {
          for(byte k = 0; k < 8 ; k++)
          {
            bitWrite(SR[0], k, buffer_r[k] & (1 << column));
          } // SR1 contains 8x red        
          for(byte k = 0; k < 8 ; k++)
          {
            bitWrite(SR[1], 7 - k, buffer_r[k + 8] & (1 << column));
          } // SR2 contains 8x red
          
          for(byte k = 0; k < 8 ; k++)
          {
            bitWrite(SR[2], k, buffer_g[k] & (1 << column));
          } // SR3 contains 8x green
          for(byte k = 0; k < 8 ; k++)
          {
            bitWrite(SR[3], 7 - k, buffer_g[k + 8] & (1 << column));
          } // SR4 contains 8x green
          
          for(byte k = 0; k < 8 ; k++)
          {
            bitWrite(SR[4], k, buffer_b[k] & (1 << column));
          } // SR5 contains 8x blue        
          for(byte k = 0; k < 8 ; k++)
          {
            bitWrite(SR[5], 7 - k, buffer_b[k + 8] & (1 << column));
          } // SR6 contains 8x blue

          byte row = B0;
          // set the current row (this will be interpreted as asserting ground to that row)
          bitSet(row, column);
          
          // shift data out to the shift registers
          EnableSPI();          
          
          digitalWrite(latchPin, LOW);                          // LatchPin to ground, so LEDs are not blinking when "pushing through"
          back = SendRecSPI(row);                               // Byte for SR6 slide out
          for (byte j = 6; j > 0; j--){back = SendRecSPI(SR[j - 1]);} // 5 bytes for SR5 slide out to SR1
          digitalWrite(latchPin, HIGH);                         // LatchPin to HIGH, thus activating all SR
          
          DisableSPI();
                                   
          // this slight delay prevents fickering
          delayMicroseconds(300);         
        }
        
      } while (millis() <= end_time);     
}

