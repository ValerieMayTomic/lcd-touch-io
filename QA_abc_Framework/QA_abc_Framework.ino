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
  lcd.begin(20, 4);
  delay(100);
  mpr121QuickConfig();
  lcd.print("Welcome to QA!");
  delay(1000);
  lcd.clear();
}

void loop() {
  int num_correct = QArepl();
  endGame(num_correct);
  exit(0);
}

/*
   REPL that asks user each question in Q_ARRAY, and
   checks each answer against A_ARRAY. Returns the
   number of correct answers.
*/
int QArepl() {
  int correct = 0;
  for (int i=0; i < NUMQ; i++) {
    if (getResponse(askQ(q_array[i])) == a_array[i]){
      correct++;
    }
  }
  return correct;
}

/*
 * Checks the length of the question being asked, and
 * returns the number of rows it will take up.
 */
int askQ(String question){
 printQuestion(question+":");
 if(question.length() < 20)
   return 1;
 else if(question.length() < 40)
   return 2;
 else
   return 3;//Question will be cut down to three lines if longer
}

/*
 * Waits for touchpad input, looping on the same question until
 * the enter is pressed. Returns the user's response as a string
 */
String getResponse(int qRows) {
  String response = "";
  boolean entered = false;
  int touchNumber;
  int newKey = -1;
  lcd.setCursor(0,qRows);
  while(!entered)
  {
    while(checkInterrupt() && (newKey == -1));
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
       if ((touchstatus & (1<<DELETE)) || (newKey == DELETE)){
        deleteLast(response, qRows);
        newKey = -1;
       }
      else if ((touchstatus & (1<<PQRS)) || (newKey == PQRS))
        newKey = keyPressed(response,PQRS,1,qRows);
      else if ((touchstatus & (1<<GHI))|| (newKey == GHI))
        newKey = keyPressed(response,GHI,1,qRows);
      //else if (touchstatus & (1<<CHECK))
        //response = keyPressed(response,CHECK,1,qRows);
      else if ((touchstatus & (1<<SPACE))|| (newKey == SPACE)){
        response.concat(' ');
        newKey = -1;
      }
      else if ((touchstatus & (1<<TUV)) || (newKey == TUV))
        newKey = keyPressed(response,TUV,1,qRows);
      else if ((touchstatus & (1<<JKL)) || (newKey == JKL))
        newKey = keyPressed(response,JKL,1,qRows);
      else if ((touchstatus & (1<<ABC)) || (newKey == ABC))
        newKey = keyPressed(response,ABC,1,qRows);
      else if ((touchstatus & (1<<ENTER)) || (newKey == ENTER)){
        entered = true;
        newKey = -1;
      }
      else if ((touchstatus & (1<<WXYZ)) || (newKey == WXYZ))
         newKey = keyPressed(response,WXYZ,1,qRows);
      else if ((touchstatus & (1<<MNO)) || (newKey == MNO))
        newKey = keyPressed(response,MNO,1,qRows);
      else if ((touchstatus & (1<<DEF)) || (newKey == DEF))
        newKey = keyPressed(response,DEF,1,qRows);
    }
    else{
      newKey = -1;
    }
    response = printResponse(response, qRows);
  }
  lcd.clear();
  return response;
}

/*
 * A function waiting for touchpad input
 * after a key has been pressed. An int is
 * returned representing a new key press,
 * or -1 if the key press times out.
 */
int keyPressed(String &response, int prevKey, int pressCount, int qRows){
  confirm(response, prevKey, pressCount);
  printResponse(response, qRows);
  unsigned long cur_time = millis(); 
  int cur_key = -1;
  boolean touchDetected = false;
  while(!touchDetected){
    
    while(checkInterrupt() && ((millis() - cur_time) < 1200));
    
    touchstatus = mpr121Read(0x01) << 8;
    touchstatus |= mpr121Read(0x00);
    
    if ((millis() - cur_time) > 1200){
      return -1;
    }
    
    int touchNumber = 0; 
    touchstatus = mpr121Read(0x01) << 8;
    touchstatus |= mpr121Read(0x00);
      
    for (int j=0; j<12; j++){
      if ((touchstatus & (1<<j)))
      touchNumber++;
    }
      
    if (touchNumber == 1){
      touchDetected = true;
      if (touchstatus & (1<<DELETE))
       cur_key = DELETE;
      else if (touchstatus & (1<<PQRS))
        cur_key = PQRS;
      if (touchstatus & (1<<GHI))
       cur_key = GHI;
      //else if (touchstatus & (1<<CHECK))
        //do nothing
      else if (touchstatus & (1<<SPACE))
        cur_key = SPACE;
      else if (touchstatus & (1<<TUV))
        cur_key = TUV;
      else if (touchstatus & (1<<JKL))
        cur_key = JKL;  
      else if (touchstatus & (1<<ABC))
        cur_key = ABC;
      else if (touchstatus & (1<<ENTER))
        cur_key = ENTER;
      else if (touchstatus & (1<<WXYZ))
        cur_key = WXYZ;
      else if (touchstatus & (1<<MNO))
        cur_key = MNO;
      else if (touchstatus & (1<<DEF))
        cur_key = DEF; 
    }
  }
  if(cur_key == prevKey){
    deleteLast(response, qRows);
    keyPressed(response, cur_key, pressCount+1, qRows);
  }
  else{
    return cur_key;
  }
}

/* A function which adds the appropriate character to the
 *  response string depending on which key has been
 *  pressed how many times.
 */
