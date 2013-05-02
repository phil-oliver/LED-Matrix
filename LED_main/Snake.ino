#define N64_PIN 2
#define N64_PIN_DIR DDRD
#define N64_HIGH DDRD &= ~0x04
#define N64_LOW DDRD |= 0x04
#define N64_QUERY (PIND & 0x04)

#define LEFTBIT 1
#define RIGHTBIT 2
#define UPBIT 4
#define DOWNBIT 8
#define ABIT 16
#define STARTBIT 32
#define SELECTBIT 64

#define PLEFT (prev &LEFTBIT)
#define PRIGHT (prev & RIGHTBIT)
#define PDOWN (prev &DOWNBIT)
#define PUP (prev & UPBIT)
#define PA (prev & ABIT)
#define PSTART (prev & STARTBIT)
#define PSELECT (prev & SELECTBIT)

void N64_send(unsigned char *buffer, char length);
void N64_get();
void translate_raw_data();
void n64Setup();
void n64Loop();
boolean paused = false;

char N64_raw_dump[33]; // 1 received bit per byte

// 8 bytes of data that we get from the controller
struct {
    // bits: 0, 0, 0, start, y, x, b, a
    unsigned char data1;
    // bits: 1, L, R, Z, Dup, Ddown, Dright, Dleft
    unsigned char data2;
    char stick_x;
    char stick_y;
} N64_status;


struct Link {
  int _x, _y;
  struct Link *_next;
};

struct Link *head = NULL; 
struct Link *tail = NULL;

char prev = 0;
const int JS_x_pin = A1, JS_y_pin = A0;  // joystick pins
int direct = 1;  // up = 1, down = 2, left = 3, right = 44
int newDirect;  // new direction selected by user
boolean directionSet = false;  // tells us if snake's advancement direction was set for current iterration
int appleX = 12, appleY = 0;  // apple coordinates
unsigned long appleTimer = 0;  // will be started when apple appears and reset when eaten/not eaten in time
unsigned long oldMillis = 0, currMillis = 0;  // to control update time of the snake's links
int score = 0;

boolean dead = false;

int col = 0;


void snake(int pause)
{
  unsigned long end_time  = 0; 
  int gameSpeed = pause;  // you may set this to a smaller value for a more difficult game
  
  do
  {
    n64Loop();
    
    Serial.println((int)N64_status.data1);

     currMillis = millis();
     
     if (!directionSet)
     {
       setDirection ();
       direct = newDirect;
       directionSet = true;
     }
     if (currMillis - oldMillis >= gameSpeed)
     {
       oldMillis = currMillis;
       moveSnake(newDirect);
       displaySnake ();
       
       // we've moved the snake so direction needs to set again
       directionSet = false;
     }
     if (dead)
       endGame();
     if ((millis() / 1000) - (appleTimer) > 10 || (int)N64_status.data1 == 32)
     {  // this moves the apple if it wasn't eaten within 10 seconds
       appleTimer = millis () / 1000;
       appleX = random(NUM_COLS);
       appleY = random(NUM_ROWS);
     }
     
     end_time = millis() + 100;
     displayFramePWM(Frame, end_time);
     if (((int)N64_status.data2) == 16)
     {
       endGame();
     }
  } while (1);
}

void buildInitialSnake (int length) {
  int startX = 0, startY = 7;  // you may adjust these as you wish
  
  for(int i=0; i < length; i++, startX++)
  {
    addLink (startX, startY);
    if (i == 0)
    {
      tail = head;
    }
  }
}

void addLink (int x, int y)
{
  //  checkCollision (x, y);
  // allocating mem for new link
  struct Link *temp;
  temp = (struct Link*) malloc (sizeof(struct Link));
  if (temp == NULL) Serial.println ("Can't allocate memory");  
  
  // inserting new link into list ("snake")
  temp->_x = x;
  temp->_y = y;
  temp->_next = NULL;
  if (head != NULL)
    head->_next = temp;
  head = temp;
}

void moveSnake (char newDirect)
{
  int newX = head->_x, newY = head->_y;
  
  // setting new coordinates of next link of the snake:
  if (direct == newDirect) {
    if (direct == 4)
      newX += 1;
    else if (direct == 3)
      newX -= 1;
    else if (direct == 1)
      newY -= 1;
    else if (direct == 2)
      newY += 1;
  }
  else if((direct == 4 || direct == 3) && (newDirect == 1 || newDirect == 2))
  {// vertical movement turns into horizontal
    if (newDirect == 1)
      newY -= 1; 
    else if(newDirect == 2)
      newY += 1; 
  }
   else if((direct == 1 || direct == 2) && (newDirect == 3 || newDirect == 4))
   {// horizontal movement turns into vertical
    if (newDirect == 4)
      newX += 1; 
    else if(newDirect == 3)
      newX -= 1; 
  }

  // dealing with walls limitations
  if(newX > NUM_COLS - 1)
  {
    newX = 0;
  }
  if (newX < 0)
  {
    newX = NUM_COLS - 1;
  }
  if (newY > NUM_ROWS - 1)
  {
      newY = 0;
  }
  if (newY < 0)
  {
    newY = NUM_ROWS - 1;
  }
  
  dead = checkCollision (newX, newY);
  addLink (newX, newY);
  
  // checking if next movement will "eat" the apple and if it will sets new apple location
  if ((newX == appleX) && (newY == appleY))
  {  // if true then apple was eaten
     appleX = random(NUM_COLS);
     appleY = random(NUM_ROWS);
     appleTimer = millis() / 1000;
     score++;
  }

  // deleting tail link if an apple hasn't been eaten    
  else
  {  // apple was not eaten
    struct Link *temp = tail;
    tail = tail->_next;
    delete temp;
  }

}

