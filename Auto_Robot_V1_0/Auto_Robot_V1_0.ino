/* Author: Adam Sherlaw
 * Created: December 2013
 * adamsherlaw@gmail.com
 *
 * Code is open source, feel free to use, modify, distribute
 * as you wish. Please include reference to the original creator
 * when doing so.
 */


/*
Basis code for automated fully functional robot project.
 Original code is for robot testing via the serial port on the arduino.
 Hardware is 
 - Arduino Uno
 - Tamiya dual motor gearbox
 - Tamiya track set
 - Tamiya universal plate set
 - DX dual channel h bridge - 2A per channel - 3A peak per channel
 
 Future:
 Bluetooth for wireless control
 Wifi for internet access
 Servo with ping sensor on board to detect obsticals and then turns side to
 side to determine which way to go.
 Possibly implement a speed control ie: based on how long the key has been pressed
 speed up..?
 */

// Declarations of pin mappings

// Left Motor
int ENA = 9;
int IN1 = 12;
int IN2 = 2;

// Right motor
int ENB = 11;
int IN3 = 10;
int IN4 = 3;

int motor_speed = 120;

unsigned long serial_data;
int in_byte;

/* Setup outputs for tracks along with display message
 */
void setup () {
  pinMode (ENA, OUTPUT);
  pinMode (IN1, OUTPUT);
  pinMode (IN2, OUTPUT);
  pinMode (ENB, OUTPUT);
  pinMode (IN3, OUTPUT);
  pinMode (IN4, OUTPUT);
  Serial.begin (9600);
  Serial.println("Robot online....\n"); 
  Serial.println("Commands:");
  Serial.println("Forward: 1\nBackward: 2\nLeft: 3\nRight: 4");
}

void loop () {
  // Retrieve the command
  getSerial();
  
  // Act accoring to the command
  switch (serial_data) {
    case 1: moveForward();
        break;
      
    case 2: moveBackward();
        break; 
      
    case 3: turnLeft();
        break; 
      
    case 4: turnRight();
        break;
   }
}

/* Retrieve the serial commands
 * 
 */
long getSerial () {
  serial_data = 0;
  
  while (in_byte != '/') {
    in_byte = Serial.read();
    
    if (in_byte > 0 && in_byte != '/') {
      serial_data = serial_data * 10 + in_byte - '0';
      //Serial.println(serial_data); 
    }
  }
  in_byte = 0;
  
  return serial_data;
}

/* Methods for robot track control
 * Very basic time delay driven track Control.
 */

void turnRight() {
  Serial.println("Right");
  digitalWrite (IN1, HIGH);
  digitalWrite (IN2, LOW);
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);
  analogWrite (ENA, motor_speed);
  analogWrite (ENB, motor_speed);
  delay (500);
  digitalWrite (IN1, LOW);
  digitalWrite (IN3, LOW);

}

void turnLeft() {
  Serial.println("Left");
  digitalWrite (IN1, LOW);
  digitalWrite (IN2, HIGH);
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, HIGH);
  analogWrite (ENA, motor_speed);
  analogWrite (ENB, motor_speed);
  delay (500);
  digitalWrite (IN2, LOW);
  digitalWrite (IN4, LOW);

}

void  moveBackward() {
  Serial.println("Backward");
  digitalWrite (IN1, HIGH);
  digitalWrite (IN2, LOW);
  digitalWrite (IN3, LOW);
  digitalWrite (IN4, HIGH);
  analogWrite (ENA, motor_speed);
  analogWrite (ENB, motor_speed);
  delay (500);
  digitalWrite (IN1, LOW);
  digitalWrite (IN4, LOW);
}

void moveForward() {
  digitalWrite (IN1, LOW);
  digitalWrite (IN2, HIGH);
  digitalWrite (IN3, HIGH);
  digitalWrite (IN4, LOW);
  analogWrite (ENA, motor_speed);
  analogWrite (ENB, motor_speed);
  delay (500);
  digitalWrite (IN2, LOW);
  digitalWrite (IN3, LOW);
  Serial.println("Forward");
}





