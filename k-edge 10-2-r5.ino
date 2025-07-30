// K-EDGE-10-2-R6 JAN 18, 2023
// Need to disconnect D8 while loading flash
// connecting AP7 on adafruit causes kb circuit not to work, wonder if under voltage occuring
// next test to key kb response
//
// K-EDGE-10-2-R5 JAN 17, 2023
// Added register 3 code, reg3, for reading KB input from myGamer keyboard
//
// K-EDGE 10-2-R5 JAN 16, 2023
// RE-ADDED FROM 10-2-R4
// *****************************************************************
// Dec 19, 2022
//   -- Added 8255 chip support 
// *****************************************************************
// KNODE10-2-DEV    Nov 19, 2022
// Diana Kanecki, DM, MBA, ACS, Bio. Sci.
//-----------------------------------------------------------------
// Development module to work with MCP23017, OLED, and SIPO/PISO Registers
// Version 1.0
// *****************************************************************
// Blinks an LED attached to a MCP23XXX pin.

// ok to include only the one needed
// both included here to make things simple for example
// MCP 23017 includes
// ****************************************************************
// Nov 27, 2022
// Added two 74595 shift registers as registers 0 and 1 respectively
// using modified SPI shiftOut(*) routines without having to use SPI.h
// software and hardware testing passed
// ***************************************************************
#include <Adafruit_MCP23X08.h>
#include <Adafruit_MCP23X17.h>

// OLED includes
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// MCP 23017 defines
#define LED_PIN 0     // MCP23XXX pin LED is attached to. chip 23017 pin 21 gpa0

// only used for SPI
#define CS_PIN 6

// uncomment appropriate line
//Adafruit_MCP23X08 mcp;
Adafruit_MCP23X17 mcp;

// OLED Defines
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
0b00000001, 0b11000000,
0b00000001, 0b11000000,
0b00000011, 0b11100000,
0b11110011, 0b11100000,
0b11111110, 0b11111000,
0b01111110, 0b11111111,
0b00110011, 0b10011111,
0b00011111, 0b11111100,
0b00001101, 0b01110000,
0b00011011, 0b10100000,
0b00111111, 0b11100000,
0b00111111, 0b11110000,
0b01111100, 0b11110000,
0b01110000, 0b01110000,
0b00000000, 0b00110000 };

// I/O routine defines
volatile int j=0;
volatile int NodeId=0;
char buff[100]= "Hello from Host\n\0";
volatile int index1;
volatile bool receivedone;
int lineCount1 = 0;

// 74595 Register defines
int clkPin =      D5;
int dataInPin =   D6;
int dataOutPin =  D7;

int latchPin0 =   D3;
int latchPin1 =   D4;

// 74165 chip control, reg3
int r3ClkPin  = clkPin;
int r3DataPin = dataInPin;
int r3CE      = 7;  //adafruit PA7
int kbPin     = D8;   // jan 18, 2023 using D8 cause error loading flash, disconnect D8 while loading

// 8255 chip control
int cWR    = 5;  // gpa5
int cA0    = 3;  // gpa3  
int cA1    = 4;  // gpa4 
int cCS    = 2;  // gpa2 
int cRD    = 1;  // gpa1 
int cReset = 6;  // gpa6

// 8255 data path
int OutDB0 = 8;   // gpb0;
int OutDB1 = 9;   // gpb1;
int OutDB2 = 10;  // gpb2;
int OutDB3 = 11;  // gpb3;
int OutDB4 = 12;  // gpb4;
int OutDB5 = 13;  // gpb5;
int OutDB6 = 14;  // gpb6;
int OutDB7 = 15;  // gpb7;

void setupReg3() {
// Jan 17, 2023 --- need to test
// 74165 chip control, reg3
//int r3ClkPin  = clkPin;
//int r3DataPin = dataInPin;
//int r3CE      = 7;  //adafruit PA7
//int kbPin     = D8;
   
  pinMode(kbPin,INPUT);
  mcp.pinMode(r3CE,OUTPUT);
  mcp.digitalWrite(r3CE,HIGH); 
}

void setCSR(){
mcp.digitalWrite(cCS,HIGH);

mcp.digitalWrite(cA0,HIGH);
mcp.digitalWrite(cA1,HIGH);

mcp.digitalWrite(OutDB7,HIGH);
mcp.digitalWrite(OutDB6,LOW);  
mcp.digitalWrite(OutDB5,LOW);
mcp.digitalWrite(OutDB4,LOW);
mcp.digitalWrite(OutDB3,LOW);
mcp.digitalWrite(OutDB2,LOW);
mcp.digitalWrite(OutDB1,LOW);
mcp.digitalWrite(OutDB0,LOW);     

mcp.digitalWrite(cWR,LOW);
mcp.digitalWrite(cCS,LOW);

delay(500);

mcp.digitalWrite(cWR,HIGH);
mcp.digitalWrite(cCS,HIGH);   
}

