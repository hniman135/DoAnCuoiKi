#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

BH1750 lightMeter;
int MODE;
int in;
float valPower;
float power = 0;
float temp = 0;

void setup(){

  pinMode(5, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, rst, LOW); // interrupt 0 ~ pin 2

  Serial.begin(9600);

  Wire.begin();
  lightMeter.begin();

  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();

  Intro();
  config();


}

void loop() {
  AUTO(MODE);  
}

void Intro(){
  int cntIntro = 0;
  // define variables

  int startPoint;
  int endPoint;
  int i, j;

  // speed of the text movement
  int speed = 20;

  // text to display
  String txtMsg = "ENGINE CONTROL";
  //INTRO

    startPoint = 0;   //set starting point
    endPoint = 15;    //set ending point
    lcd.clear();

    //for each letter of the string starting from the last one.
    for (i = txtMsg.length() - 1; i >= 0; i--)
    {
      startPoint = 0;


      //for each position on the LCD display
      for (j = 0; j < endPoint; j++)
      {

        lcd.setCursor(startPoint, 0);
        lcd.print(txtMsg[i]);

        delay(speed);

        if (startPoint != endPoint - 1) {
          lcd.setCursor(startPoint, 0);
          lcd.print(' ');
        }
        startPoint++;
      }
      endPoint--;

      delay(speed);
    }

    // hold the string on the display for 1 sec.
    delay(1000);
}

void AUTO(int Md)
{
  if (Md == 0)
  {
    analogWrite(5, 0); //Turn off engine for safety
    config();
  }

  else if(Md == 1)
  {
    float lux = lightMeter.readLightLevel(); //read value light sensor
    //display
    lcd.setCursor(0, 0);
    lcd.print("LUX: ");
    lcd.setCursor(0, 1);
    lcd.print("POWER(%): ");
    
    //PWM
    if (lux <= 40)
    {
      temp = 51;
      power = 20;
    }
    else if (lux > 40 && lux <=800)
    {
      temp = ((lux -41) * 153 / 759) + 51;
      power = ((lux-41) * 59 / 759) + 20;
    }
    else
    {
      temp = 255;
      power = 100;
    }
    //dispaly read value
    analogWrite(5, temp);
    lcd.setCursor(11, 0);
    lcd.print(lux);
    lcd.setCursor(11, 1);
    lcd.print(power);
    delay(1000);
  }
  else if(Md == 2)
  {
    int tempPow = int (valPower * 2.55);
    analogWrite(5, tempPow);
    lcd.setCursor(0, 1);
    lcd.print("POWER(%): ");
    lcd.setCursor(11, 1);
    lcd.print(valPower);
  }
}
void rst()
{
  MODE = 0;
}

void config()
{
  //display options
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MODE: ");
  lcd.setCursor(6,0);
  lcd.print("1. AUTO");
  lcd.setCursor(6,1);
  lcd.print("2. MANUAL");
  //wait for input
    while(Serial.available()==0)
  {
 
  }
  MODE = Serial.parseFloat();
  //clear serial buffer
  while(Serial.available())
    Serial.read();
  lcd.clear();
  //if choose MODE 2 (MANUAL), must put 1 more 1 input - POWER
  if(MODE == 2 ){
      lcd.setCursor(0, 0);
      lcd.print("Nhap power(%): ");
      while(Serial.available()==0)
    {
  
    }
    valPower = Serial.parseFloat();
    while(Serial.available())
    Serial.read();
    lcd.clear();
  }  
}