void displaySnake ()
{
  clearFrame();
  struct Link *ptr;
  ptr = tail;
  while (ptr != NULL)
  {
    setPixel(ptr->_x, ptr->_y, 4);
    ptr = ptr->_next;
  }
  setPixel(appleX, appleY, 32);  
}

void setDirection ()
{
  int x, y;

  if (direct == 3 || direct == 4) {
    if (((int)N64_status.data1) == 8)  newDirect = 1;
    else if (((int)N64_status.data1) == 4) newDirect = 2;
    else newDirect = direct;
  }
  else if (direct == 1 || direct == 2) {
    if (((int)N64_status.data1) == 1)  newDirect = 4;
    else if (((int)N64_status.data1) == 2) newDirect = 3;
    else newDirect = direct;
  }
}


boolean checkCollision (int x, int y)
{
  struct Link *ptr;
  ptr = tail;
  while (ptr != NULL)
  {
    if (x == ptr->_x && y == ptr->_y)
    {
      return true;
    }
    ptr = ptr->_next;
  }
  return false;
}

void endGame ()
{
  clearFrame();
  // displaying end game message:
  endGameMsg();
  freeMemory ();  // freeing memory
  
  // resetting all variables & parameters
  
  head = NULL; 
  tail = NULL;
  score = 0;
  direct = 4;  
  directionSet = false; 
  appleX = 12, appleY = 0; 
  // building new initial snake
  delay (50);
  clearFrame();
  int initialSnakeLen = 4;
  buildInitialSnake (initialSnakeLen);
}

void freeMemory ()
{
  while (tail != NULL)
  {
    struct Link *ptr;
    ptr = tail;
    tail = tail->_next;
    delete ptr;
  }
}

void endGameMsg ()
{
  String str = String("SCORE: ") + score;
  Display_String(50, str, str.length());
}












void n64Loop()
{
    int i;
    unsigned char data, addr;

    // Command to send to the gamecube
    // The last bit is rumble, flip it to rumble
    // yes this does need to be inside the loop, the
    // array gets mutilated when it goes through N64_send
    unsigned char command[] = {0x01};
 
    // don't want interrupts getting in the way
    noInterrupts();
    // send those 3 bytes
    N64_send(command, 1);
    // read in data and dump it to N64_raw_dump
    N64_get();
    // end of time sensitive code
    interrupts();

    // translate the data in N64_raw_dump to something useful
    translate_raw_data();
}

void n64Setup()
{
  //Serial.begin(115200);



  // Communication with gamecube controller on this pin
  // Don't remove these lines, we don't want to push +5V to the controller
  digitalWrite(N64_PIN, LOW);  
  pinMode(N64_PIN, INPUT);


  // Initialize the gamecube controller by sending it a null byte.
  // This is unnecessary for a standard controller, but is required for the
  // Wavebird.
  unsigned char initialize = 0x00;
  noInterrupts();
  N64_send(&initialize, 1);

  // Stupid routine to wait for the gamecube controller to stop
  // sending its response. We don't care what it is, but we
  // can't start asking for status if it's still responding
  int x;
  for (x=0; x<64; x++) {
      // make sure the line is idle for 64 iterations, should
      // be plenty.
      if (!N64_QUERY)
          x = 0;
  }

  // Query for the gamecube controller's status. We do this
  // to get the 0 point for the control stick.
  unsigned char command[] = {0x01};
  N64_send(command, 1);
  // read in data and dump it to N64_raw_dump
  N64_get();
  interrupts();
  translate_raw_data();  
}



void translate_raw_data()
{
    // The get_N64_status function sloppily dumps its data 1 bit per byte
    // into the get_status_extended char array. It's our job to go through
    // that and put each piece neatly into the struct N64_status
    int i;
    memset(&N64_status, 0, sizeof(N64_status));
    // line 1
    // bits: A, B, Z, Start, Dup, Ddown, Dleft, Dright
    for (i=0; i<8; i++) {
        N64_status.data1 |= N64_raw_dump[i] ? (0x80 >> i) : 0;
    }
    // line 2
    // bits: 0, 0, L, R, Cup, Cdown, Cleft, Cright
    for (i=0; i<8; i++) {
        N64_status.data2 |= N64_raw_dump[8+i] ? (0x80 >> i) : 0;
    }
    // line 3
    // bits: joystick x value
    // These are 8 bit values centered at 0x80 (128)
    for (i=0; i<8; i++) {
        N64_status.stick_x |= N64_raw_dump[16+i] ? (0x80 >> i) : 0;
    }
    for (i=0; i<8; i++) {
        N64_status.stick_y |= N64_raw_dump[24+i] ? (0x80 >> i) : 0;
    }
}


