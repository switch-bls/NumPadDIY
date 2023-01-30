#include <Keyboard.h>


////Config variables
//Matrix Setup
const int LAYERS = 3;
const int ROWS = 5;
const int COLUMNS = 5;

//Loop Delay
int delayLoopPin = 2;

//Input pins,
int ColPins[COLUMNS] = {8,9,7,6,5};
int RowPins[ROWS] = {14,2,16,15,10};

//Output debug info to serial monitor
const bool EnableDebug = true;

////System Variables

//Default Buttons
const int NO_ACTION = 0;
const int LAYER_RAISE = 1;
const int LAYER_LOWER = 2;

//Keymap
bool switchStates[ROWS][COLUMNS];
int currentLayer = 0; //0 raised; 1 default; 2 lowered; 

const unsigned int Keymap[LAYERS][ROWS][COLUMNS] = 
{
  {//0 Raised layer
    {
      222,//  -
      235, // .
      223, // +
      224, // Enter
      NO_ACTION //  
    },
    {
      221, // *
      227, // 3
      233, // 9
      230, // 6
      NO_ACTION  // 
    },
    {
      220, // //
      226, // 2
      232, // 8
      229, // 5
      NO_ACTION  //
    },
    {
      NO_ACTION, //
      NO_ACTION, // 
      NO_ACTION, //
      NO_ACTION, // 
      234, // 0
    },
    {
      219,  // META
      225,  // 1
      231, // 7
      228,  // 4
      NO_ACTION,  
    }
   }
 };


void setup()
{
  setupPins();
  Serial.begin(9600); 
  pinMode(11, OUTPUT);
}

void delayByPin() {
  //If delayPin becomes low (connected to ground) then delay loop
  //implemented to stop the chip from becoming stuck in a key spamming loop
  //Note: Can be removed if in need of the extra pin.
  while (digitalRead(delayLoopPin) == LOW) 
  {
    delay(8000);
  }
}

void loop()
{
  delayByPin();
  switchMatrixLoop();
  delay(10);
}

void setupPins() {
  pinMode(delayLoopPin, INPUT_PULLUP);

  //Setup col pins
  for(int i=0; i<COLUMNS; i++){
    pinMode(ColPins[i], INPUT_PULLUP);
  }

  //Setup row pins
  for(int i=0; i<ROWS; i++){
    pinMode(RowPins[i], OUTPUT);
  }
}

void switchMatrixLoop() {
  //loop through columns, loop through rows and read outputs
  for(int row = 0; row < ROWS; row++) 
  {
    digitalWrite(RowPins[row], LOW); //Power Low on column
    for(int column = 0; column < COLUMNS; column++)
    {
      //Pass inverted pin state of ColPin as row state because of INPUT_PULLUP
      //If the switch is pressed, this column should be pulled LOW by the column pin.
      //By inverting this a pressed key is 1 and a released key is 0.
      keyChangeCheck(!digitalRead(ColPins[column]), row, column);       
    }
    digitalWrite(RowPins[row], HIGH); //Power back to High on Column
  }
}
          

void releaseAllSwitchStates() //Clear switchStates array
{
  for(int y = 0; y < ROWS; y++){
    for(int z = 0; z < COLUMNS; z++){
      switchStates[y][z] = false;
    }
  }
}

void keyChangeCheck(bool currentState, int row, int column) 
{  
  bool previousState = switchStates[row][column];
  if(currentState == previousState)   //Key is the same
  {
    //No change;
  }
  else if (currentState == HIGH)      //Key is pressed
  {
    switchStates[row][column] = true;
    keyPressHandler(Keymap[currentLayer][row][column]);
    
    DebugPrintKeyState();
  }
  else                                //Key is released
  {
    //Serial.println(String(Keymap[currentLayer][row][column]) + String(" Key released, Layer: ") + currentLayer + String(" Row: ") + row + String(" Column: ") + column);
    switchStates[row][column] = false;
    keyReleaseHandler(Keymap[currentLayer][row][column]);

    DebugPrintKeyState();
  }
}
void keyPressHandler(int key) //Key Press
{
  if(key == LAYER_RAISE){
    Keyboard.releaseAll();
    releaseAllSwitchStates();
    currentLayer = 0;
  }
  else if(key == LAYER_LOWER){
    Keyboard.releaseAll();
    releaseAllSwitchStates();
    currentLayer = 2;
  }
  else if(key == NO_ACTION){
    // Do nothing
  }
  else {
    digitalWrite(11, HIGH); 
    Keyboard.press(key);
  }
}

void keyReleaseHandler(int key) //Key Release
{
  if(key == LAYER_RAISE){
    Keyboard.releaseAll();
    releaseAllSwitchStates();
    currentLayer = 1;
  }
  else if(key == LAYER_LOWER){
    Keyboard.releaseAll();
    releaseAllSwitchStates();
    currentLayer = 1;
  }
  else if(key == NO_ACTION){
    // Do nothing
  }
  else {
    Keyboard.release(key);
    digitalWrite(11, LOW);
  }
}


void DebugPrintKeyState()//Debugging
{
  if(EnableDebug)
  {
  Serial.println("Matrix State:");
  for(int y = 0; y < ROWS; y++){
    for(int z = 0; z < COLUMNS; z++){
      Serial.print(switchStates[y][z]);  
    }
    Serial.println("");
   }   
   Serial.println("");
  }
}
