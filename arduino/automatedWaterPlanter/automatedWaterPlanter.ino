#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Wire.h>

//soilSensorPin
int SensorPin = A0;

const int rs = 11, en = 10, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
// buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {25, 26, 27, 28}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {22, 23, 24}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad cusKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int val = 0;
int angPin = A9;

int buzz = 9;
int LeftMotorForward1 = 13;
int LeftMotorReverse1 = 12;
int cursor_y = 0, cursor_x = 1;

const int echoPin = 31; // Echo Pin of Ultrasonic Sensor
const int pingPin = 30; // Trigger Pin of Ultrasonic Sensor

bool low_water = false;
bool buzzer = true;
long time_interval_in_seconds=0;
bool time_interval_set=false;
String value="";
long seconds=0;

int menu[6];//0.Main menu 1.interval menu 2.second menu 3.Minute menu 4.Hour menu 5.Water the plants
void setmenu(int j) {
  for(int i=0;i<6;i++) {
    if(i==j) menu[i]=1;
    else menu[i]=0;
  }
}
void setup() {
  setmenu(0);
  lcd.begin(16, 2);
  lcd.setCursor(0, 1);
  lcd.clear();
  pinMode(buzz, OUTPUT);
  Serial.begin(9600); // Starting Serial Communication
  pinMode(pingPin, OUTPUT); // initialising pin 30 as output
  pinMode(echoPin, INPUT); // initialising pin 31 as input

  pinMode(angPin, INPUT);
  pinMode(SensorPin, INPUT);

  pinMode(LeftMotorForward1, OUTPUT);
  pinMode(LeftMotorReverse1, OUTPUT);
}
void show_time_interval_menu() {
  char key = cusKeypad.getKey();
  lcd.setCursor(0, 0);
  lcd.print(String("Time:0.Menu"));
  lcd.setCursor(0, 1);
  lcd.print(String("1.S 2.Min 3.Hr"));
  if (key == '0') {
    lcd.clear();
    setmenu(0);
  }
  if(key=='1') {
    lcd.clear();
    setmenu(2);
  }
  if(key=='2') {
    lcd.clear();
    setmenu(3);
  }
  if(key=='3') {
    lcd.clear();
    setmenu(4);
  }
}
void show_water_volume() {

  //ultrasonic

  long duration, inches, cm;

  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);

  digitalWrite(pingPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(pingPin, LOW);


  duration = pulseIn(echoPin, HIGH); // using pulsin function to determine total time
  cm = microsecondsToCentimeters(duration); // calling method

  double litre_value = 3.1416 * 5 * 5 * cm; //calculating water volume by taking the height and considering the tank as a cylinder which has 5cm radious

  litre_value = (double)(litre_value / 1000);
  String message;
  lcd.setCursor(0, 0);
  char key = cusKeypad.getKey();
  if ((int)litre_value < 20) { //if water goes down under 20 litre we show a water level low message
    low_water = true;
    if (buzzer) {
      message = String("Water Level LOW!");
      lcd.print(message);
      tone(buzz, 1000);
      lcd.setCursor(0, 1);
      lcd.print(String("1->stop buzzer   "));
      if (key == '1') {
        noTone(buzz);
        buzzer = false;
      }
    }
    else {
      message = String("Water Level LOW!");
      lcd.print(message);
      lcd.setCursor(0, 1);
      lcd.print(String("1->input TIME   "));

      if (key == '1') {
        lcd.clear();
        setmenu(1);
      }
    }
  }
  else {
    buzzer = true;
    low_water = false;
    message = String("Water:") + String(litre_value) + String(" LITRE");
    lcd.print(message);
    lcd.setCursor(0, 1);
    lcd.print(String("1->input TIME   "));
    noTone(buzz);
    if (key == '1') {
      lcd.clear();
      setmenu(1);
    }
  }
}
void show_seconds_menu() {
  lcd.setCursor(0,0);
  lcd.print(String("*.set #.Menu"));
  lcd.setCursor(0,1);
  lcd.print(String("Value:"));
  char key = cusKeypad.getKey();
  if(key=='#') {
    lcd.clear();
    value="";
    setmenu(0);
  }
  if(key!='#' and key !='*' and value.length()<7) {
    value+=String(key);
    lcd.print(value);
  }
  
  if(key=='*') {
    time_interval_set=true;
    char value_in_char[7];
    //Serial.println(String("value ")+String(value));
    value.toCharArray(value_in_char,value.length()+1);
    time_interval_in_seconds=atol(value_in_char);
    seconds=time_interval_in_seconds;
    //Serial.println(String(String("Got time ")+time_interval_in_seconds));
    time_interval_in_seconds+=(millis()/100);
    
    value="";
    lcd.clear();
    setmenu(0);
  }
}
void show_minutes_menu() {
  lcd.setCursor(0,0);
  lcd.print(String("*.set #.Menu"));
  lcd.setCursor(0,1);
  lcd.print(String("Value:"));
  char key = cusKeypad.getKey();
  if(key=='#') {
    lcd.clear();
    value="";
    setmenu(0);
  }
  if(key!='#' and key !='*' and value.length()<7) {
    value+=String(key);
    lcd.print(value);
  }
  
  if(key=='*') {
    time_interval_set=true;
    char value_in_char[7];
    //Serial.println(String("value ")+String(value));
    value.toCharArray(value_in_char,value.length()+1);
    time_interval_in_seconds=atol(value_in_char);
    time_interval_in_seconds*=60;
    seconds=time_interval_in_seconds;
    //Serial.println(String(String("Got time ")+time_interval_in_seconds));
    time_interval_in_seconds+=(millis()/100);
    
    value="";
    lcd.clear();
    setmenu(0);
  }
}
void show_hours_menu() {
    lcd.setCursor(0,0);
  lcd.print(String("*.set #.Menu"));
  lcd.setCursor(0,1);
  lcd.print(String("Value:"));
  char key = cusKeypad.getKey();
  if(key=='#') {
    lcd.clear();
    value="";
    setmenu(0);
  }
  if(key!='#' and key !='*' and value.length()<7) {
    value+=String(key);
    lcd.print(value);
  }
  if(key=='*') {
    time_interval_set=true;
    char value_in_char[7];
    //Serial.println(String("value ")+String(value));
    value.toCharArray(value_in_char,value.length()+1);
    time_interval_in_seconds=atol(value_in_char);
    time_interval_in_seconds*=60;
    time_interval_in_seconds*=60;
    seconds=time_interval_in_seconds;
    //Serial.println(String(String("Got time ")+time_interval_in_seconds));
    time_interval_in_seconds+=(millis()/100);
    value="";
    lcd.clear();
    setmenu(0);
  }
}
void  water_the_plant() {
  int SensorValue = analogRead(SensorPin);
  float SensorVolts = analogRead(SensorPin)*0.0048828125;
  Serial.println(String(SensorVolts));
  if((int) SensorVolts>=1) {
    lcd.setCursor(0,0);
    lcd.print(String("Open water gate"));
    digitalWrite(LeftMotorReverse1, HIGH);
    digitalWrite(LeftMotorForward1,LOW);
    delay(1000/5);
    lcd.setCursor(0,0);
    lcd.print(String("Watering plants"));
    digitalWrite(LeftMotorReverse1,LOW);
    digitalWrite(LeftMotorForward1,LOW);
    delay(2000/5);
    lcd.setCursor(0,0);
    lcd.print(String("Close water gate"));
    digitalWrite(LeftMotorReverse1, LOW);
    digitalWrite(LeftMotorForward1,HIGH);
    delay(1000/5);
    digitalWrite(LeftMotorReverse1,LOW);
    digitalWrite(LeftMotorForward1,LOW);
   
  }
  else {
    lcd.setCursor(0,0);
    lcd.print(String("Soil is moist"));
    lcd.setCursor(1,1);
    lcd.print(String("Water skipped"));
    delay(2000/5);
  }
  time_interval_in_seconds=(millis()/100)+seconds;
  lcd.clear();
  setmenu(0);
}
void loop() {
  
  if(menu[0]==1) show_water_volume();
  if(menu[1]==1) show_time_interval_menu();
  if(menu[2]==1) show_seconds_menu();
  if(menu[3]==1) show_minutes_menu();
  if(menu[4]==1) show_hours_menu();
   Serial.println(String((millis()/100))+String(" ")+String(time_interval_in_seconds));
  
  if(time_interval_set==true and (millis()/100)>=time_interval_in_seconds) {
    lcd.clear();
    setmenu(5);
  }
  if(menu[5]==1) water_the_plant();
  //SoilSensorCodes
  //int SensorValue = analogRead(SensorPin);
  //float SensorVolts = analogRead(SensorPin)*0.0048828125;

  /*lcd.setCursor(0, 1);
    char key = cusKeypad.getKey();

    val = analogRead(angPin);
    val = map(val, 0, 1023, 0, 255);


    int SensorValue = analogRead(SensorPin);




    if(cm > 250)
    {
    lcd.clear();
    lcd.print("Water level low!");
    delay(500);
    tone(buzz,1000);
    delay(1000);
    noTone(buzz);
    }else
    {
      noTone(buzz);
    }
    if (key == '1')
      {
       lcd.clear();
       lcd.print("mosture value:");
       lcd.print(SensorValue);
      }else if(key == '2')
      {
       lcd.clear();
       lcd.print("Distance: ");
       lcd.print(cm);
       lcd.print("cm");
      }  */
}

long microsecondsToCentimeters(long microseconds) // method to covert microsec to centimeters
{
  return microseconds / 29 / 2;
}
