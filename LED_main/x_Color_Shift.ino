void colorShift(unsigned long duration, int pause)
{
  byte LUT[40];
  byte Frame[NUM_ROWS * NUM_COLS];
  byte counter = 0;
  unsigned long endTime  = 0;
  
  duration = millis() + (duration * 10000);
  
  // create color look up table
  for (byte k = 0; k < 8; k++) {
    LUT[k] = color(7, k, 0);  // R: 7     ;  G: 0-->7 ; B: 0 
    LUT[8 + k] = color(7 - k, 7, 0);  // R: 7-->0 ;  G: 7     ; B: 0
    
    if (k<4) {
      LUT[16 + k] = color(0, 7, k);
    } // R: 0     ;  G: 7     ; B: 0-->3
              
    LUT[20 + k] = color(0, 7 - k, 3);  // R: 0     ;  G: 7-->0 ; B: 3
    LUT[28 + k] = color(k, 0, 3);  // R: 0-->7 ;  G: 0     ; B: 3
    
    if (k<4) {
      LUT[36 + k] = color(7, 0, 3 - k);
    } // R: 7     ;  G: 0     ; B: 3-->0
  } 
  
  
    
  do {
    
    if (counter == 40) {
      counter = 0;
    }
  
    for (byte col = 0; col < NUM_COLS; col++) {
      for (byte row = 0; row < NUM_ROWS; row++) {
        if (counter + col < 40) {
          Frame[row * NUM_COLS + col] = LUT[counter + col];
        }
        else {
          Frame[row * NUM_COLS + col] = LUT[counter + col - 40];
        }
      } 
    }
        
    endTime = millis() + ((unsigned long) pause);
    displayFramePWM(Frame, endTime);
    counter++;
  

  } while (millis() <= duration);
}