void setup8255(){
mcp.pinMode(cWR,OUTPUT);  // cWR GPa5;
mcp.pinMode(cA0,OUTPUT);  //cA1 = 4;  // gpa4;
mcp.pinMode(cA1,OUTPUT);  //cA2 = 3;  // gpa3;
mcp.pinMode(cCS,OUTPUT);  //cCS = 2;  // gpa2;
mcp.pinMode(cRD,OUTPUT);  //cRD = 1;  // gpa1;
mcp.pinMode(cReset,OUTPUT);

mcp.pinMode(OutDB0,OUTPUT); // OutDB0 = 8;   // gpb0;
mcp.pinMode(OutDB1,OUTPUT); // OutDB1 = 9;   // gpb1;
mcp.pinMode(OutDB2,OUTPUT); // OutDB2 = 10;  // gpb2;
mcp.pinMode(OutDB3,OUTPUT); // OutDB3 = 11;  // gpb3;
mcp.pinMode(OutDB4,OUTPUT); // OutDB4 = 12;  // gpb4;
mcp.pinMode(OutDB5,OUTPUT); // OutDB5 = 13;  // gpb5;
mcp.pinMode(OutDB6,OUTPUT); // OutDB6 = 14;  // gpb6;
mcp.pinMode(OutDB7,OUTPUT); // OutDB7 = 15;  // gpb7; 

// master reset, initial conditions
mcp.digitalWrite(cCS,LOW);
mcp.digitalWrite(cReset,HIGH);
delay(500);
mcp.digitalWrite(cReset,LOW);
mcp.digitalWrite(cRD,HIGH);
mcp.digitalWrite(cWR,HIGH);
mcp.digitalWrite(cCS,HIGH);

setCSR();                    // all ports output
setupReg3();

// send a test pulse  port a
mcp.digitalWrite(OutDB1,HIGH); //OK
mcp.digitalWrite(OutDB2,HIGH); //OK

mcp.digitalWrite(cA1,LOW); //ok
mcp.digitalWrite(cA0,LOW);  //ok
mcp.digitalWrite(cWR,LOW); // ok
mcp.digitalWrite(cCS,LOW); //ok

delay(500);

mcp.digitalWrite(cWR,HIGH);
mcp.digitalWrite(cCS,HIGH);     

// send a test pulse port B
mcp.digitalWrite(OutDB0,HIGH); //OK
mcp.digitalWrite(OutDB4,HIGH); //OK

mcp.digitalWrite(cA1,LOW); //ok
mcp.digitalWrite(cA0,HIGH);  //ok
mcp.digitalWrite(cWR,LOW); // ok
mcp.digitalWrite(cCS,LOW); //ok

delay(500);

mcp.digitalWrite(cWR,HIGH);
mcp.digitalWrite(cCS,HIGH);  

}

void writePortA(int data1){
volatile int tmp=0;

// send a test pulse  port a
// mcp.digitalWrite(OutDB0,LOW); //OK
// mcp.digitalWrite(OutDB1,HIGH); //OK


Serial.println("In WritePortA   data = ");
Serial.println(data1); 
for (int i=0; i<=7;i++){
tmp = (data1 & 1);
mcp.digitalWrite((OutDB0+i),tmp);
data1 = data1 >> 1;          
//     Serial.print("i= ");
//     Serial.print(i);
//     Serial.print(" tmp=");
//     Serial.print(tmp);
//     Serial.print(" data = ");
//     Serial.println(data1);
//     delay(1000);          
}

mcp.digitalWrite(cA1,LOW); //ok
mcp.digitalWrite(cA0,LOW);  //ok
mcp.digitalWrite(cWR,LOW); // ok
mcp.digitalWrite(cCS,LOW); //ok

delay(10);

mcp.digitalWrite(cWR,HIGH);
mcp.digitalWrite(cCS,HIGH);  

}

