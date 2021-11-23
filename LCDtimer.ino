#include <LiquidCrystal.h>
#include<EEPROM.h>
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int stsp = A5;
const int reset = A4;
const int set = A3;
const int inc = A2;
const int dec = A1;
const int buzz = 0;
const int preAlarm = 2;
const int timer_alive = 1;
const int levelOne = 3;
const int levelTwo = 4;
const int levelThree =5;
int alarm_level = -1;
int hrs = 0;
int Min = 0;
int sec = 0;
int timer_gap = 0; //TIMER GAP FOR HIGHER LEVEL
int warning_time = 3;
unsigned int check_val = 50;
int add_chk = 0;
int add_hrs = 1;
int add_min = 2;
int add_sec = 3;
bool deadAlarm = false;
bool RUN = false;
bool sec_flag = true;
bool min_flag = true;
bool hrs_flag = true;
long previousMillis = 0;  
long interval = 1000;
long buzz_interval = 1000;
long previousMillis_reset = 0;  
long interval_reset = 5000;
bool stsp_press = false;
unsigned long currentMillis = 0;
unsigned long currentMillis_reset = 0;

void setup()
{ 
  Serial.begin(9600);
  Serial.print("PROG START\n");
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   DEADMAN");
  lcd.setCursor(0, 1);
  lcd.print("    ALARM");
  pinMode(stsp, INPUT_PULLUP);
  pinMode(inc, INPUT_PULLUP);
  pinMode(dec, INPUT_PULLUP);
  pinMode(set, INPUT_PULLUP);
  pinMode(reset, INPUT_PULLUP);
  pinMode(buzz, OUTPUT);
  pinMode(timer_alive, OUTPUT);
  pinMode(preAlarm, OUTPUT);
  pinMode(levelOne, OUTPUT);
  pinMode(levelTwo, OUTPUT);  
  pinMode(levelThree, OUTPUT);    
  digitalWrite(timer_alive, LOW);
  digitalWrite(buzz, LOW);
  if (EEPROM.read(add_chk) != check_val)
  {
    EEPROM.write(add_chk, check_val);
    EEPROM.write(add_hrs, 0);
    EEPROM.write(add_min, 1);
    EEPROM.write(add_sec, 2);
  }
  else
  {
    hrs = EEPROM.read(add_hrs);
    Min = EEPROM.read(add_min);
    sec = EEPROM.read(add_sec);
  }
  delay(500);
  INIT();
}

void loop()
{
  if (digitalRead(stsp) == LOW)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    Serial.println("Start button pressed");
    lcd.print("ALARM TIMER"); // monitor milllis
    RUN = true;
    while (RUN)
    {
      
      if(digitalRead(reset) == LOW){
        currentMillis_reset = millis();
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("ALARM TIMER");
        RESET();
        }
      currentMillis = millis();
      digitalWrite(timer_alive, HIGH);
      
      if(sec <= warning_time){
        lcd.setCursor(1,0);
        lcd.print("WARNING: RESET");
        digitalWrite(buzz, HIGH);
        digitalWrite(preAlarm, HIGH);
        alarm_level = 0;     
        }
       TIMER_DECREMENT_PRINT();
      if (hrs == 0 && Min == 0 && sec == 0){
        Serial.println("inside level 1 if");
        digitalWrite(preAlarm, HIGH);
        digitalWrite(levelOne, HIGH);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  LEVEL 1 ALARM");
        alarm_level = 1;
        lcd.setCursor(0,1);
        Serial.println("alarm level value: " + String(alarm_level));
        RESET_TIME();
        while( (alarm_level==1) && (digitalRead(reset)==HIGH) ){
           TIMER_DECREMENT_PRINT();
           if (hrs == 0 && Min == 0 && sec == 0){
               Serial.println("inside level 2 if");
              digitalWrite(preAlarm, HIGH);
              digitalWrite(levelOne, HIGH);
              digitalWrite(levelTwo, HIGH);
              alarm_level = 2;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("  LEVEL 2 ALARM");
              Serial.println("alarm level value: " + String(alarm_level));
              RESET_TIME();
              while( (alarm_level==2) && (digitalRead(reset)==HIGH) ){
                
                TIMER_DECREMENT_PRINT();
                if (hrs == 0 && Min == 0 && sec == 0){
                  Serial.println("inside level 3 if");
                  digitalWrite(preAlarm, HIGH);
                  digitalWrite(levelOne, HIGH);
                  digitalWrite(levelTwo, HIGH);
                  digitalWrite(levelThree, HIGH);
                  lcd.clear();
                  lcd.setCursor(0,0);
                  lcd.print("  LEVEL 3 ALARM");
                  Serial.println("alarm level value: " + String(alarm_level));
                  alarm_level = 3;
                  RESET_TIME();
                  while((alarm_level==3) && (digitalRead(reset)==HIGH)){
                    TIMER_DECREMENT_PRINT();
                    if (hrs == 0 && Min == 0 && sec == 0){
                      lcd.setCursor(0,1);
                      lcd.print("   DEAD ALARM  ");
                      Serial.println("inside level final stage IF");
                      while(digitalRead(reset)==HIGH){
                        //PROGRAM LOOP STUCK WHILE NOT RESET
                        }//while RESET is still not pressed after 3 levels
                      }//if level 3 time expired
                    }//while alarm_level == 3
                  }//if level 2 time expired
                  
                }//while alarm_level == 2
            }//if level 1 time expired
           
          }//while alarm_level == 1
        
        digitalWrite(timer_alive, LOW);
        lcd.setCursor(4, 0);
        RUN = false;

         
        INIT();
      }//if timer is at 0:0:0
    }//while TIMER RUN=true
  }//end of if START BUTTON PRESSED
  EDIT_TIME();

  if(digitalRead(reset)==LOW){
    RESET();
    }
}

