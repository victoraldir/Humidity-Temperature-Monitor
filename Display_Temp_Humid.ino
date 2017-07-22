/*-----( Import needed libraries )-----*/
#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>

/*-----( Declare Constants )-----*/
// for DHT11, 
//      VCC: 5V or 3V
//      GND: GND
//      DATA: 2
int pinDHT11 = 2;
SimpleDHT11 dht11;
const int timeDisplay = 5000; //5 seconds
const int interruptPin = 3;
const int power5V = 5;
volatile unsigned long lastInterrupt;
volatile boolean flagShowTemp;

/*-----( Declare objects )-----*/
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x20, 4, 5, 6, 0, 1, 2, 3, 7, NEGATIVE);  // Set the LCD I2C address


void setup() {

  // Start Serial DEBUG
  Serial.begin(115200);

  // initialize the pushbutton pin as an input:
  pinMode(interruptPin, INPUT_PULLUP);
  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(interruptPin), pin_ISR, CHANGE);

  pinMode(power5V, OUTPUT);
  digitalWrite(power5V, HIGH);

  setupLCD();

  flagShowTemp = true;
  
}


void pin_ISR() {

    Serial.println("");
    Serial.print("lastInterrupt: "); Serial.println(lastInterrupt);

   if(millis() - lastInterrupt > 5){ // we set a 10ms no-interrupts window
    lastInterrupt = millis();
    flagShowTemp = true;
  }
  
}

void loop() {

  if(flagShowTemp){
    turn5VOn();
    // put your main code here, to run repeatedly:
    Serial.println("Firing printTempHumidity()");
    printTempHumidity();
  
  
    turn5Voff();
    flagShowTemp = false;
  }
  yield();

}

//Print the temperature and humidity on the LCD for 5 seconds
void printTempHumidity(){

  lcd.backlight(); // turn backlight on

  unsigned long timeStart = millis(); //unsigned long to fit the values till milis() rollover
  Serial.print("timeStart: "); Serial.print(timeStart);
  unsigned long remaining = 0;
  Serial.print("remaining: "); Serial.print(remaining);

  boolean flag = true;
  while(flag){

    remaining = millis() - timeStart;
    
    if(remaining > timeDisplay){
        flag = false;
    }
    
    // read without samples.
    byte temperature = 0;
    byte humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      lcd.setCursor(0,1);
      //lcd.print("                "); //Cleaning second line
      lcd.print("err= " + err); 
      Serial.println(err);
      delay(1000);
      //setup();
      //return;
    }
  
    lcd.setCursor(2,1);
    lcd.print((int)temperature); 
    lcd.setCursor(4,1);
    lcd.print("*C");
  
    lcd.setCursor(10,1);
    lcd.print((int)humidity);
    lcd.setCursor(13,1);
    lcd.print("%");

    // DHT11 sampling rate is 1HZ.
    delay(1500);
    yield();
  }

  lcd.noBacklight(); // turn backlight off
}

void turn5Voff(){
  digitalWrite(power5V, LOW);
}

void turn5VOn(){
  digitalWrite(power5V, HIGH);
  setupLCD();
}

void setupLCD(){
  
  lcd.begin(16,2);         // 16x2 segments

  // ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++){
    lcd.backlight();
    delay(250);
    lcd.noBacklight();
    delay(250);
  }

  lcd.backlight(); //Leave it on
  
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("Temp. and humid.");

  lcd.setCursor(0,1); //Line 2
  lcd.print("T: *");

  lcd.setCursor(8,1);
  lcd.print("H: *");
}


