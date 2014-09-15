/* Author: Adam Sherlaw
 * Created: August 2014
 * adamsherlaw@gmail.com
 *
 * Code is open source, feel free to use, modify, distribute
 * as you wish. Please include reference to the original creator
 * when doing so.
 */
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Global strings to store last two lines of display
String message0 = "";
String message1 = "";


/* 
 * Multi user toaster that allows for each user to have their
 * own toasting setting to avoid dissapointments in the mornings.
 *
 * Toaster should be set to the highest toasting setting before use
 * as the microcontroller takes over stopping the toaster at the
 * correct time depending on the user selected.
 *
 * Having the original timer still in place means that the toaster
 * will still operate as intended by the manuafacturer and poses no 
 * extra danger to the user.
 *
 * Control of the toaster is obtained through an optocoupler
 * which means that there is no contact with the high voltage side of the 
 * toaster and the optocopler simply bypasses the cancel switch to stop the
 * toasting.
 *
 * A leaf switch is placed at the bottom of the toasters toast carrage that
 * is activated when the toasting is loaded and toasting has begun.
 *
 */


void setup() 
{
  // Set up serial for debugging
  Serial.begin(9600);
  // Set up LCD display for display
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  lcd.print("ToastDuino");
  lcd.setCursor(0, 1);
  lcd.print("By Adam Sherlaw");
  // Toaster control inputs and output
  pinMode(A0, INPUT);
  pinMode(10, INPUT);
  pinMode(11, OUTPUT);
}


void loop() 
{
  // Array of users
  PROGMEM char* users[] = { 
    "Adam", "Dad", "Mum", "Todd", "Sam", ""   };
  // Array of options
  PROGMEM char* option[] = { 
    "Run", "Set", "Cancel", ""   };
  
  // Select User then select option - delay for debounce of buttons
  int user = menuSelect(users, 5);
  delay(200);
  int opt = menuSelect(option, 3);
  delay(200);

  byte value;
  int save;
  int button = 100;
  int time = 0;


  switch (opt) 
  {
  // Run toasting
  case 0:
    // Pull toast settings from eeprom
    value = EEPROM.read(user);
    // Debugging statements
    Serial.print(user);
    Serial.print("\t");
    Serial.println(value, DEC);
    // Run the toast cycle
    toast(value);
    delay(2000);
    break; 

  // Set toast time
  case 1:
    button = 5;
    // Display the stored value for the user
    time = int(EEPROM.read(user));
    lcdText(0, "Time:");
    lcdText(1, String(time));
    
    // Increment or decrement the time for toasting
    // Loop until button 0 is pressed
    while(button) {
      button = readUserInput();
      delay(100); // Debounce
      if (button == 3 && time > 0) time --;
      if (button == 4 && time < 255) time ++;
      lcdText(1, String(time));
      
      // When button 0 is pressed, store the current toast time
      if (button == 0) {
        Serial.println(time);
        EEPROM.write(user, time);
      }
    }
    break; 
  }
}

/* Select a menu option.
 * Method moves the current option to the top of the screen
 * Achieved by redrawing the entire LCD when an up or down arrow
 * is pressed.
 * Returns the selected menu item (index of item) from the menu item array
 */
int menuSelect(char *menu[], int length) 
{
  int selection = 0;        
  int input;

  // Infinate loop until button 0 is pressed.
  while (1) {
    // Keep the selection inside the range of the array
    selection = selection % length;
    // Display the current menu item on the top line followed by the next item
    lcdText(0, String(char(126) + String(menu[selection])));
    // Empty line means we are at the end
    if (menu[selection + 1][0] != 0) lcdText(1, String(" " + String(menu[selection + 1])));
    else lcdClearLine(1);

    // Read the user input - i.e. button press to move items up or down the display
    input = readUserInput();

    if (input == 0) return selection;
    if (input == 3 && selection != 0) selection --;
    if (input ==4) selection ++;
    
    delay(100); // Debounce on buttons
  }  
}


// Displays the given text on the specified line of the lcd
void lcdText(int line, String lcdMessage) {
  // Don't display an unchanged message to eliminate flicker
  if (line == 0 && message0 == lcdMessage) return;
  if (line == 1 && message1 == lcdMessage) return;

  // Else there must be a new line of text to display
  lcdClearLine(line);
  lcd.print(lcdMessage);    
  if (line == 0) message0 = lcdMessage;
  else message1 = lcdMessage;
}

// Clears the given lcd line
void lcdClearLine(int line) 
{
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor(0, line);
}


/* Reads the input A0 (Freetronics LCD Shield) for user input.
 * Returns:
 * 0: Select keypress
 * 1: Left keypress
 * 2: Right keypress
 * 3: Up keypress
 * 4: Down keypress
 * 5: No button pressed.
 */
int readUserInput() 
{
  int input = analogRead(A0);
  if (input > 700 && input < 800) return 0;
  if (input > 450 && input < 550) return 1;
  if (input > 0 && input < 50) return 2;
  if (input > 100 && input < 200) return 3;
  if (input > 300 && input < 400) return 4;
  else return 5;
}

/* Activate toaster for given time
 * Timer begins when the user presses the
 * toaster down.
 */
void toast(int time)
{
  int button = 5;
  // Converts users input time to milliseconds - Crude / Possibly incorrect at this stage
  double cook = double(time) / 100 * 60 * 1000;
  // Debugging
  Serial.println(cook);
  lcdText(0, "Push Toast ");
  lcdText(1, "down to begin");
  
  // While the toaster is not pressed - Wait
  while (digitalRead(10)) delay(10); 
  
  // Alert user to current state
  lcdText(0, "Toasting...");
  lcdClearLine(1);
  delay(cook);
  cancelToast();
  lcdText(0, "TOAST COMPLETE!");
  lcdClearLine(1);
}

/* Over rides the toasters external cancel button
 * using an optocoupler to activate the switch.
 */
void cancelToast() {
  Serial.println("TOAST DONE");
  digitalWrite(11, HIGH); 
  delay(100);
  digitalWrite(11, LOW);
}



