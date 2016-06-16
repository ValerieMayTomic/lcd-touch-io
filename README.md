# README #

### About lcd-touch-io ###

lcd-touch-io is a set of programs that implements input/output functionality for an LCD touch screen and a capacitive touch keypad connected to an Arduino Uno. In this case a [typical 16 pin, 5v, 20x4 character display](https://www.amazon.com/RioRand-Module-Arduino-White-Blue/dp/B00GZ6GK7A/ref=sr_1_5?ie=UTF8&qid=1465861278&sr=8-5&keywords=lcd+display) was used, and a [12 button touch keypad.](https://www.sparkfun.com/products/12017)

Swapping for similar hardware will require minor changes in initialization code, as well as edits to hardware-specific functions.

### Individual Program Descriptions ###

These are general descriptions of the programs. See the "How to Use" section for specific instructions regarding the framework's usage.

* lcd_test: A simple Arduino test sketch which ensures that the LCD display is functioning properly with the LiquidCrystal library

* MPR121_Keypad_Example: A more complex Arduino test sketch which implements a basic phone number entry system using the MPR121 keypad and the LCD display

* QA_Framework: An Arduino sketch that provides a complete framework for a question/answer style puzzle using the keypad and LCD display. Answers in the form of numbers.

* QA_abc_Framework: An Arduino sketch that provides a complete framework for a question/answer style puzzle using the keypad and LCD display. Answers in the form of letters.

### How to Use ###

The following instructions apply to both the number and letter versions of the QA Framework.

#### Setting Questions/Answers ####

Inside the header file mydata.h, edit q_array and a_array to contain the questions and answers you desire. You may have as many questions/answers as you like, but the number of questions and answers must be the same. Make sure that the entries on each line are separated by a backslash after the comma.

#### Setting Intro and Endgame ####

To change the start/endgame mechanics, you will need to directly edit the Arduino sketch associated with the framework you're using.

To edit the text that initially appears on the LCD screen (default is "Welcome to QA!"), simply alter the final lines of code in the Arduino setup function.

In order to change what displays at the end of the game, simply edit the endGame function. This function executes a different block of code depending on whether or not the user has answered all questions successfully


### Contact ###

valerie_tomic@brown.edu