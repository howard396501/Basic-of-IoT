#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>
#include <Keypad.h>
#include <IRremote.h>
#include <Servo.h>
#define KEY_ROWS 4 // 按鍵模組的列數
#define KEY_COLS 4 // 按鍵模組的行數
#define dht_dpin 2
Servo myservo1 ;
Servo myservo2 ;

int xPin = A1;
int yPin = A0;
int buttonPin = 3;

int xPosition = 0 ;
int yPosition = 0 ;
int buttonState = 0 ;

int joyval ;
dht DHT ;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE) ;  // 設定 LCD I2C 位址

char keymap[KEY_ROWS][KEY_COLS] = 
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
} ;

byte colPins[KEY_COLS] = {9, 8, 7, 6} ;     // 按鍵模組，行1~4接腳。
byte rowPins[KEY_ROWS] = {13, 12, 11, 10} ; // 按鍵模組，列1~4接腳。 
 
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, KEY_ROWS, KEY_COLS) ;

int RelayPin = 5 ;
int RECV_PIN = 4 ;
decode_results results ;
IRrecv irrecv(RECV_PIN) ;
void setup() 
{
  Serial.begin(9600) ; 
  pinMode(RelayPin, OUTPUT) ;
  irrecv.enableIRIn() ; // 啟動紅外線接收 
  pinMode(xPin, INPUT) ;
  pinMode(yPin, INPUT) ;
  pinMode(buttonPin, INPUT) ; 
  myservo1.attach(A2) ;
  myservo2.attach(A3) ;
  lcd.begin(16, 2) ; // 初始化 LCD
  lcd.backlight() ; //開啟背光
  lcd.setCursor(0, 0) ;
  lcd.print("Waiting Command..") ;
}

void loop() 
{
  char key = myKeypad.getKey() ;  
  if (irrecv.decode(&results)) //當紅外線收到訊號
  {
    translateIR() ;
    Serial.print("results value is "); // 輸出解碼後的資料
    Serial.print(results.value, HEX);
    Serial.print(", bits is ");
    Serial.print(results.bits);
    Serial.print(", decode_type is ");
    Serial.println(results.decode_type);
    irrecv.resume() ; // 重新接收下一個訊號
  } 
// ↑紅外線設定完成↑

// ↓鍵盤按下之功能↓    
  if (key == 0x31) //按下"1"
  { 
    lcd.clear() ; 
    DHT.read11(dht_dpin) ;
    lcd.setCursor(0, 0) ; 
    lcd.print("Temp: ") ;
    lcd.print(DHT.temperature) ;
    lcd.print("C") ;    
    lcd.setCursor(0, 1) ;
    lcd.print("Humi: ") ;
    lcd.print(DHT.humidity) ;
    lcd.print("% ") ;
  }
  if(key == 0x30 ) //按下"0"
  {
    lcd.clear() ;
    lcd.setCursor(0, 0) ;
    lcd.print("Waiting Command..") ;
  }
  if(key == 0x32 ) //按下"2"
  {
    lcd.clear() ;
    digitalWrite(RelayPin, HIGH) ;
    lcd.setCursor(0, 0) ;
    lcd.print("Relay On !") ;
    lcd.setCursor(0, 1) ;
    lcd.print("'Off' press 3") ;
  }

  if(key == 0x33 ) //按下"3"
  {
    lcd.clear() ;
    digitalWrite(RelayPin, LOW) ;
    lcd.setCursor(0, 0) ;
    lcd.print("Relay Off !") ;
    lcd.setCursor(0, 1) ;
    lcd.print("'On' press 2") ;
  }
  if(key == 0x34) //按下"4"
  {
    lcd.setCursor(0, 0) ;
    lcd.print("x-axis angle:") ;
    lcd.print(xPosition) ;
    lcd.print(" ") ;
    lcd.setCursor(0, 1) ;  
    lcd.print("y-axis angle:") ;
    lcd.print(yPosition) ;
    lcd.print(" ") ;
  }
  xPosition = analogRead(xPin);
  xPosition = map(xPosition, 0, 1023, 0, 180) ;
  myservo1.write(xPosition) ;

  yPosition = analogRead(yPin);
  yPosition = map(yPosition, 0, 1023, 0, 180) ;
  myservo2.write(yPosition) ;
}
  
void translateIR() //紅外線按下之功能
{
  switch(results.value)
  {
     case 0xFF6897: 
      lcd.clear() ;
      DHT.read11(dht_dpin) ;
      lcd.setCursor(0, 0) ; 
      lcd.print("Temp: ") ;
      lcd.print(DHT.temperature) ;
      lcd.print("C") ;
      lcd.setCursor(0, 1) ;
      lcd.print("Humi: ") ;
      lcd.print(DHT.humidity) ;
      lcd.print("% ") ;
      irrecv.blink13(true) ;
      break ;
    case 0xFF4AB5: 
      lcd.clear() ;
      lcd.setCursor(0, 0) ;
      lcd.print("Waiting Command..") ;
      irrecv.blink13(true) ;    
      break ;
    case 0xFF9867: 
      lcd.clear() ;
      digitalWrite(RelayPin, HIGH) ;
      lcd.setCursor(0, 0) ;
      lcd.print("Relay On !") ;
      lcd.setCursor(0, 1) ;
      lcd.print("'Off' press 3") ;
      irrecv.blink13(true) ;    
      break ;
    case 0xFFB04F: 
      lcd.clear() ;
      digitalWrite(RelayPin, LOW) ;
      lcd.setCursor(0, 0) ;
      lcd.print("Relay Off !") ;
      lcd.setCursor(0, 1) ;
      lcd.print("'On' press 2") ;       
      irrecv.blink13(true) ;
      break ;
    case 0xFF30CF:
      lcd.setCursor(0, 0) ;
      lcd.print("x-axis angle:") ;
      lcd.print(xPosition) ;
      lcd.print(" ") ;
      lcd.setCursor(0, 1) ;  
      lcd.print("y-axis angle:") ;
      lcd.print(yPosition) ;
      lcd.print(" ") ;
      break ;
  }
}



/*void joyservo()
{
    lcd.clear() ;
    for(;;)
    {
      xPosition = analogRead(xPin);
      xPosition = map(xPosition, 0, 1023, 0, 180) ;
      myservo1.write(xPosition) ;
      
      yPosition = analogRead(yPin);
      yPosition = map(yPosition, 0, 1023, 0, 180) ;
      myservo2.write(yPosition) ;
      char key = myKeypad.getKey() ;
      
      lcd.setCursor(0, 0) ;
      lcd.print("x-axis angle:") ;
      lcd.print(xPosition) ;
      lcd.print(" ") ;
      lcd.setCursor(0, 1) ;  
      lcd.print("y-axis angle:") ;
      lcd.print(yPosition) ;
      lcd.print(" ") ;
      if(key)
      {
       break ; 
      }

    }
}*/



