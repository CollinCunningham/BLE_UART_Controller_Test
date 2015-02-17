// BLE Controller Test
// Example of parsing Controller/Sensor data received from Bluefruit LE Connect app
// Requires Bluefruit LE hardware & Adafruit_BLE_UART library
//

#include <SPI.h>
#include "Adafruit_BLE_UART.h"

#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RST 9
#define ADAFRUITBLE_RDY 2

Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

unsigned long time = 0l;
boolean connection = false;
uint8_t btm = 65;
uint8_t out = btm;
uint8_t cap = 90;

#define persec 30
#define sendat (1000/persec)

/**************************************************************************/
/*!
 This function is called whenever select ACI events happen
 */
/**************************************************************************/
void aciCallback(aci_evt_opcode_t event)
{
  switch(event)
  {
  case ACI_EVT_DEVICE_STARTED:
    Serial.println(F("Advertising started"));
    break;
  case ACI_EVT_CONNECTED:
    Serial.println(F("Connected!"));
    connection = true;
    break;
  case ACI_EVT_DISCONNECTED:
    Serial.println(F("Disconnected or advertising timed out"));
    connection = false;
    break;
  default:
    break;
  }
}

/**************************************************************************/
/*!
 This function is called whenever data arrives on the RX channel
 */
/**************************************************************************/
void rxCallback(uint8_t *buffer, uint8_t len)
{
  //  char pfx[2] = { (char)buffer[0], (char)buffer[1] };
  if ((char)buffer[0] == '!') {  //Sensor data flag
    switch ((char)buffer[1]) {
    case 'Q':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Quaternion Data:");
      printQuaternionData(buffer);
      break;
    case 'A':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Accelerometer Data:");
      printSensorDataXYZ(buffer);
      break; 
    case 'G':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Gyro Data:");
      printSensorDataXYZ(buffer);
      break;
    case 'M':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Magnetometer Data:");
      printSensorDataXYZ(buffer);
      break;
    case 'L':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Location Data:");
      printLocationData(buffer);
      break;
    case 'B':
      if (checkCRC(buffer) == false) { break; }
      Serial.print("Button ");
      Serial.print((char)buffer[2]);
      Serial.print(" ");
      printButtonState((char)buffer[3]);
      break;
    case 'C':
      if (checkCRC(buffer) == false) { break; }
      Serial.println("Color ");
      printColorData(buffer);
      break;
    default:
      Serial.println("Unknown Data Type");
      break;
    }
    Serial.println("");
  }

  /* Echo the same data back! */
  //  uart.write(buffer, len);
}

/**************************************************************************/
/*!
 Configure the Arduino and start advertising with the radio
 */
/**************************************************************************/
void setup(void)
{ 
  Serial.begin(9600);
  Serial.println(F("Arduino setup"));

  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  uart.begin();
}

/**************************************************************************/
/*!
 Constantly checks for new events on the nRF8001
 */
/**************************************************************************/
void loop()
{

  uart.pollACI();

}


void printSensorDataXYZ(uint8_t *buffer) {

  float x = *( (float*)(buffer + 2) );
  Serial.print("x = ");
  Serial.println(x, 7);

  float y = *( (float*)(buffer + 6) );
  Serial.print("y = ");
  Serial.println(y, 7);

  float z = *( (float*)(buffer + 10) );
  Serial.print("z = ");
  Serial.println(z, 7); 

}


void printLocationData(uint8_t *buffer) {

  float x = *( (float*)(buffer + 2) );
  Serial.print("lat = ");
  Serial.println(x, 7);

  float y = *( (float*)(buffer + 6) );
  Serial.print("lng = ");
  Serial.println(y, 7);

  float z = *( (float*)(buffer + 10) );
  Serial.print("alt = ");
  Serial.println(z, 7); 

}


void printColorData(uint8_t *buffer) {

  byte r = *(buffer + 2);
  Serial.print("r = ");
  Serial.println(r, DEC);

  byte g = *(buffer + 3);
  Serial.print("g = ");
  Serial.println(g, DEC);

  byte b = *(buffer + 4);
  Serial.print("b = ");
  Serial.println(b, DEC); 

}


void printQuaternionData(uint8_t *buffer) {

  float x = *( (float*)(buffer + 2) );
  Serial.print("x = ");
  Serial.println(x, 7);

  float y = *( (float*)(buffer + 6) );
  Serial.print("y = ");
  Serial.println(y, 7);

  float z = *( (float*)(buffer + 10) );
  Serial.print("z = ");
  Serial.println(z, 7); 

  float w = *( (float*)(buffer + 14) );
  Serial.print("w = ");
  Serial.println(w, 7); 

}


boolean checkCRC(uint8_t *buffer) {

  uint8_t len = sizeof(buffer);
  uint8_t crc = buffer[len-2];
  uint8_t sum = 0;

  for (int i = 0; i < (len-1); i++) {

    sum += buffer[i];

  }

  Serial.print("CRC ");

  if ((crc & ~sum) == 0) {
    Serial.println("PASS");
    return true;
  }

  else {
    Serial.println("FAIL");
    return false;
  }

}


void printButtonState(char buttonState) {

  if (buttonState == '0') {
    Serial.println("released");
  }
  else if (buttonState == '1') {
    Serial.println("pressed"); 
  }
  else {
    Serial.println("unknown state");
  }
}