/**
 * This sends the given byte sequence to the controller
 * length must be at least 1
 * Oh, it destroys the buffer passed in as it writes it
 */
void N64_send(unsigned char *buffer, char length)
{
    // Send these bytes
    char bits;
    
    bool bit;

    // This routine is very carefully timed by examining the assembly output.
    // Do not change any statements, it could throw the timings off
    //
    // We get 16 cycles per microsecond, which should be plenty, but we need to
    // be conservative. Most assembly ops take 1 cycle, but a few take 2
    //
    // I use manually constructed for-loops out of gotos so I have more control
    // over the outputted assembly. I can insert nops where it was impossible
    // with a for loop
    
    asm volatile (";Starting outer for loop");
outer_loop:
    {
        asm volatile (";Starting inner for loop");
        bits=8;
inner_loop:
        {
            // Starting a bit, set the line low
            asm volatile (";Setting line to low");
            N64_LOW; // 1 op, 2 cycles

            asm volatile (";branching");
            if (*buffer >> 7) {
                asm volatile (";Bit is a 1");
                // 1 bit
                // remain low for 1us, then go high for 3us
                // nop block 1
                asm volatile ("nop\nnop\nnop\nnop\nnop\n");
                
                asm volatile (";Setting line to high");
                N64_HIGH;

                // nop block 2
                // we'll wait only 2us to sync up with both conditions
                // at the bottom of the if statement
                asm volatile ("nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              );

            } else {
                asm volatile (";Bit is a 0");
                // 0 bit
                // remain low for 3us, then go high for 1us
                // nop block 3
                asm volatile ("nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\nnop\n"  
                              "nop\n");

                asm volatile (";Setting line to high");
                N64_HIGH;

                // wait for 1us
                asm volatile ("; end of conditional branch, need to wait 1us more before next bit");
                
            }
            // end of the if, the line is high and needs to remain
            // high for exactly 16 more cycles, regardless of the previous
            // branch path

            asm volatile (";finishing inner loop body");
            --bits;
            if (bits != 0) {
                // nop block 4
                // this block is why a for loop was impossible
                asm volatile ("nop\nnop\nnop\nnop\nnop\n"  
                              "nop\nnop\nnop\nnop\n");
                // rotate bits
                asm volatile (";rotating out bits");
                *buffer <<= 1;

                goto inner_loop;
            } // fall out of inner loop
        }
        asm volatile (";continuing outer loop");
        // In this case: the inner loop exits and the outer loop iterates,
        // there are /exactly/ 16 cycles taken up by the necessary operations.
        // So no nops are needed here (that was lucky!)
        --length;
        if (length != 0) {
            ++buffer;
            goto outer_loop;
        } // fall out of outer loop
    }

    // send a single stop (1) bit
    // nop block 5
    asm volatile ("nop\nnop\nnop\nnop\n");
    N64_LOW;
    // wait 1 us, 16 cycles, then raise the line 
    // 16-2=14
    // nop block 6
    asm volatile ("nop\nnop\nnop\nnop\nnop\n"
                  "nop\nnop\nnop\nnop\nnop\n"  
                  "nop\nnop\nnop\nnop\n");
    N64_HIGH;

}

void N64_get()
{
    // listen for the expected 8 bytes of data back from the controller and
    // blast it out to the N64_raw_dump array, one bit per byte for extra speed.
    // Afterwards, call translate_raw_data() to interpret the raw data and pack
    // it into the N64_status struct.
    asm volatile (";Starting to listen");
    unsigned char timeout;
    char bitcount = 32;
    char *bitbin = N64_raw_dump;

    // Again, using gotos here to make the assembly more predictable and
    // optimization easier (please don't kill me)
read_loop:
    timeout = 0x3f;
    // wait for line to go low
    while (N64_QUERY) {
        if (!--timeout)
            return;
    }
    // wait approx 2us and poll the line
    asm volatile (
                  "nop\nnop\nnop\nnop\nnop\n"  
                  "nop\nnop\nnop\nnop\nnop\n"  
                  "nop\nnop\nnop\nnop\nnop\n"  
                  "nop\nnop\nnop\nnop\nnop\n"  
                  "nop\nnop\nnop\nnop\nnop\n"  
                  "nop\nnop\nnop\nnop\nnop\n"  
            );
    *bitbin = N64_QUERY;
    ++bitbin;
    --bitcount;
    if (bitcount == 0)
        return;

    // wait for line to go high again
    // it may already be high, so this should just drop through
    timeout = 0x3f;
    while (!N64_QUERY) {
        if (!--timeout)
            return;
    }
    goto read_loop;

}
