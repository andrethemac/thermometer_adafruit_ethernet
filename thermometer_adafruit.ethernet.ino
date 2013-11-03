/*
temperature logger and display
switches between a bargraph with the average temperature the last 8 hours
and a numeric display.
sends the temp, pressure and humidity to a webserver, every minute
uses arduino ethernet
adafruit 8 x 5 blinky display (neoPixel)
powered using the 5 v and ground, data uses pin 6
sht15 powerd using 5 v and ground, data to pin 3, clock to pin 2
mpl3115a2 pressure sensor powerd using 3 v and ground, sda to sda on arduino, sdc to sdc on arduino

webserver is lighttp on a linux box, webpage writen python.
database is mysql on the same linux box.
*/


/*
MPL3115A2 pressure Sensor Example
 Hardware Connections:
 -VCC = 3.3V0
 -SDA = A4, SCL = A5
 -INT pins can be left unconnected for this demo
 Usage:
 -Serial terminal at 9600bps
 -Prints pressure in meters, temperature in degrees C, with 1/16
 resolution.
 -software enabled interrupt on new data, ~1Hz with full resolution
 -the IIC_support file contains the IIC read and write functions
 -IIC_support.ino is just another .ion sketch that holds functions
 that do not need to be modified.
 */

#include <Wire.h> // for IIC communication

const int SENSORADDRESS = 0x60; // address specific to the MPL3115A1,
// value found in datasheet

/**
 * ReadSHT1xValues
 *
 * Read temperature and humidity values from an SHT1x-series (SHT10,
 * SHT11, SHT15) sensor.
 *
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au>
 * www.practicalarduino.com
 */

// SHT15 uses it's own data protocol

#include <SHT1x.h>

// Specify data and clock connections and instantiate SHT1x object
#define dataPin  3
#define clockPin 2
#define LED 9
SHT1x sht1x(dataPin, clockPin);

// The real world environnement we are going to meassure
float temp_c;
float humidity;
float pressure;

/*
adafruit Matrix Setup
 * using a adafruit neopixel blinky shield with 40 neopixes
 * in a 5 x 8 matrix
 */

#include <avr/pgmspace.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 6

// Color definitions (16 bit)
#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define ORANGE   0xFBC0
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF

// 5 colors for the numbers
const uint16_t colors[] = { 
  BLUE,CYAN,GREEN,ORANGE,RED };

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(40, PIN, NEO_GRB + NEO_KHZ800);


// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 5, PIN,
NEO_MATRIX_BOTTOM
+ NEO_MATRIX_RIGHT 
+ NEO_MATRIX_ROWS 
+ NEO_MATRIX_PROGRESSIVE,
NEO_GRB
+ NEO_KHZ800
);

//The data for the 3x5 pixel font
// these definitions als in progmem (see arduino playground for more info)

PROGMEM prog_uint8_t c0[] = { 
  B11100000,B10100000,B10100000,B10100000,B11100000 };
PROGMEM prog_uint8_t c1[] = { 
  B00100000,B01100000,B10100000,B00100000,B00100000 };
PROGMEM prog_uint8_t c2[] = { 
  B11100000,B00100000,B11100000,B10000000,B11100000 };
PROGMEM prog_uint8_t c3[] = { 
  B11100000,B00100000,B11100000,B00100000,B11100000 };
PROGMEM prog_uint8_t c4[] = { 
  B10100000,B10100000,B11100000,B00100000,B00100000 };
PROGMEM prog_uint8_t c5[] = { 
  B11100000,B10000000,B11100000,B00100000,B11100000 };
PROGMEM prog_uint8_t c6[] = { 
  B11100000,B10000000,B11100000,B10100000,B11100000 };
PROGMEM prog_uint8_t c7[] = { 
  B11100000,B00100000,B00100000,B01000000,B01000000 };
PROGMEM prog_uint8_t c8[] = { 
  B11100000,B10100000,B11100000,B10100000,B11100000 };
PROGMEM prog_uint8_t c9[] = { 
  B11100000,B10100000,B11100000,B00100000,B11100000 };
PROGMEM prog_uint8_t ca[] = { 
  B11100000,B10100000,B11100000,B00100000,B10100000 };
PROGMEM prog_uint8_t cb[] = { 
  B10000000,B10000000,B11100000,B10100000,B11100000 };
PROGMEM prog_uint8_t cc[] = { 
  B00000000,B00000000,B11100000,B10000000,B11100000 };
PROGMEM prog_uint8_t cd[] = { 
  B00100000,B00100000,B11100000,B10100000,B11100000 };
PROGMEM prog_uint8_t ce[] = { 
  B11100000,B10000000,B11100000,B10000000,B11100000 };
PROGMEM prog_uint8_t cf[] = { 
  B11100000,B10000000,B11100000,B10000000,B10000000 };
PROGMEM prog_uint8_t sp[] = { 
  B00000000,B00000000,B01000000,B00000000,B00000000 };

//defintions for the color
unsigned long kleur = 0;

//general definitions
float templist[60];
float tempHistory[8] = {
  0,0,0,0,0,0,0,0};
int lightIntensity = 20;

