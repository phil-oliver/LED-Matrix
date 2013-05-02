void displayString(int pause, String str, int strSize)
{

  byte string[strSize];
  
  Serial.println(str);
  
  for (int i = 0; i < strSize; i++) {
    string[i] = str[i];
  }
  
  byte color = 0;
    
  const int stringLength = strSize + 1;
  
  // a space is created that will hold the string. Each letter is 7x5 LEDs
  byte textfield_r[(stringLength * 6) + 24];
  byte textfield_g[(stringLength * 6) + 24];
  byte textfield_b[(stringLength * 6) + 24];

  byte currentLetter = 0;
  unsigned long end_time  = 0;

  for (int i = 0; i < ((stringLength * 6) + 24); i++)
  {
    textfield_r[i] = 0;
    textfield_g[i] = 0;
    textfield_b[i] = 0;
  }

  // clear the frame
  for (byte column=0; column < NUM_COLS; column++)
  {
    buffer_r[column] = 0;
    buffer_g[column] = 0;
    buffer_b[column] = 0;
  }    
    
  // create the text field
  for (int i=0; i<(stringLength-1); i++)
  {
    currentLetter = string[i];
    color = 0;
   
    for (byte j=0; j<5; j++)
    {
      switch (color)
      {
        case  0:
          // current letter is red
          textfield_r[NUM_COLS + (i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
          textfield_g[NUM_COLS + (i * 6) + j] = 0;
          textfield_b[NUM_COLS + (i * 6) + j] = 0;
        break;
  
        case  1:
          //The current letter is green
          textfield_r[NUM_COLS+(i * 6) + j] = 0;
          textfield_g[NUM_COLS+(i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
          textfield_b[NUM_COLS+(i * 6) + j] = 0;
        break;
        
        case  2:
          // current letter is blue
          textfield_r[NUM_COLS+(i * 6) + j] = 0;
          textfield_g[NUM_COLS+(i * 6) + j] = 0;
          textfield_b[NUM_COLS+(i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
        break;
        
        case  3:
          // current letter is red & green
          textfield_r[NUM_COLS+(i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
          textfield_g[NUM_COLS+(i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
          textfield_b[NUM_COLS+(i * 6) + j] = 0;
        break;
        
        case  4:
          // current letter is red & blue
          textfield_r[NUM_COLS+(i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
          textfield_g[NUM_COLS+(i * 6) + j] = 0;
          textfield_b[NUM_COLS+(i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
        break;
        
        case  5:
          // current letter is green & blue
          textfield_r[NUM_COLS+(i * 6) + j] = 0;
          textfield_g[NUM_COLS+(i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
          textfield_b[NUM_COLS+(i * 6) + j] = pgm_read_byte_near(characterSet + (((currentLetter - 32) * 5) + j));
        break;
      }      
    }
  }
  
  // the field containg the converted text is displayed on the LEDs
  for (int counter = 0; counter < ((stringLength * 5) + stringLength - 7 + NUM_COLS); counter++)
  {
    // create the new frame
    for (byte column= 0; column < NUM_COLS; column++)
    {
      buffer_r[column] = textfield_r[counter + column];
      buffer_g[column] = textfield_g[counter + column];
      buffer_b[column] = textfield_b[counter + column];
    }
    end_time = millis() + ((unsigned long)pause);
    
    // draw current frame
    displayFrame(buffer_r, buffer_g, buffer_b, end_time);
  }
  
}

