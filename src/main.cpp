#include <Arduino.h>
#include <config.h>
#include <matrixchars.h>
#include <tones.h>
#include <flasher.h>
#include <OneButton.h>
#include <tools.h>
#include <MaxMatrix.h>
#include <avr/pgmspace.h>

/*
Control Panel Lite by MileStorm
made in 2019 - 2020

Contains four games:
 - Push the lit button
 - Simon says
 - Sound board
 - <not yet implemented>

uses code for Simon says game from SparkFun Inventor's Kit: Example sketch 16
*/

MaxMatrix dot_matrix(DOTMATRIX_DIN, DOTMATRIX_CS, DOTMATRIX_CLK, DOTMATRIX_DISPLAY_COUNT);

OneButton myButtons[] = {OneButton(BUTTON_RED, true), OneButton(BUTTON_GREEN, true), OneButton(BUTTON_BLUE, true), OneButton(BUTTON_YELLOW, true)};
Flasher myLeds[] = {Flasher(LED_RED, 300, 300), Flasher(LED_GREEN, 300, 300), Flasher(LED_BLUE, 300, 300), Flasher(LED_YELLOW, 300, 300)};

// matrix sprites
byte xicht_happy[] =   {8, 8, B00000000, B00100000, B01001100, B01000000, B01000000, B01001100, B00100000, B00000000};
byte xicht_wink[] =    {8, 8, B00000000, B00100100, B01000100, B01000000, B01000000, B01001100, B00100000, B00000000};
byte xicht_sad[] =     {8, 8, B00000000, B01000000, B00101100, B00100000, B00100000, B00101100, B01000000, B00000000};
byte xicht_default[] = {8, 8, B00000000, B00000000, B00101100, B00100000, B00100000, B00101100, B00000000, B00000000};

byte xicht_smajlik[] = {8, 8, B00000000, B00010110, B00100110, B01100000, B01100000, B00100110, B00010110, B00000000};

byte numbers_score[] = {
  B01111100, B01000100, B01111100, B00000000, // 0 = 0
  B00000000, B00001000, B01111100, B00000000, // 1 = 4
  B01110100, B01010100, B01011100, B00000000, // 2 = 8
  B01010100, B01010100, B01111100, B00000000, // 3 = 12
  B00011100, B00010000, B01111100, B00000000, // 4 = 16
  B01011100, B01010100, B01110100, B00000000, // 5 = 20
  B01111100, B01010100, B01110100, B00000000, // 6 = 24
  B00000100, B00000100, B01111100, B00000000, // 7 = 28
  B01111100, B01010100, B01111100, B00000000, // 8 = 32
  B01011100, B01010100, B01111100, B00000000  // 9 = 36
};

// Scrolling text
byte buffer[10];
char start_message_buttons[] =  " Buttons   ";
char start_message_simon[] =    " Simon says   ";
char start_message_soundboard[] =    " Soundboard   ";

// gameType: menu -1, mashbutton 0, simon 1, soundboard 2,
int gameType = -1;

// --------------------------------------------------------------------------------

// Button game
// ===========

int activeLed = -1;
int buttonGameScore = 0;

void turnOnAllLeds() {
	for (int i = 0; i < 4; i++) {
		myLeds[i].permanentOn();
	}
}

void turnOffAllLeds() {
	for (int i = 0; i < 4; i++) {
		myLeds[i].off();
	}
}

void flashAllLeds() {
	for (int i = 0; i < 4; i++) {
		myLeds[i].flashingOn();
	}
}

int turnOnRandomLed() {
	int rndNumber;

	// makes the next led different than the one, that is lit.
	// do {
	// 	rndNumber = randomGenerator(0,3);
	// } while (rndNumber == activeLed);

  rndNumber = randomGenerator(0,3);

	myLeds[rndNumber].permanentOn();
	return rndNumber;
}

// ----------------------------------------------------------------
// Simon says
// ==========

// Game state variables
byte gameBoard[32]; //Contains the combination of buttons as we advance
byte gameRound = 0; //Counts the number of succesful rounds the player has made it through

// Put extracted character on Display
void printCharWithShift(char c, int shift_speed){
  if (c < 32) return;
  c -= 32;
  memcpy_P(buffer, CH + 7 * c, 7);
  dot_matrix.writeSprite(DOTMATRIX_DISPLAY_COUNT * 8, 0, buffer);
  dot_matrix.setColumn(DOTMATRIX_DISPLAY_COUNT * 8 + buffer[0], 0);

  for (int i = 0; i < buffer[0] + 1; i++)
  {
    delay(shift_speed);
    dot_matrix.shiftLeft(false, false);
  }
}