//now
unsigned long currentMillis;

// 15 second interval for the display
unsigned long previousMillis = 0;
unsigned long interval = 15000;

//1 minute interval
unsigned long previousMinute = 0;
unsigned long intervalMinute = 60000;

// 1 hour interval
unsigned long previousHour = 0;
unsigned long intervalHour = 3600000;

//switches between displays
boolean disp = false;

/*
ethernet setup
 */
#include <SPI.h>
#include <EthernetServer.h>
#include <Ethernet.h> 

// my mac address (see sticker on arduino)
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x8E, 0x23 };

//medion =  my server's dns name
char medion[] = "medion";
EthernetClient client; //(medion, 80);
// msg to send to server
String msg;


void setup()
{
  Wire.begin(); // join i2c bus
  Serial.begin(9600); // start serial for output

  // This is a basic II2 communication check. If the sensor doesn't
  // return decicmal number 196 (see 0x0C register in datasheet),
  // "IIC bad" is printed, otherwise nothing happens.
  if(IIC_Read(0x0C) == 196); //checks who_am_i bit for basic I2C handshake test
  else Serial.println("i2c bad");

  // Enable and configure the sensor.
  sensor_config();

  // adafruit init
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  matrix.begin();
  matrix.setBrightness(10);

  // ethernet setup
  // configure dhcp
  if ( Ethernet.begin(mac) == 0 ) {
    Serial.println("failed to configure dhcp");
    for ( ;; )
      ;
  } 
  else {
    // this is my IP address
    Serial.println( Ethernet.localIP() );
  }
  mydisplay(0);
}

void loop()
{
  // get the environment
  sensor_read_data();

  // Read values from the sensor
  temp_c = sht1x.readTemperatureC();
  humidity = sht1x.readHumidity();
  pressure = sensor_read_data();

  currentMillis = millis();

  if ( currentMillis - previousMinute > intervalMinute) {
    previousMinute = currentMillis;
    bargraphUpdate(temp_c);
    //send the data
    SendMessage();
  }

  if ( currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    disp = !disp;
    if ( disp == true ) {
      mydisplay(temp_c);
    } 
    else {
      bargraph(temp_c);
    }
  }  
}

void SendMessage(){
  //send the temperature messured to the webserver and database
  String msg  = "GET /cgi-bin/t3.py?t=";
  String msg2 = "&h=";
  String msg3 = "&p=";
  String msgend = " HTTP/1.0";
  msg = msg + floatToString(temp_c) + msg2 + floatToString(humidity) + msg3 + floatToString(pressure) + msgend;
  client.connect(medion,80);
  if (client.connected()){
    Serial.println("connected");
    Serial.print(msg);
    client.println(msg);
    client.println();
    client.stop();
  } 
  else {
    Serial.println("error");
  }
  client.stop();
}



boolean check_new()
{
  // This function check to see if there is new data.
  // You can call this function and it will return TRUE if there is
  // new data and FALSE if there is no new data.

  // If INT_SOURCE (0x12) register's DRDY flag is enabled, return
  if(IIC_Read(0x12) == 0x80) // check INT_SOURCE register on
    // new data ready (SRC_DRDY)
  {
    return true;
  }
  else return false;
}

void sensor_config()
{
  // To configure the sensor, find the register hex value and
  // enter it into the first field of IIC_Write (see below). Then
  // fingure out the hex value of the data you want to send.
  //
  // For example:
  // For CTRL_REG1, the address is 0x26 and the data is 0xB9
  // 0xB9 corresponds to binary 1011 1001. Each place holder
  // represents a data field in CTRL_REG1.

  // CTRL_REG1 (0x26): enable sensor, oversampling, altimeter mode
  //  IIC_Write(0x26, 0xB9);
  // barometer
  IIC_Write(0x26, 0x39);

  // CTRL_REG4 (0x29): Data ready interrupt enbabled
  IIC_Write(0x29, 0x80);

  // PT_DATA_CFG (0x13): enable both pressure and temp event flags
  IIC_Write(0x13, 0x07);

  // This configuration option calibrates the sensor according to
  // the sea level pressure for the measurement location

  // BAR_IN_MSB (0x14):
  IIC_Write(0x14, 0xC6);

  // BAR_IN_LSB (0x15):
  IIC_Write(0x15, 0x5B);
}

float sensor_read_data()
{
  // This function reads the pressure and temperature registers, then
  // concatenates the data together, and prints in values of
  // meters for pressure and degrees C for temperature.

  // variables for the calculations
  unsigned long m_pressure, c_pressure, l_pressure;
  float pressure;

  // read registers 0x01 through 0x05
  m_pressure = IIC_Read(0x01);
  c_pressure = IIC_Read(0x02);
  l_pressure = IIC_Read(0x03);

  // here is where we calculate the pressure and temperature
  pressure = (float)((((m_pressure << 8) + c_pressure) << 2) + (l_pressure >> 6)) / 100.0;// l_pressure ) ;

  // wait here for new data
  while(check_new() == false);

  return pressure;

  // once there is new data, it is printed
  //  Serial.print("pressure: "); // in meters
  //  Serial.print(pressure);
  //  Serial.print("hPA / ");
}

