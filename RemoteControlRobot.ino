/* Copyright Michael Rahr
 * This project is based on the ArduinoNunchuck library.
 *
 * 
 *
 * 
 *
 */

#include <Wire.h>
#include "message.h"
#include <ArduinoNunchuk.h>
#include <SoftwareSerial.h>
//#define Debug

SoftwareSerial radio_serial(11, 12); // RX, TX

#define BAUDRATE	115200
const unsigned int	timeOut = 100; 
unsigned long		sendTimer;
static struct debug_info di;


ArduinoNunchuk nunchuk = ArduinoNunchuk();

#define cmd 13			//Pin 13 for command pin
//char inData[20];		// Allocate some space for the string
//char inChar;			// Where to store the character read
byte index = 0;			// Index into array; where to store the character
bool gotData = false;


byte X_val;
byte Y_val;
byte lastValue;
int readings = 0;
byte minX, maxX, minY, maxY, readX, readY;
byte lastX, lastY;

void setup()
{

	pinMode(cmd, OUTPUT);		//Set command pin to LOW to enforce command mode
	digitalWrite(cmd, LOW);		



#ifdef DEBUG
	Serial.begin(BAUDRATE);
#endif // DEBUG

	nunchuk.init();
	minX = minY = 140;


#ifdef DEBUG
	Serial.println("Setting up Software Serial for the OpenSmart");
#endif // DEBUG

	
	
//******************************************************************************************
//Setup the radio to 19200 BAUD
//This is done by setting it into command mode and send "AT+BAUD=4"
//And then ste it back into transiver mode

	radio_serial.begin(9600);


#ifdef DEBUG
	radio_serial.println("AT+INF");
	delay(500);

	while (radio_serial.available()) {
		Serial.write(radio_serial.read());
	}
#endif // DEBUG


//********************************************************************************************************
//Radio setup
	delay(100);
	radio_serial.println("AT+BAUD=4");		//Send the AT command 
	delay(200);

	radio_serial.begin(4);	//Setup Radio to Baud 6 = 57600
	radio_serial.end();
	digitalWrite(cmd, HIGH);	//Change the Command pin to HIGH for the device to go to command mode.

	delay(200);					//Wait 200mS for the device to be ready
	radio_serial.begin(19200);	//Open the serial port to the Radio
	delay(200);					//Wait 200ms Be starting to communicate
//********************************************************************************************************



	//Pre defined values for the min and max values 
	maxX = 233;
	minX = 32;
	maxY = 225;
	minY = 33;

	//Calibrate the reading from the Nunchuck
	while (readings < 250) {
		readings++;
		nunchuk.update();
		readX = nunchuk.analogX;
		readY = nunchuk.analogY;

		if (readX > maxX) maxX = readX;
		if (readX < minX) minX = readX;
		if (readY > maxY) maxY = readY;
		if (readY < minY) minY = readY;
		delay(10);
	}
	

	nunchuk.update();
	X_val = map(nunchuk.analogX, minX - 2, maxX + 2, 0, 255);
	Y_val = map(nunchuk.analogY, minY - 2, maxY + 2, 0, 255);
	lastX = X_val;
	lastY = Y_val;
	lastValue = 0;
	sendTimer = millis();
}
//******************************************************************************************


void loop()
{
	nunchuk.update();


	X_val = map(nunchuk.analogX, minX - 2, maxX + 2, 0, 255);
	Y_val = map(nunchuk.analogY, minY - 2, maxY + 2, 0, 255);
	Serial.println(X_val);
	delay(10); //10 millisec delay for each loop


	//We have 14 byte to fill in
	//0-4 will be byte
	//5-6-7-8     float
	//9-10-11-12  float
	//13		  byte
	//We fill in the datastruct on each loop, this should ensure that we never end up sending a direction
	//for more than 100 mSec even if the device miss the stop command
	di.id = messageId::sendSteeringCmd;
	di.data1 = 0x00;
	di.data2 = 0x00;
	di.data3 = 0x00;				//Could be used for something with the Wii buttons
	di.data4 = 0x00;				//Could be used for something with the Wii buttons
	di.PidSetpoint = 0.0;
	di.SelfBalancePidSetpoint = 0.0;
	di.end = endOfMessage;


	if (X_val > 160) { di.data1 = 0x04; di.data2 = X_val; }
	if (X_val < 100) { di.data1 = 0x03; di.data2 = 255 - X_val; }
	if (Y_val > 160) { di.data1 = 0x02; di.data2 = Y_val; }
	if (Y_val < 100) { di.data1 = 0x01; di.data2 = 255 - Y_val; }


	/*Send debug info each 100 mSec or when data change*/
	if ((lastValue != di.data1) || (millis() >= sendTimer))
	{
		sendTimer = +timeOut; //Add timeout value to the Sentimer, for the next resend, currently 100mSec
		lastValue = di.data1;
		radio_write((char *)&di);
	}


}

//******************************************************************************************************************************************************
void radio_write(char* data) {
	radio_serial.write((uint8_t*)data, sizeof(debug_info));
}

