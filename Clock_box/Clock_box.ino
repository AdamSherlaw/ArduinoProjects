/* Author: Adam Sherlaw
 * Created: June 2014
 * adamsherlaw@gmail.com
 *
 * Code is open source, feel free to use, modify, distribute
 * as you wish. Please include reference to the original creator
 * when doing so.
 */
#include <Time.h>


/*
 * Word clock 
 * Uses 100 led's, connected in parralel for each word and 
 * 4 8bit shift registers to control 4 7 output darlington
 * transistor arrays. 
 * The clock is slplit into two halves. The top half and the
 * bottom half. Each of the halves are controlled by two 8bit
 * sift registers and two transistor arrays. This was done to 
 * aid in the creation of the binary strings that are pused out
 * to the shif registers.
 * Below the code there are hexidecimal numbers followed by their
 * binary equilvalent. The ones represent highs that turn on the
 * transistor that is connected to the shift register which inturn
 * turns on the corresponding 'word' in the clock.
 * 
 *
 * To set the time on the clock:
 * Hold down the hours button while powering up the clock
 * The clock will flash the led to indicate you are in setup.
 * Press the hours and minutes buttons to set the number of hours
 * and minutes (12 hour). Each button press should flash the led.
 *
 * When done leave the clock to time out and the led will flash once
 * more. The time should now be displayed.
 */


// Pin declarations for shift register inputs.
int latchPin = 4;
int clockPin = 3;
int dataPin = 2;

int latchPin2 = 10;
int clockPin2 = 11;
int dataPin2 = 12;

int hourBtn = 7;
int minuteBtn = 6;

int led = 13;

// Array Declarations 

// Array holds first byte values for phrases on top half of clock.
byte pointerByte1[12] = { 
  0x1E9, 0x199, 0x199, 0x1B9, 0x199, 0x199, 0x1B9, 
  0x1D1, 0x1D1, 0x1F1, 0x51, 0x51 };

// Array holds second byte values for phrases on top half of clock.
byte pointerByte2[12] = { 
  0xFF, 0x1DF, 0x1FD, 0x1F7, 0x1EF, 0x1CF, 0x1FB, 
  0X1CF, 0x1EF, 0x1F7, 0x1FD, 0x1DF};

// Array holds first byte values for the light values one through to twelve for the hours.
byte hoursByte1[12] = { 
  0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F, 0xFE, 0xFF, 0xFF, 
  0xFF, 0xFF };

// Array holds second byte values for the light values one through to twelve for the hours.
byte hoursByte2[12] = { 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFB, 
  0xF7, 0xEF };

void setup() {
  Serial.begin(9600); 

  pinMode(hourBtn, INPUT);
  pinMode(minuteBtn, INPUT);

  pinMode(led, OUTPUT);

  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(dataPin2, OUTPUT);
  pinMode(latchPin2, OUTPUT);
  pinMode(clockPin2, OUTPUT);

  // Enable the setting of the clock time (set the micro controller time)
  if (digitalRead(hourBtn) == LOW) setTime();
}


void loop() { 
  displayTime(now()); 
}

// Display the current time of the microcontroller
void displayTime(time_t time) {
  int minutes = minute(time);
  int hours = hour(time);
  hours = hours % 12;

  // Debugging
  printTime(time);
  
  // Show the upcoming hour
  if (minutes >= 35) showHour(hours + 1); 
  // Show the past hour
  else showHour(hours);

  // Display the current time by selecting the correct byte to shift out for the 
  // Second half
  if (minutes < 5) shift(1, pointerByte1[0], pointerByte2[0]);

  if (minutes >= 5 && minutes < 10)   shift(1, pointerByte1[1], pointerByte2[1]);

  if (minutes >= 10 && minutes < 15)  shift(1, pointerByte1[2], pointerByte2[2]);

  if (minutes >= 15 && minutes < 20)  shift(1, pointerByte1[3], pointerByte2[3]);

  if (minutes >= 20 && minutes < 25)  shift(1, pointerByte1[4], pointerByte2[4]);

  if (minutes >= 25 && minutes < 30)  shift(1, pointerByte1[5], pointerByte2[5]);

  if (minutes >= 30 && minutes < 35)  shift(1, pointerByte1[6], pointerByte2[6]);

  if (minutes >= 35 && minutes < 40)  shift(1, pointerByte1[7], pointerByte2[7]);

  if (minutes >= 40 && minutes < 45)  shift(1, pointerByte1[8], pointerByte2[8]);

  if (minutes >= 45 && minutes < 50)  shift(1, pointerByte1[9], pointerByte2[9]);

  if (minutes >= 50 && minutes < 55)  shift(1, pointerByte1[10], pointerByte2[10]);

  if (minutes >= 55 && minutes < 60)  shift(1, pointerByte1[11], pointerByte2[11]);

  delay(1000);
  
}


/* Display the current time by selecting the correct byte to shift out for the 
 * Second half
 */