void writePortB(int data1){
volatile int tmp=0;

// send a test pulse  port a
// mcp.digitalWrite(OutDB0,LOW); //OK
// mcp.digitalWrite(OutDB1,HIGH); //OK


//  if (data1 == 1)
//   {
//    mcp.digitalWrite(OutDB0,HIGH);
//    delay(1000);
//   }

Serial.println("In WritePortB   data = ");
Serial.println(data1); 
for (int i=0; i<=7;i++){
tmp = (data1 & 1);
mcp.digitalWrite((OutDB0+i),tmp);
data1 = data1 >> 1;          
//     Serial.print("i= ");
//     Serial.print(i);
//     Serial.print(" tmp=");
//     Serial.print(tmp);
//     Serial.print(" data = ");
//     Serial.println(data1);

}

mcp.digitalWrite(cA1,LOW); //ok
mcp.digitalWrite(cA0,HIGH);  //ok
mcp.digitalWrite(cWR,LOW); // ok
mcp.digitalWrite(cCS,LOW); //ok

delay(10);

mcp.digitalWrite(cWR,HIGH);
mcp.digitalWrite(cCS,HIGH); 

//  delay(1000); 
}

void setup() {

// MCP 23017 setup codes
Serial.begin(9600);
//while (!Serial);
Serial.println("MCP23xxx Blink Test!");

// uncomment appropriate mcp.begin
if (!mcp.begin_I2C()) {
//if (!mcp.begin_SPI(CS_PIN)) {
Serial.println("Error.");
while (1);
}

// configure pin for output
mcp.pinMode(LED_PIN, OUTPUT);

Serial.println("Looping...");

// OLED Setup codes
// SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
Serial.println(F("SSD1306 allocation failed"));
for(;;); // Don't proceed, loop forever
}

// Show initial display buffer contents on the screen --
// the library initializes this with an Adafruit splash screen.
display.display();
delay(2000); // Pause for 2 seconds

// Clear the buffer
display.clearDisplay();

// Draw a single pixel in white
display.drawPixel(10, 10, SSD1306_WHITE);

// Show the display buffer on the screen. You MUST call display() after
// drawing commands to make them visible on screen!
display.display();
delay(2000);
// display.display() is NOT necessary after every single drawing command,
// unless that's what you want...rather, you can batch up a bunch of
// drawing operations and then update the screen all at once by calling
// display.display(). These examples demonstrate both approaches...

testdrawline();      // Draw many lines

testdrawrect();      // Draw rectangles (outlines)

testfillrect();      // Draw rectangles (filled)

testdrawcircle();    // Draw circles (outlines)

testfillcircle();    // Draw circles (filled)

testdrawroundrect(); // Draw rounded rectangles (outlines)

testfillroundrect(); // Draw rounded rectangles (filled)

testdrawtriangle();  // Draw triangles (outlines)

testfilltriangle();  // Draw triangles (filled)

testdrawchar();      // Draw characters of the default font

testdrawstyles();    // Draw 'stylized' characters

testscrolltext();    // Draw scrolling text

testdrawbitmap();    // Draw a small bitmap image

// Invert and restore display, pausing in-between
display.invertDisplay(true);
delay(1000);
display.invertDisplay(false);
delay(1000);

testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps

pinMode(clkPin, OUTPUT);
pinMode(dataInPin,INPUT);
pinMode(dataOutPin,OUTPUT);

pinMode(latchPin0,OUTPUT);
pinMode(latchPin1,OUTPUT);    

setup8255();    
}

void writeReg0(int data){
digitalWrite(latchPin0,LOW);
shiftOut(dataOutPin, clkPin, MSBFIRST, data);  
digitalWrite(latchPin0,HIGH);
}

void writeReg1(int data){
digitalWrite(latchPin1,LOW);
shiftOut(dataOutPin, clkPin, MSBFIRST, data);  
digitalWrite(latchPin1,HIGH);
}

void test4(){
writePortA(1);  
display.print(1);
display.display();
delay(500);
writePortA(2);
display.print(2);
display.display();
delay(500); 
writePortA(4);
display.print(4);
display.display();
delay(500);
display.println(8);
display.display();
writePortA(8);  

delay(2000);

display.clearDisplay();  
display.setCursor(0,0);
display.display();

display.println("PIN LOW");
display.display();
mcp.digitalWrite(LED_PIN, LOW);

//writeReg0(4);
//writeReg1(2);
writePortB(1);  

display.print(1);
display.display();
delay(500);
writePortB(2);
display.print(2);
display.display();
delay(500); 
writePortB(4);
display.print(4);
display.display();
delay(500);
writePortB(8);
display.println(8);
display.display();

delay(2000);

}

