#include <RTClib.h>
RTC_DS3231 rtc;

#include <Keypad.h>
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};
byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

#include <Servo.h>
Servo servo;  // create servo object to control a servo

int start[7]; //7th bit is for checking if initialized
int endt[7]; //7th bit is for checking if initialized
bool atpos;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  atpos=0;
  // SETUP RTC MODULE
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }

  // automatically sets the RTC to the date & time on PC this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // manually sets the RTC with an explicit date & time, for example to set
  // January 21, 2021 at 3am you would call:
  // rtc.adjust(DateTime(2021, 1, 21, 3, 0, 0));


  servo.attach(10);  // attaches the servo on pin 10 to the servo object
  servo.write(65);   // set servo initial position to 65 degrees

  start[6]=0;
  start[7]=0;

  pinMode(12, OUTPUT);
  pinMode(13,OUTPUT);
}

char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

long st;
long et;
long nt;



char getKeychar(){
  unsigned long int time;
  time=millis();
  while(millis()-time<3000){
    char key=keypad.getKey();
    if(key){
      Serial.println(key);
      return key;
    }
  }
  return 'D';
}

void loop() {
  // put your main code here, to run repeatedly:
  
  char key = keypad.getKey();
  if (key=='#'){
    Serial.print("#");
    digitalWrite(12,HIGH);
    delay(1000);
    start[0] = getKeychar()-48;
    start[1] = getKeychar()-48;
    start[2] = getKeychar()-48;
    start[3] = getKeychar()-48;
    start[4] = getKeychar()-48;
    start[5] = getKeychar()-48;
    start[6] = 1;
    for(int i=0;i<=5;i++){
      if(start[i]>=9){
        start[6]=0;
      }
    }
    digitalWrite(12,LOW);
  }
  if (key=='*'){
    Serial.print("*");
    digitalWrite(13,HIGH);
    endt[0] = getKeychar()-48;
    endt[1] = getKeychar()-48;
    endt[2] = getKeychar()-48;
    endt[3] = getKeychar()-48;
    endt[4] = getKeychar()-48;
    endt[5] = getKeychar()-48;
    endt[6] = 1;
    for(int i=0;i<=5;i++){
      if(endt[i]>=9){
        endt[6]=0;
      }
    }
    digitalWrite(13,LOW);
  }
  DateTime now = rtc.now();
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);
  Serial.print(start[6]);
  Serial.println(endt[6]);
  delay(1000);
  //unit8_t if the data type for date time
  if(start[6]==1 && endt[6]==1){ //work only if start and end time has been initialized
    //st is end time
    st=0; 
    for(int i=0;i<6;i++){
      st=st*10+start[i];
    }
    //et is endtime
    et=0; 
    for(int i=0;i<6;i++){
      et=et*10 +endt[i];
    }
    //nt is present time
    nt=(long)now.hour(); 
    nt=nt*100+(long)now.minute();
    nt=nt*100+(long)now.second(); 
    Serial.println(st);
    Serial.println(et);
    Serial.println(nt);
    //st,et and nt are used for time comparison
    if(nt>=st && nt<=et){
      Serial.print("I was here");
      if(atpos==0){
        for (int pos = 65; pos <= 120; pos += 1) {  // rotate slowly from 65 degrees to 120 degrees, one by one degree
          servo.write(pos);  // control servo to go to position in variable 'pos'
          delay(10);         // waits 10ms for the servo to reach the position
        }
        atpos=1;
      }
    }
    else{
      if(atpos==1){
        for (int pos = 120; pos >= 65; pos -= 1) {  // rotate slowly from 120 degrees to 65 degrees, one by one degree
          servo.write(pos);  // control servo to go to position in variable 'pos'
          delay(10);         // waits 10ms for the servo to reach the position
        }
        atpos=0;
      }
    }
  }
}