/*
   Q&A Framework Code
   by: Valerie May Tomic
   Escape Rhode Island
   6/9/16

   An interactive question and answer framework using
   the MPR121 Touchpad and the 16x2 LCD-09053 ROHS display.
   Place spefic questions/answers and win results in the
   appropriate locations in the data.h header.

   Hardware: fill in later
*/

//include touchpad library code:
#include "mpr121.h"
#include "i2c.h"

//Touchpad globals
#define MPR121_R 0xB5  // ADD pin is grounded
#define MPR121_W 0xB4 // So address is 0x5A
int irqpin = 7;  // D7
uint16_t touchstatus;

// include the lcd library code:
#include <LiquidCrystal.h>
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//include user Q/A information:
#include "mydata.h"

void setup() {
  //interupt pin init
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH);

  //i2c and lcd init
  Serial.begin(9600);
  DDRC |= 0b00010011;
  PORTC = 0b00110000;  // Pull-ups on I2C Bus
  i2cInit();
  lcd.begin(16, 2);
  delay(100);
  mpr121QuickConfig();
  lcd.print("Welcome to QA!");
  delay(3000);
}

void loop() {
  int num_correct = QArepl();
  lcd.print(endGame(num_correct));
  exit(0);
}

/*
   REPL that asks user each question in Q_ARRAY, and
   checks each answer against A_ARRAY. Returns the
   number of correct answers.
*/
int QArepl() {
  int correct = 0;
  for (int i=0; i <= NUMQ; i++) {
    askQ(q_array[i]);
    if (getResponse() == a_array[i]){
      correct++;
    }
  }
  return correct;
}

void askQ(String question){
 lcd.print(question+":");
}

String getResponse() {
  String response = "";
  boolean entered = false;
  int touchNumber;
  while(!entered)
  {
    while(checkInterrupt());
    touchNumber = 0;
    
    touchstatus = mpr121Read(0x01) << 8;
    touchstatus |= mpr121Read(0x00);
    
    for (int j=0; j<12; j++)  // Check how many electrodes were pressed
    {
      if ((touchstatus & (1<<j)))
        touchNumber++;
    }
    
    if (touchNumber == 1)
    {
      if (touchstatus & (1<<DELETE)){
        int len = response.length();
        if (len > 0)
          response[len-1] = '\0';
      }
      else if (touchstatus & (1<<SEVEN))
        response.concat('7');
      else if (touchstatus & (1<<FOUR))
        response.concat('4');
      else if (touchstatus & (1<<ONE))
        response.concat('1');
      else if (touchstatus & (1<<ZERO))
        response.concat('0');
      else if (touchstatus & (1<<EIGHT))
        response.concat('8');
      else if (touchstatus & (1<<FIVE))
        response.concat('5');
      else if (touchstatus & (1<<TWO))
        response.concat('2');
      else if (touchstatus & (1<<ENTER))
        entered = true;
      else if (touchstatus & (1<<NINE))
        response.concat('9');
      else if (touchstatus & (1<<SIX))
        response.concat('6');
      else if (touchstatus & (1<<THREE))
        response.concat('3');

      lcd.setCursor(0,1);
      lcd.print(response);
      lcd.print("________________");
    }
  }
  return response;
}

//endGame function
String endGame(int correct) {
if (correct == NUMQ)
    return ("You won!");
 else{
    String response = "You got ";
    response.concat(correct+48);//lol ASCII
    response.concat(" of ");
    response.concat(NUMQ+48);
    return(response);
 }
}

//TOUCHPAD FUNCTIONS
byte mpr121Read(uint8_t address) {
  byte data;

  i2cSendStart();
  i2cWaitForComplete();

  i2cSendByte(MPR121_W);  // write 0xB4
  i2cWaitForComplete();

  i2cSendByte(address); // write register address
  i2cWaitForComplete();

  i2cSendStart();

  i2cSendByte(MPR121_R);  // write 0xB5
  i2cWaitForComplete();
  i2cReceiveByte(TRUE);
  i2cWaitForComplete();

  data = i2cGetReceivedByte();  // Get MSB result
  i2cWaitForComplete();
  i2cSendStop();

  cbi(TWCR, TWEN);  // Disable TWI
  sbi(TWCR, TWEN);  // Enable TWI

  return data;
}

void mpr121Write(unsigned char address, unsigned char data) {
  i2cSendStart();
  i2cWaitForComplete();

  i2cSendByte(MPR121_W);// write 0xB4
  i2cWaitForComplete();

  i2cSendByte(address); // write register address
  i2cWaitForComplete();

  i2cSendByte(data);
  i2cWaitForComplete();

  i2cSendStop();
}

void mpr121QuickConfig(void) {
  // Section A
  // This group controls filtering when data is > baseline.
  mpr121Write(MHD_R, 0x01);
  mpr121Write(NHD_R, 0x01);
  mpr121Write(NCL_R, 0x00);
  mpr121Write(FDL_R, 0x00);

  // Section B
  // This group controls filtering when data is < baseline.
  mpr121Write(MHD_F, 0x01);
  mpr121Write(NHD_F, 0x01);
  mpr121Write(NCL_F, 0xFF);
  mpr121Write(FDL_F, 0x02);

  // Section C
  // This group sets touch and release thresholds for each electrode
  mpr121Write(ELE0_T, TOU_THRESH);
  mpr121Write(ELE0_R, REL_THRESH);
  mpr121Write(ELE1_T, TOU_THRESH);
  mpr121Write(ELE1_R, REL_THRESH);
  mpr121Write(ELE2_T, TOU_THRESH);
  mpr121Write(ELE2_R, REL_THRESH);
  mpr121Write(ELE3_T, TOU_THRESH);
  mpr121Write(ELE3_R, REL_THRESH);
  mpr121Write(ELE4_T, TOU_THRESH);
  mpr121Write(ELE4_R, REL_THRESH);
  mpr121Write(ELE5_T, TOU_THRESH);
  mpr121Write(ELE5_R, REL_THRESH);
  mpr121Write(ELE6_T, TOU_THRESH);
  mpr121Write(ELE6_R, REL_THRESH);
  mpr121Write(ELE7_T, TOU_THRESH);
  mpr121Write(ELE7_R, REL_THRESH);
  mpr121Write(ELE8_T, TOU_THRESH);
  mpr121Write(ELE8_R, REL_THRESH);
  mpr121Write(ELE9_T, TOU_THRESH);
  mpr121Write(ELE9_R, REL_THRESH);
  mpr121Write(ELE10_T, TOU_THRESH);
  mpr121Write(ELE10_R, REL_THRESH);
  mpr121Write(ELE11_T, TOU_THRESH);
  mpr121Write(ELE11_R, REL_THRESH);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  mpr121Write(FIL_CFG, 0x04);

  // Section E
  // Electrode Configuration
  // Enable 6 Electrodes and set to run mode
  // Set ELE_CFG to 0x00 to return to standby mode
  mpr121Write(ELE_CFG, 0x0C); // Enables all 12 Electrodes
  //mpr121Write(ELE_CFG, 0x06);   // Enable first 6 electrodes

  // Section F
  // Enable Auto Config and auto Reconfig
  /*mpr121Write(ATO_CFG0, 0x0B);
    mpr121Write(ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   mpr121Write(ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    mpr121Write(ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
}

byte checkInterrupt(void){
  if (digitalRead(irqpin)) {
    return 1;
  }
  return 0;
}