void test5(){
display.println("---Test 5");
display.display();

writeReg1(0);
display.print(0);
display.display();
delay(1000);

writeReg1(1);
display.print(1);
display.display();
delay(1000);

writeReg1(2);
display.print(2);
display.display();
delay(1000);

display.print(3);
display.display();
writeReg1(3);
delay(1000);  

writeReg1(4);  
display.print(4);
display.display();
delay(1000);

writeReg1(8);
display.print(8);
display.display();
delay(1000);

writeReg1(9);
display.println(9);
display.display();
delay(1000);

}

// ===================== KB UNIT ==========================
int rdReg3() {
  // Jan 17, 2023 --- Need to test

  // 74165 chip control, reg3
  // int r3ClkPin  = clkPin;
  // int r3DataPin = dataInPin;
  // int r3CE      = 7;  //adafruit PA7
  // int kbPin     = D8;

int data=0;

data=shiftIn(r3DataPin, r3ClkPin,MSBFIRST);
return(data);
}


void loop() {
lineCount1=0;
display.setTextSize(1);  

display.display();
display.clearDisplay();  
display.setCursor(0,0);
display.display();


display.print("Hello-KNODE10-2-r3");
display.println(j);
display.display();
delay(2000);    


display.println("PIN HIGH");
display.display();
mcp.digitalWrite(LED_PIN, HIGH);

//writeReg0(2);
//writeReg1(4);

//  test4();  // QC passed

test5();
delay(1000);

display.println("PIN LOW");
display.display();
mcp.digitalWrite(LED_PIN, LOW);

display.println(" ");
display.display();
//writePortA(1);
//writePortB(1);
delay(2000);
j++;  
}

// *******************************************************************
// I/O Routines
// *******************************************************************
// *****************************************************
// *** I/O Routines
// *****************************************************
void writeInt1( int val){
// Sept 1, 2022 - verified to work with formatting  
display.print(val);
display.display();

}

void writeStr1( char str1[]){
// Sept 1, 2022 - verified to work with formatting  
display.print(str1);
display.display();

}

void writeLnStr1( char str1[]){
// Sept 1, 2022 - verified to work with formatting     
display.println(str1);
display.display();
lineCount1++;

if  (lineCount1 > 6)
{
delay(2000);       
display.clearDisplay();
display.setCursor(0,0);
display.display(); 
lineCount1=0;     
}
display.clearDisplay();
display.display();     
}

void writeLn1() {
// Sept 1, 2022 - verified to work with formatting   
display.println();
display.display();
lineCount1++;

if  (lineCount1 > 6)
{
delay(2000);       
display.clearDisplay();
display.setCursor(0,0);
display.display(); 
lineCount1=0;     
}  


}

// *******************************************************************
// OLED Support Code
// *******************************************************************
void testdrawline() {
int16_t i;

display.clearDisplay(); // Clear display buffer

for(i=0; i<display.width(); i+=4) {
display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
display.display(); // Update screen with each newly-drawn line
delay(1);
}
for(i=0; i<display.height(); i+=4) {
display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
display.display();
delay(1);
}
delay(250);

display.clearDisplay();

for(i=0; i<display.width(); i+=4) {
display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
display.display();
delay(1);
}
for(i=display.height()-1; i>=0; i-=4) {
display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
display.display();
delay(1);
}
delay(250);

display.clearDisplay();

for(i=display.width()-1; i>=0; i-=4) {
display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
display.display();
delay(1);
}
for(i=display.height()-1; i>=0; i-=4) {
display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
display.display();
delay(1);
}
delay(250);

display.clearDisplay();

for(i=0; i<display.height(); i+=4) {
display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
display.display();
delay(1);
}
for(i=0; i<display.width(); i+=4) {
display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
display.display();
delay(1);
}

delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) {
display.clearDisplay();

for(int16_t i=0; i<display.height()/2; i+=2) {
display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
display.display(); // Update screen with each newly-drawn rectangle
delay(1);
}

delay(2000);
}

void testfillrect(void) {
display.clearDisplay();

for(int16_t i=0; i<display.height()/2; i+=3) {
// The INVERSE color is used so rectangles alternate white/black
display.fillRect(i, i, display.width()-i*2, display.height()-i*2, SSD1306_INVERSE);
display.display(); // Update screen with each newly-drawn rectangle
delay(1);
}

delay(2000);
}

void testdrawcircle(void) {
display.clearDisplay();

for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
display.display();
delay(1);
}

delay(2000);
}

void testfillcircle(void) {
display.clearDisplay();

for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
// The INVERSE color is used so circles alternate white/black
display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
display.display(); // Update screen with each newly-drawn circle
delay(1);
}