void showHour(int hour) {
  Serial.println(hour);
  if (hour == 1) shift(2, hoursByte1[0], hoursByte2[0]);

  if (hour == 2) shift(2, hoursByte1[1], hoursByte2[1]);

  if (hour == 3) shift(2, hoursByte1[2], hoursByte2[2]);

  if (hour == 4) shift(2, hoursByte1[3], hoursByte2[3]);

  if (hour == 5) shift(2, hoursByte1[4], hoursByte2[4]);

  if (hour == 6) shift(2, hoursByte1[5], hoursByte2[5]);

  if (hour == 7) shift(2, hoursByte1[6], hoursByte2[6]);

  if (hour == 8) shift(2, hoursByte1[7], hoursByte2[7]);

  if (hour == 9) shift(2, hoursByte1[8], hoursByte2[8]);

  if (hour == 10) shift(2, hoursByte1[9], hoursByte2[9]);

  if (hour == 11) shift(2, hoursByte1[10], hoursByte2[10]);

  if (hour == 12) shift(2, hoursByte1[11], hoursByte2[11]);

}


// Push the given bytes out to the given shift registers
void shift(int reg ,byte value1, byte value2) {
  if (reg == 1) {
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, MSBFIRST, value2);
    shiftOut(dataPin, clockPin, MSBFIRST, value1);
    digitalWrite(latchPin, 1); 
  } 
  if (reg == 2) {
    digitalWrite(latchPin2, 0);
    shiftOut(dataPin2, clockPin2, MSBFIRST, value2);
    shiftOut(dataPin2, clockPin2, MSBFIRST, value1);
    digitalWrite(latchPin2, 1);
  }
}

/* Print the current time to the serial monitor for 
 * debugging
 */
void printTime(time_t time) {
  int minutes = minute(time);
  int hours = hour(time);
  int seconds = second(time);

  Serial.print("Hours: ");
  Serial.print(hours);
  Serial.print(", Minutes: ");
  Serial.print(minutes);
  Serial.print(", Seconds: ");
  Serial.println(seconds);
}

/* Reads the two buttons on the back of the 
 * clock to set the hours and the minutes
 */
void setTime() {
  int i = 0;
  int j = 0;
  time_t currentTime = now();
  
  flashLed(13, 250, 4); 
  
  // Wait for a button to be pressed, or time out - 10 seconds
  while (now() - currentTime < 10) {
    //Serial.println(now() - currentTime);
    // Increment the hours by one
    if (digitalRead(hourBtn) == LOW) {
      Serial.println("Up Hours");
      currentTime = now();
      i++;
      flashLed(13, 150, 1);
    } 
    // Increment the minutes by one
    if (digitalRead(minuteBtn) == LOW) {
      Serial.println("Up Minutes");
      currentTime = now();
      j++;
      flashLed(13, 150, 1);
    }
  } 
  
  // Set the final time
  setTime(i,j,00,1,1,14);
 
  flashLed(13, 250, 4); 
}


// Flashes the given pin high and low - 13 has an led bulitin
void flashLed(int pin, int delayTime, int number) {
  for (int i = 0; i < number; i++) {
     digitalWrite(pin, HIGH);
     delay(delayTime);
     digitalWrite(pin, LOW);
     delay(delayTime);
  }
}

/* Hexidecimal values for the shift registers
 
 BYTE ONE - PHRASES
 0x1E9 = IT IS --- O'CLOCK
 0x199 = IT IS FIVE MINUTES PAST
 0x199 = IT IS TEN MINUTES PAST
 0x1B9 = IT IS QUATER PAST
 0x199 = IT IS TWENTY MINUTES PAST
 0x199 = IT IS TWENTY FIVE MINUTES PAST
 0x1B9 = IT IS HALF PAST
 0x1D1 = IT IS 25 MINUTES TO ---
 0x1D1 = IT IS TWENTY MINUTES TO ---
 0x1F1 = IT IS QUATER TO ---
 0x51 = IT IS TEN TO ---
 0x51 = IT IS FIVE TO ---
 
 BYTE TWO - PHRASES
 0xFF = IT IS --- O'CLOCK
 0x1DF = IT IS FIVE MINUTES PAST
 0x1FD = IT IS TEN MINUTES PAST
 0x1F7 = IT IS QUATER PAST
 0x1EF = IT IS TWENTY MINUTES PAST
 0x1CF = IT IS TWENTY FIVE MINUTES PAST
 0x1FB = IT IS HALF PAST
 0X1CF = IT IS 25 MINUTES TO ---
 0x1EF = IT IS TWENTY MINUTES TO ---
 0x1F7 = IT IS QUATER TO ---
 0x1FD = IT IS TEN TO ---
 0x1DF = IT IS FIVE TO ---
 
 BYTE ONE - HOURS
 0xFD = 11111101 ONE
 0xFB = 11111011 TWO
 0xF7 = 11110111 THREE
 0xEF = 11101111 FOUR
 0xDF = 11011111 FIVE
 0xBF = 10111111 SIX
 0x7F = 01111111 SEVEN
 0xFE = 11111110 EIGHT
 0xFF = 11111111 NINE
 0xFF = 11111111 TEN
 0xFF = 11111111 ELEVEN
 0xFD = 11111111 TWELVE
 
 BYTE TWO - HOURS
 0xFF = 11111111 ONE
 0xFF = 11111111 TWO
 0xFF = 11111111 THREE
 0XFF = 11111111 FOUR
 0xFF = 11111111 FIVE
 0xFF = 11111111 SIX
 0xFF = 11111111 SEVEN
 0xFF = 11111111 EIGHT
 0xFD = 11111101 NINE
 0xFB = 11111011 TEN
 0xF7 = 11110111 ELEVEN
 0xEF = 11101111 TWELVE
 */