// These are the two I2C functions in this sketch.
byte IIC_Read(byte regAddr)
{
  // This function reads one byte over IIC
  Wire.beginTransmission(SENSORADDRESS);
  Wire.write(regAddr); // Address of CTRL_REG1
  Wire.endTransmission(false); // Send data to I2C dev with option
  // for a repeated start. THIS IS
  // NECESSARY and not supported before
  // Arduino V1.0.1!!!!!!!!!
  Wire.requestFrom(SENSORADDRESS, 1); // Request the data...
  return Wire.read();
}

void IIC_Write(byte regAddr, byte value)
{
  // This function writes one byto over IIC
  Wire.beginTransmission(SENSORADDRESS);
  Wire.write(regAddr);
  Wire.write(value);
  Wire.endTransmission(true);
}

String floatToString(float in) {
  String result = "";
  if (in < 0.0) { 
    result = "-"; 
    in = abs(in);
  }
  int intpart = (int)in;
  int frac = (in - intpart) * 100;
  String i = String(intpart);
  String f = String(frac);
  result = result + i + "." + f;
  return result;
}

void bargraphUpdate(float temp){
  for ( int x = 59; x > 0 ; x-- ){
    templist[x] = templist[x-1];
  }
  templist[0] = temp;
}

void bargraph(float temp) {
  //shows the average temperature for the last 8 hours
  //from left (8 hours ago) to right (the actual temperature)
  strip.show();
  templist[0] = temp;
  float tempavg = 0;
  for ( int x = 0; x < 60 ; x++ ) {
    tempavg += templist[x];
  }
  tempHistory[0] = (tempavg /= 60);

  if ( currentMillis - previousHour > intervalHour) {
    previousHour = currentMillis;
    for ( int x = 7; x > 0 ; x--) {
      tempHistory[x] = tempHistory[x-1];
    }
  }
  tempHistory[0] = temp;

  for( int x = 0; x < strip.numPixels(); x++) {
    // what row are we on ?
    switch( x / 8 ) {
    case 0:
      //row 0 (bottom row) is blue (too cold)
      kleur = strip.Color(0,0,lightIntensity);
      break;
    case 1:
      // row 1 = cyan (chilly)
      kleur = strip.Color(0,lightIntensity,lightIntensity);
      break;
    case 2:
      // row 2 = green (oke)
      kleur = strip.Color(0,lightIntensity,0);
      break;
    case 3:
      // row 3 = orange (rather warm)
      kleur = strip.Color(lightIntensity,lightIntensity/2,0);
      break;
    case 4:
      // row 5 (toprow) is red (to hot)
      kleur = strip.Color(lightIntensity,0,0);
      break;
    default:
      // no color
      kleur = strip.Color(0,0,0);
    }
    /*
     * Every row stand for a temperature range
     * row 0 is the temperature up to 15 degrees celcius
     * row 1 is the temperature between 15 and 20
     * row 2 is the temperature between 20 and 25
     * row 3 is the temperature between 25 and 30
     * row 4 is the temperature between 30 and higher
     * */
    // if the pixel presenting the temperature is lower than the history
    // turn the pixel off
    if ( (((x/8)*5)+10) >= tempHistory[x % 8] ) {
      kleur = 0;
    }
    strip.setPixelColor(x,kleur);
  }
  strip.show();
}

void mydisplay(int x) {
  // x is the number we want to display
  // we have only place for 2 digits, so we gonna drop everyting over 100
  x %= 100;
  // clear the display
  matrix.show();
  matrix.fillScreen(BLACK);
  // get the color
  int c = 0;
  if ( x <= 15 ) {
    c = 0;
  } else {
    c = (x - 10) / 5;
  }
  if ( c > 4 ) {
    c = 4;
  }
  uint16_t color = colors[c];
  // first digit (only if x is higher than 10)
  if ( x > 10) {
    myprint(1,0,x/10,color);
  }
  // second digit
  myprint(5,0,x%10,color);
}

void myprint(int x, int y, int c , uint16_t color) {
  // draw the digit in our font
  // uses the adafruit gfx library
  switch(c) {
  case 0:
    matrix.drawBitmap(x,y,c0,3,5,color);
    break;
  case 1:
    matrix.drawBitmap(x,y,c1,3,5,color);
    break;
  case 2:
    matrix.drawBitmap(x,y,c2,3,5,color);
    break;
  case 3:
    matrix.drawBitmap(x,y,c3,3,5,color);
    break;
  case 4:
    matrix.drawBitmap(x,y,c4,3,5,color);
    break;
  case 5:
    matrix.drawBitmap(x,y,c5,3,5,color);
    break;
  case 6:
    matrix.drawBitmap(x,y,c6,3,5,color);
    break;
  case 7:
    matrix.drawBitmap(x,y,c7,3,5,color);
    break;
  case 8:
    matrix.drawBitmap(x,y,c8,3,5,color);
    break;
  case 9:
    matrix.drawBitmap(x,y,c9,3,5,color);
    break;
  default:
    matrix.drawBitmap(x,y,sp,3,5,color);
    break;
  }
}