// Extract characters from Scrolling text
void printStringWithShift(char* s, int shift_speed){
  while (*s != 0){
    printCharWithShift(*s, shift_speed);
    s++;
  }
}

void writeScore(int scoreInput, bool scrollable = true) {
  byte finalSprite[10];
  int spritePos = 4;
  finalSprite[1] = 8;
  int ones;
  int tens;

  if (scoreInput < 10) { // jednoznakovy cisla
    finalSprite[0] = 4;
    spritePos = 4;
    for (int i = 2; i < 6; i++) { //6,10
        finalSprite[i] = numbers_score[(scoreInput * 4) + (i - 2)];
    }
  }
  else { // dvouznakovy cisla
    finalSprite[0] = 8;
    spritePos = 0;
    ones=scoreInput%10;
    scoreInput=scoreInput/10;
    tens=scoreInput%10;

    for (int i = 2; i < 6; i++) {
        finalSprite[i] = numbers_score[(tens * 4) + (i - 2)];
    }
    for (int i = 6; i < 10; i++) {
        finalSprite[i] = numbers_score[(ones * 4) + (i - 6)];
    }

  }
  if (scrollable == true){
    dot_matrix.writeSprite(spritePos+8, 0, finalSprite);
    for (int i=0; i<8; i++){
      dot_matrix.shiftLeft(false, false);
      delay(100);
    }
  } else {
    dot_matrix.writeSprite(spritePos, 0, finalSprite);
  }

}

// Adds a new random button to the game sequence, by sampling the timer
void add_to_moves(void)
{
  byte newButton = random(0, 4); //min (included), max (exluded)

  // We have to convert this number, 0 to 3, to CHOICEs
  if(newButton == 0) newButton = CHOICE_RED;
  else if(newButton == 1) newButton = CHOICE_GREEN;
  else if(newButton == 2) newButton = CHOICE_BLUE;
  else if(newButton == 3) newButton = CHOICE_YELLOW;

  gameBoard[gameRound++] = newButton; // Add this new button to the game array
}

// Toggle buzzer every buzz_delay_us, for a duration of buzz_length_ms.
void buzz_sound(int buzz_length_ms, int buzz_delay_us)
{
  // Convert total play time from milliseconds to microseconds
  long buzz_length_us = buzz_length_ms * (long)1000;

  // Loop until the remaining play time is less than a single buzz_delay_us
  while (buzz_length_us > (buzz_delay_us * 2))
  {
    buzz_length_us -= buzz_delay_us * 2; //Decrease the remaining play time

    // Toggle the buzzer at various speeds
    digitalWrite(BUZZER, HIGH);
    delayMicroseconds(buzz_delay_us);

    digitalWrite(BUZZER, LOW);
    delayMicroseconds(buzz_delay_us);
  }
}

// Lights a given LEDs
// Pass in a byte that is made up from CHOICE_RED, CHOICE_YELLOW, etc
void setLEDs(byte leds)
{
  if ((leds & CHOICE_RED) != 0)
    digitalWrite(LED_RED, HIGH);
  else
    digitalWrite(LED_RED, LOW);

  if ((leds & CHOICE_GREEN) != 0)
    digitalWrite(LED_GREEN, HIGH);
  else
    digitalWrite(LED_GREEN, LOW);

  if ((leds & CHOICE_BLUE) != 0)
    digitalWrite(LED_BLUE, HIGH);
  else
    digitalWrite(LED_BLUE, LOW);

  if ((leds & CHOICE_YELLOW) != 0)
    digitalWrite(LED_YELLOW, HIGH);
  else
    digitalWrite(LED_YELLOW, LOW);
}

// Light an LED and play tone
// Red, upper left:     440Hz - 2.272ms - 1.136ms pulse
// Green, upper right:  880Hz - 1.136ms - 0.568ms pulse
// Blue, lower left:    587.33Hz - 1.702ms - 0.851ms pulse
// Yellow, lower right: 784Hz - 1.276ms - 0.638ms pulse
void toner(byte which, int buzz_length_ms)
{
  setLEDs(which); //Turn on a given LED

  //Play the sound associated with the given LED
  switch(which)
  {
  case CHOICE_RED:
    buzz_sound(buzz_length_ms, 1136);
    break;
  case CHOICE_GREEN:
    buzz_sound(buzz_length_ms, 568);
    break;
  case CHOICE_BLUE:
    buzz_sound(buzz_length_ms, 851);
    break;
  case CHOICE_YELLOW:
    buzz_sound(buzz_length_ms, 638);
    break;
  }

  setLEDs(CHOICE_OFF); // Turn off all LEDs
}