delay(2000);
}

void testdrawroundrect(void) {
display.clearDisplay();

for(int16_t i=0; i<display.height()/2-2; i+=2) {
display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
display.height()/4, SSD1306_WHITE);
display.display();
delay(1);
}

delay(2000);
}

void testfillroundrect(void) {
display.clearDisplay();

for(int16_t i=0; i<display.height()/2-2; i+=2) {
// The INVERSE color is used so round-rects alternate white/black
display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
display.height()/4, SSD1306_INVERSE);
display.display();
delay(1);
}

delay(2000);
}

void testdrawtriangle(void) {
display.clearDisplay();

for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
display.drawTriangle(
display.width()/2  , display.height()/2-i,
display.width()/2-i, display.height()/2+i,
display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
display.display();
delay(1);
}

delay(2000);
}

void testfilltriangle(void) {
display.clearDisplay();

for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
// The INVERSE color is used so triangles alternate white/black
display.fillTriangle(
display.width()/2  , display.height()/2-i,
display.width()/2-i, display.height()/2+i,
display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
display.display();
delay(1);
}

delay(2000);
}

void testdrawchar(void) {
display.clearDisplay();

display.setTextSize(1);      // Normal 1:1 pixel scale
display.setTextColor(SSD1306_WHITE); // Draw white text
display.setCursor(0, 0);     // Start at top-left corner
display.cp437(true);         // Use full 256 char 'Code Page 437' font

// Not all the characters will fit on the display. This is normal.
// Library will draw what it can and the rest will be clipped.
for(int16_t i=0; i<256; i++) {
if(i == '\n') display.write(' ');
else          display.write(i);
}

display.display();
delay(2000);
}

void testdrawstyles(void) {
display.clearDisplay();

display.setTextSize(1);             // Normal 1:1 pixel scale
display.setTextColor(SSD1306_WHITE);        // Draw white text
display.setCursor(0,0);             // Start at top-left corner
display.println(F("Hello, world!"));

display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
display.println(3.141592);

display.setTextSize(2);             // Draw 2X-scale text
display.setTextColor(SSD1306_WHITE);
display.print(F("0x")); display.println(0xDEADBEEF, HEX);

display.display();
delay(2000);
}

void testscrolltext(void) {
display.clearDisplay();

display.setTextSize(2); // Draw 2X-scale text
display.setTextColor(SSD1306_WHITE);
display.setCursor(10, 0);
display.println(F("scroll"));
display.display();      // Show initial text
delay(100);

// Scroll in various directions, pausing in-between:
display.startscrollright(0x00, 0x0F);
delay(2000);
display.stopscroll();
delay(1000);
display.startscrollleft(0x00, 0x0F);
delay(2000);
display.stopscroll();
delay(1000);
display.startscrolldiagright(0x00, 0x07);
delay(2000);
display.startscrolldiagleft(0x00, 0x07);
delay(2000);
display.stopscroll();
delay(1000);
}

void testdrawbitmap(void) {
display.clearDisplay();

display.drawBitmap(
(display.width()  - LOGO_WIDTH ) / 2,
(display.height() - LOGO_HEIGHT) / 2,
logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
display.display();
delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
int8_t f, icons[NUMFLAKES][3];

// Initialize 'snowflake' positions
for(f=0; f< NUMFLAKES; f++) {
icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
icons[f][YPOS]   = -LOGO_HEIGHT;
icons[f][DELTAY] = random(1, 6);
Serial.print(F("x: "));
Serial.print(icons[f][XPOS], DEC);
Serial.print(F(" y: "));
Serial.print(icons[f][YPOS], DEC);
Serial.print(F(" dy: "));
Serial.println(icons[f][DELTAY], DEC);
}

//  for(int i=1; i<=2;i++) { // Loop forever...
display.clearDisplay(); // Clear the display buffer

// Draw each snowflake:
for(f=0; f< NUMFLAKES; f++) {
display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
}

display.display(); // Show the display buffer on the screen
delay(200);        // Pause for 1/10 second

// Then update coordinates of each flake...
for(f=0; f< NUMFLAKES; f++) {
icons[f][YPOS] += icons[f][DELTAY];
// If snowflake is off the bottom of the screen...
if (icons[f][YPOS] >= display.height()) {
// Reinitialize to a random position, just off the top
icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
icons[f][YPOS]   = -LOGO_HEIGHT;
icons[f][DELTAY] = random(1, 6);
}
}
//  }
}


