
#include<Wire.h>
#define modeOne 41           //electric mode
#define modeTwo 40           //manual mode
#define relay_lock 51        //relay for power lock
#define relay_charge 50      //relay for charging cutoff
void reed();                 //count read function and speed
void mpu();                  //imu unit function


int battery_voltage = A15;   //select the input pin for the voltage divider      
int value = 0;               //variable to store the value coming from the sensor
int voltage;  




const int MPU6050_addr = 0x68; //MPU address initialization
int16_t AccY;
int Accy_1;
int count_reed = 0;           //redd switch count
int a = 0;                    //flag for count
long previousMillis = 0;      //last saved time
double rpm_1;                 //speed
double rpm;
int abc = 0;
long currentMillis;          //current time from start of program
unsigned long time_1;
double speed_kmph;           //speed of vehicle
long distance;               //distance travlled in every 5 sec
#include <LiquidCrystal.h>
const int rs = 8, en = 9, d4 = 10 , d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
float new_distance = 0;




void setup() {
  digitalWrite(reset_1, HIGH);
  Wire.begin();
  Wire.beginTransmission(MPU6050_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  pinMode(relay_lock, OUTPUT);
  pinMode(relay_charge, OUTPUT);
  lcd.begin(20, 4 );         //indicate coloumn and rows of lcd
  Serial.begin(9600);
  pinMode(46, INPUT);        //redd switch input
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(modeOne, INPUT);
  pinMode(modeTwo, INPUT);
  pinMode(reset_1, OUTPUT);
  digitalWrite(modeOne, HIGH);
  digitalWrite(modeTwo, HIGH);
  digitalWrite(reset_1, LOW);
  pinMode(battery_voltage, INPUT);

}
void loop()
{
  value = analogRead(battery_voltage);
  voltage = (value *0.107);
  lcd.print(voltage);
  digitalWrite(relay_charge, HIGH);
  digitalWrite(relay_lock, LOW);

  while(voltage <= 10){

    digitalWrite(relay_charge, LOW);
    lcd.setCursor(1,3);
    lcd.print("BATTERY LOW");
    delay(20);
  }

  
  while(digitalRead(modeOne)==HIGH && digitalRead(modeTwo)==HIGH){ //when system is witch on and no mode is selected
  digitalWrite(relay_lock, LOW);              //turn off power lock
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("V!BICYCLE");
  lcd.setCursor(1, 0);
  lcd.print("VIDYALANKAR MUMBAI");
  delay(10);
  lcd.setCursor(8,3);
  lcd.print("BATTERY:");
  delay(10);
  lcd.setCursor(18,3);
  lcd.print(" %");
  delay(20);
  lcd.setCursor(16,3);
  lcd.print(voltage);
  delay(30);
  digitalRead(modeOne);                      //read switch condition
  digitalRead(modeTwo);                      //read switch condition
  
  }
 


 while (digitalRead(modeOne) == LOW)         //electric mode
  {
  digitalWrite(relay_lock, HIGH);            //turn on power lock
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("ELECTRIC MODE");
  lcd.setCursor(0, 1);
  lcd.print("DISTANCE (km) = ");
  lcd.print(new_distance/1000);
  lcd.setCursor(0, 2);
  lcd.print("SPEED (km/hr) = ");
  lcd.print(rpm);
  lcd.setCursor(18,3);
  lcd.print(" %");
  lcd.setCursor(16,3);
  lcd.print(voltage);
   
    reed();                                 //speed calculation function
    mpu();                                  //slope detection function
    

  }
    
 while (digitalRead(modeTwo) == LOW)        //manual mode
  {
    digitalWrite(relay_lock, LOW);          //turn off power lock
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("MANUAL MODE");
    lcd.setCursor(0,2);
    lcd.print("DISTANCE(km) = ");
    lcd.print(new_distance/1000);
    delay(10);
    reed();
    
    
  }

 
} 

void reed(){
    
    int sensorValue = digitalRead(46);  //read the switch value
    time_1 = millis();                  //time from strat of progarm
    int abc = time_1 - previousMillis;  //time slot

     if (sensorValue == HIGH && a == 0) //switch close
    {

      count_reed = count_reed + 1 ;    //increment count
      a = 1;                           //set flag
      
    }
    else if (sensorValue == LOW)      //reed switch open
    {
      a = 0;                          //clear flag
    }

   if (abc > 5000)                    //update parameters every 5 secs
    {
      rpm = (60 * count_reed) / 5;    //rpm calculation
      rpm = (count_reed * 1.49 * 2) ; //speed calculation     


      previousMillis = time_1;        //save the time at which rpm was calculated
      lcd.print(rpm);
      distance = (2 * 3.14 * 13 * 0.0254 * count_reed); //distance calculation
      new_distance = new_distance + distance; //upadte new distance
      lcd.print(new_distance);
      count_reed = 0;                 //make count 0 after 5 secs
      abc = abc - 5000;               //flag check
      
    }
 }

 void mpu(){


    Wire.beginTransmission(MPU6050_addr);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_addr,14,true);
    AccY = Wire.read() << 8 | Wire.read();
    Accy_1 = AccY / 100;
    if (Accy_1  > 10)                 //check for up slope
    {
      digitalWrite(relay_lock, HIGH); //motor power lock on
    }
    else if (Accy_1 < 10)             //check for down slope
    {
      lcd.clear();
      lcd.setCursor(0,3);
      lcd.print("DOWN SLOPE-MOTOR OFF");
      digitalWrite(relay_lock, HIGH); //motor power lock off

    }
    else
    {
      digitalWrite(relay_lock, LOW); //motor power lock on
    }
   
 }