// Plays the current contents of the game moves
void playMoves(void)
{
  for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++) 
  {
    toner(gameBoard[currentMove], 150);

    // Wait some amount of time between button playback
    // Shorten this to make game harder
    delay(150); // 150 works well. 75 gets fast.
  }
}

// Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.
byte checkButton(void)
{
  if (digitalRead(BUTTON_RED) == 0) return(CHOICE_RED);
  else if (digitalRead(BUTTON_GREEN) == 0) return(CHOICE_GREEN);
  else if (digitalRead(BUTTON_BLUE) == 0) return(CHOICE_BLUE);
  else if (digitalRead(BUTTON_YELLOW) == 0) return(CHOICE_YELLOW);

  return(CHOICE_NONE); // If no button is pressed, return none
}

// Wait for a button to be pressed.
// Returns one of LED colors (LED_RED, etc.) if successful, 0 if timed out
byte wait_for_button(void)
{
  long startTime = millis(); // Remember the time we started the this loop

  while ( (millis() - startTime) < ENTRY_TIME_LIMIT) // Loop until too much time has passed
  {
    byte button = checkButton();

    if (button != CHOICE_NONE)
    {
      toner(button, 150); // Play the button the user just pressed

      while(checkButton() != CHOICE_NONE) ;  // Now let's wait for user to release button

      delay(10); // This helps with debouncing and accidental double taps

      return button;
    }

  }

  return CHOICE_NONE; // If we get here, we've timed out!
}

// Play the regular memory game
// Returns 0 if player loses, or 1 if player wins
boolean play_memory(void)
{
  randomSeed(millis()); // Seed the random generator with random amount of millis()

  gameRound = 0; // Reset the game to the beginning

  while (gameRound < ROUNDS_TO_WIN)
  {
    add_to_moves(); // Add a button to the current moves, then play them back

    playMoves(); // Play back the current game board

    // Then require the player to repeat the sequence.
    for (byte currentMove = 0 ; currentMove < gameRound ; currentMove++)
    {
      byte choice = wait_for_button(); // See what button the user presses

      if (choice == 0) return false; // If wait timed out, player loses

      if (choice != gameBoard[currentMove]) return false; // If the choice is incorect, player loses
    }

    dot_matrix.writeSprite(0, 0, xicht_happy);
    delay(1000); // Player was correct, delay before playing moves
    dot_matrix.writeSprite(0, 0, xicht_default);
  }

  return true; // Player made it through all the rounds to win!
}

// Play the winner sound
// This is just a unique (annoying) sound we came up with, there is no magic to it
void winner_sound(void)
{
  // Toggle the buzzer at various speeds
  for (byte x = 250 ; x > 70 ; x--)
  {
    for (byte y = 0 ; y < 3 ; y++)
    {
      digitalWrite(BUZZER, HIGH);
      delayMicroseconds(x);

      digitalWrite(BUZZER, LOW);
      delayMicroseconds(x);
    }
  }
}

// Play the winner sound and lights
void play_winner(void)
{
  setLEDs(CHOICE_GREEN | CHOICE_BLUE);
  winner_sound();
  setLEDs(CHOICE_RED | CHOICE_YELLOW);
  winner_sound();
  setLEDs(CHOICE_GREEN | CHOICE_BLUE);
  winner_sound();
  setLEDs(CHOICE_RED | CHOICE_YELLOW);
  winner_sound();
}

void play_intro(void)
{
  printStringWithShift(start_message_simon, 100);  // Send scrolling Text

  play_winner();
}

// Play the loser sound/lights
void play_loser(void)
{
  setLEDs(CHOICE_RED | CHOICE_GREEN);
  buzz_sound(255, 1500);

  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
  buzz_sound(255, 1500);

  setLEDs(CHOICE_RED | CHOICE_GREEN);
  buzz_sound(255, 1500);

  setLEDs(CHOICE_BLUE | CHOICE_YELLOW);
  buzz_sound(255, 1500);
}