void RESET_TIME_HIGHER_LEVEL(){
  currentMillis = millis();
  hrs = EEPROM.read(add_hrs) - timer_gap;
  Min = EEPROM.read(add_min) - timer_gap;
  sec = EEPROM.read(add_sec) - timer_gap;
  }
void RESET_TIME(){
  currentMillis = millis();
  hrs = EEPROM.read(add_hrs);
  Min = EEPROM.read(add_min);
  sec = EEPROM.read(add_sec);
  }

void RESET()
{
  currentMillis = millis();
  hrs = EEPROM.read(add_hrs);
  Min = EEPROM.read(add_min);
  sec = EEPROM.read(add_sec);

  if(alarm_level==0){
    digitalWrite(preAlarm, LOW);
    digitalWrite(buzz, LOW);
    alarm_level=-1;
    }
  if(alarm_level==1){
    digitalWrite(preAlarm, LOW);
    digitalWrite(levelOne, LOW);
    digitalWrite(buzz, LOW);
    alarm_level=-1;
    }
  if(alarm_level==2){
    digitalWrite(preAlarm, LOW);
    digitalWrite(levelOne, LOW);
    digitalWrite(levelTwo, LOW);
    digitalWrite(buzz, LOW);
    alarm_level=-1;
    }
  if(alarm_level==3){
    digitalWrite(preAlarm, LOW);
    digitalWrite(levelOne, LOW);
    digitalWrite(levelTwo, LOW);
    digitalWrite(levelThree, LOW);
    digitalWrite(buzz, LOW);
    alarm_level=-1;
    }

   Serial.println("Resetting with alarm level: " + String(alarm_level));
}


void TIMER_DECREMENT_PRINT(){
        currentMillis = millis();
        if((currentMillis - previousMillis) > interval) {
        previousMillis = currentMillis;   
        sec = sec - 1;
        }
        if (sec == -1){
        sec = 59;
        Min = Min - 1;
         }
      if (Min == -1){
          Min = 59;
          hrs = hrs - 1;
        }
      if (hrs == -1) hrs = 0;
      lcd.setCursor(4, 1);
      if (hrs <= 9){
        lcd.print('0');
        }
      lcd.print(hrs);
      lcd.print(':');
      if (Min <= 9){
        lcd.print('0');
        }
      lcd.print(Min);
      lcd.print(':');
      if (sec <= 9){
        lcd.print('0');
       }
      lcd.print(sec);
}
void INIT()
{
  hrs = EEPROM.read(add_hrs);
  Min = EEPROM.read(add_min);
  sec = EEPROM.read(add_sec);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Start Countdown");
  lcd.setCursor(4, 1);
  if (hrs <= 9)
  {
    lcd.print('0');
  }
  lcd.print(hrs);
  lcd.print(':');
  if (Min <= 9)
  {
    lcd.print('0');
  }
  lcd.print(Min);
  lcd.print(':');
  if (sec <= 9)
  {
    lcd.print('0');
  }
  lcd.print(sec);
  sec_flag = true;
  min_flag = true;
  hrs_flag = true;
  //delay(500);
}



void EDIT_TIME(){
  if ((digitalRead(set) == LOW) & RUN==false)
  {
    delay(500);
    while (sec_flag)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SET SECONDS: ");
      lcd.print(sec);
      delay(100);
      if (digitalRead(inc) == LOW)
      {
        sec = sec + 1;
        if (sec >= 60) sec = 0;
        delay(100);
      }
      if (digitalRead(dec) == LOW)
      {
        sec = sec - 1;
        if (sec <= -1) sec = 0;
        delay(100);
      }
      if (digitalRead(set) == LOW)
      {
        sec_flag = false;
        delay(250);
      }
    }
    while (min_flag)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SET MINUTE: ");
      lcd.print(Min);
      delay(100);
      if (digitalRead(inc) == LOW)
      {
        Min = Min + 1;
        if (Min >= 60) Min = 0;
        delay(100);
      }
      if (digitalRead(dec) == LOW)
      {
        Min = Min - 1;
        if (Min <= -1) Min = 0;
        delay(100);
      }
      if (digitalRead(set) == LOW)
      {
        min_flag = false;
        delay(250);
      }
    }
    while (hrs_flag)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SET HOUR: ");
      lcd.print(hrs);
      delay(100);
      if (digitalRead(inc) == LOW)
      {
        hrs = hrs + 1;
        if (hrs > 23) hrs = 0;
        delay(100);
      }
      if (digitalRead(dec) == LOW)
      {
        hrs = hrs - 1;
        if (hrs <= -1) hrs = 0;
        delay(100);
      }
      if (digitalRead(set) == LOW)
      {
        hrs_flag = false;
        delay(250);
      }
    }
    if (hrs == 0 && Min == 0 && sec == 0)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  INVALID TIME");
      delay(2000);
    }
    else
    {
      EEPROM.write(add_hrs, hrs);
      EEPROM.write(add_min, Min);
      EEPROM.write(add_sec, sec);
    }
    INIT();
  }
}