void confirm(String &response, int key, int pressCount){
  if (key == PQRS){
    switch(pressCount%4){
      case 1:
        response.concat('p');
        break;
      case 2:
        response.concat('q');
        break;
      case 3:
        response.concat('r');
        break;
      case 0:
        response.concat('s');
        break;
    }
  }
  else if (key == GHI){
    switch(pressCount%3){
      case 1:
        response.concat('g');
        break;
      case 2:
        response.concat('h');
        break;
      case 0:
        response.concat('i');
        break;
    }
  }
  //else if (touchstatus & (1<<CHECK))
    //
  else if (key == TUV){
    switch(pressCount%3){
      case 1:
        response.concat('t');
        break;
      case 2:
        response.concat('u');
        break;
      case 0:
        response.concat('v');
        break;
    }
  }
  else if (key == JKL){
    switch(pressCount%3){
      case 1:
        response.concat('j');
        break;
      case 2:
        response.concat('k');
        break;
      case 0:
        response.concat('l');
        break;
    }
  }
  else if (key == ABC){
    switch(pressCount%3){
      case 1:
        response.concat('a');
        break;
      case 2:
        response.concat('b');
        break;
      case 0:
        response.concat('c');
        break;
    }
  }
  else if (key == WXYZ){
    switch(pressCount%4){
      case 1:
        response.concat('w');
        break;
      case 2:
        response.concat('x');
        break;
      case 3:
        response.concat('y');
        break;
      case 0:
        response.concat('z');
        break;
    }
  }
  else if (key == MNO){
    switch(pressCount%3){
      case 1:
        response.concat('m');
        break;
      case 2:
        response.concat('n');
        break;
      case 0:
        response.concat('o');
        break;
    }
  }
  else if (key == DEF){
    switch(pressCount%3){
      case 1:
        response.concat('d');
        break;
      case 2:
        response.concat('e');
        break;
      case 0:
        response.concat('f');
        break;
    }
  }
}

/*
 * Removes the final character from the response string
 */
void deleteLast(String &response, int qRows){
  int len = response.length();
  if (len > 0 && len <= 20){
    response.remove(len-1);
    lcd.setCursor(len-1,qRows);
    lcd.print(' ');
  }
  else if (len > 0 && len <= 40){
    response.remove(len-1);
    lcd.setCursor(len-21,qRows+1);
    lcd.print(' ');
  }
  else if (len > 0 && len <= 60){
    response.remove(len-1);
    lcd.setCursor(len-41,qRows+2);
    lcd.print(' ');
  }
}

/*
 * A function that prints the response string
 * with proper word wrapping for 20x4 display,
 * cutting off any letters entered outside the
 * range of the display and returning the
 * response string.
 */
String printResponse(String response, int qRows){
  int rowLimit = 4 - qRows;
  int len = response.length();
     if((len <= 20) && (rowLimit > 0)){
        lcd.setCursor(0,qRows);
        lcd.print(response);
        return response;
     }
     else if((len <= 40) && (rowLimit > 1)){
        lcd.setCursor(0,qRows);
        for(int i=0; i<20; i++)
          lcd.print(response[i]);
        lcd.setCursor(0, qRows+1);
        for(int i=20; i<len; i++)
          lcd.print(response[i]);
          return response;
     }
     else if((len <= 60) && (rowLimit > 2)){
        lcd.setCursor(0,qRows);
        for(int i=0; i<20; i++)
          lcd.print(response[i]);
        lcd.setCursor(0, qRows+1);
        for(int i=20; i<40; i++)
          lcd.print(response[i]);
        lcd.setCursor(0, qRows+2);
        for(int i=40; i<len; i++)
          lcd.print(response[i]);
        return response;
     }
     else{
      response.remove(len-(len-(20*rowLimit)));
      return printResponse(response, qRows);
     }
}

/*
 * Prints the question with proper word
 * wrapping for a 20x4 display. Any
 * questions longer than 3 lines are cut
 * down.
 */
void printQuestion(String question){
  int len = question.length();
     if(len <= 20){
        lcd.setCursor(0,0);
        lcd.print(question);
     }
     else if(len <= 40){
        lcd.setCursor(0,0);
        for(int i=0; i<20; i++)
          lcd.print(question[i]);
        lcd.setCursor(0, 1);
        for(int i=20; i<len; i++)
          lcd.print(question[i]);
     }
     else if(len <= 60){
        lcd.setCursor(0,0);
        for(int i=0; i<20; i++)
          lcd.print(question[i]);
        lcd.setCursor(0, 1);
        for(int i=20; i<40; i++)
          lcd.print(question[i]);
        lcd.setCursor(0, 2);
        for(int i=40; i<len; i++)
          lcd.print(question[i]);
     }
     else{
      lcd.setCursor(0,0);
        for(int i=0; i<20; i++)
          lcd.print(question[i]);
        lcd.setCursor(0, 1);
        for(int i=20; i<40; i++)
          lcd.print(question[i]);
        lcd.setCursor(0, 2);
        for(int i=40; i<55; i++)
          lcd.print(question[i]);
        lcd.print("...?:");
     }
}

//endGame function
void endGame(int correct) {
  lcd.noBlink();
  if (correct == NUMQ){
    lcd.print("You win!");
    delay(1000);
    lcd.setCursor(0,2);
    lcd.print("The code is 951.");
  }
  else{
    String response = "You got ";
    response.concat(correct);//lol ASCII
    response.concat(" of ");
    response.concat(NUMQ);
    response.concat("!");
    lcd.print(response);
    lcd.setCursor(0,2);
    delay(1000);
    lcd.print("Let's try again...");
    delay(1000);
    lcd.clear();
    endGame(QArepl());
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