// Show an "attract mode" display while waiting for user to press button.
void attractMode(void)
{
  while(1)
  {
    setLEDs(CHOICE_RED);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;

    setLEDs(CHOICE_BLUE);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;

    setLEDs(CHOICE_GREEN);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;

    setLEDs(CHOICE_YELLOW);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
  }
}

// ----------------------------------------------------------------

// button handler

void processPush(int buttonId) {
  Serial.print("ButtonID: ");
  Serial.println(buttonId);
  Serial.print("GameType: ");
  Serial.println(gameType);

  switch (gameType){
  case -1:
    // main menu
    gameType = buttonId;

    // inits of games
    switch (buttonId){
    case 0:
      // button game
      turnOffAllLeds();
      printStringWithShift(start_message_buttons, 100);
      writeScore(buttonGameScore, false);
      activeLed = turnOnRandomLed();
      break;

    case 1:
      // simon
      turnOffAllLeds();
      play_intro();
      break;

    case 2:
      // sound board
      turnOffAllLeds();
      break;

    case 3:
      // not yet implemented
      turnOffAllLeds();
      break;

    default:
      break;
    }
    break;

  case 0:
    // button game
    if (myLeds[buttonId].isPermanentOn()) {
      turnOffAllLeds();
      activeLed = turnOnRandomLed();
      buttonGameScore += 1;
      tone(BUZZER, NOTE_A5, 100);
      delay(100);
      tone(BUZZER, NOTE_C6, 200);
    } else {
      myLeds[activeLed].flashingOn();
      buttonGameScore = 0;
      tone(BUZZER, NOTE_A3, 400);
      delay(400);
      tone(BUZZER, NOTE_G3, 400);
      delay(400);
      tone(BUZZER, NOTE_C3, 400);
      delay(400);
      dot_matrix.clear();
    }
    writeScore(buttonGameScore, false);
    break;

  case 1:
    // simon
    // processes in the loop section, not here
    break;

  case 2:
    // sound board
    break;

  case 3:
    // not yet implemented
  break;

  default:
    break;
  }

}

// RED
void butt0Click() {
	processPush(0);
}

// GREEN
void butt1Click() {
	processPush(1);
}

// BLUE
void butt2Click() {
	processPush(2);
}

// YELLOW
void butt3Click() {
	processPush(3);
}

// -----------------------------------------
// SETUP
// ------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");

  pinMode(BUZZER, OUTPUT);

  dot_matrix.init(); // module initialize
  dot_matrix.setIntensity(3); // dot matix intensity 0-15
  // bootup
  turnOnAllLeds();
  delay(1000);
  dot_matrix.writeSprite(0, 0, xicht_smajlik);
  tone(BUZZER, NOTE_E5, 100);
  delay(100);
  tone(BUZZER, NOTE_E6, 400);
  flashAllLeds();

	for (int i = 0; i < 4; i++){
		myButtons[i].setPressTicks(60);
	}

	myButtons[0].attachPress(butt0Click);
	myButtons[1].attachPress(butt1Click);
	myButtons[2].attachPress(butt2Click);
	myButtons[3].attachPress(butt3Click);

  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);
  pinMode(BUTTON_BLUE, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
}

// --------------------------------------
// LOOP
// --------------------------------------

void loop() {
  switch (gameType){
  case 1:
    // loop for simon
    attractMode(); // Blink lights while waiting for user to press a button

    // Indicate the start of game play
    dot_matrix.clear();
    dot_matrix.writeSprite(0, 0, xicht_default);
    setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Turn all LEDs on
    delay(1000);
    setLEDs(CHOICE_OFF); // Turn off LEDs
    delay(250);


    // Play memory game and handle result
    if (play_memory() == true) {
      play_winner(); // Player won, play winner tones
      dot_matrix.clear();
      writeScore(gameRound-1);
    }
    else {
      dot_matrix.writeSprite(0, 0, xicht_sad);
      play_loser(); // Player lost, play loser tones
      dot_matrix.clear();
      writeScore(gameRound-1);
    }
    break;

  default:
    // loop for everything else
    for (int i = 0; i < 4; i++){
      myButtons[i].tick();
      myLeds[i].tick();
    }
    break;
  }
}

// TODO:
/*
u soundboardu udelat naky animace s virtualdelay
array framu, ktery se zobrazej spolu se zvukem, kterej bude taky pres virtual delay
efektu a animaci by bylo 8, protoze press a longpress
*/