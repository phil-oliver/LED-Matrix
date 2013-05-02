// some test code and frame functions
byte Frame[NUM_ROWS * NUM_COLS];

void FrameBuffer() {
 
 unsigned long end_time  = 0;
 clearFrame();
 
 for (int i = 0; i < NUM_COLS; i++) {
  setPixel(i, 3, 32);
 }
  
 while (true) {
  end_time = millis() + 100;
  displayFramePWM(Frame, end_time);
 } 
 
}

void setPixel(int x, int y, int color)
{
  Frame[(y * NUM_COLS) + x] = 7*color;
}

void clearFrame()
{
   // clear the old frame
    for (byte i=0; i<NUM_ROWS * NUM_COLS; i++)
    {
      Frame[i] = 2;//(scale)*32;
    }
}


