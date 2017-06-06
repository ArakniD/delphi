//Reads all traffic on CAN0 and forwards it to CAN1 (and in the reverse direction) but modifies some frames first.
// Required libraries
#include <U8x8lib.h>
#include <U8g2lib.h>
#include "variant.h"
#include <due_can.h>


U8G2_SH1106_128X64_VCOMH0_1_4W_HW_SPI u8g2(U8G2_R0, 9, 8);

//Leave defined if you use native port, comment if using programming port
//#define Serial SerialUSB

void setup()
{
	/* add setup code here */
	u8g2.begin();

  Serial.begin(115200);
  
  // Initialize CAN0 and CAN1, Set the proper baud rates here
  Can0.begin(CAN_BPS_500K);
  Can1.begin(CAN_BPS_500K);
  //Can0.setBigEndian(false);
  //Can1.setBigEndian(false);
  Can0.watchFor();
  Can1.watchFor();

  //By default there are 7 mailboxes for each device that are RX boxes
  //This sets each mailbox to have an open filter that will accept extended
  //or standard frames
  int filter;
  //extended
  for (filter = 0; filter < 3; filter++) {
	  Can0.setRXFilter(filter, 0, 0, true);
	  Can1.setRXFilter(filter, 0, 0, true);
  }
  //standard
  for (int filter = 3; filter < 7; filter++) {
	  Can0.setRXFilter(filter, 0, 0, false);
	  Can1.setRXFilter(filter, 0, 0, false);
  }
}

byte CalculateVoltage(float voltage)
{
	//y = 46x - 479
	// Sane limits
	if (voltage > 14.0)
		voltage = 14.0;
	else if (voltage < 12)
		voltage = 12.0;

	return (byte)((voltage * 46) - 479);
}

void sendVoltageSetPointData(float voltage)
{
	// 1D7 8 80 77 00 00 00 00 00 00
	// For 13.0 vdc output.

	CAN_FRAME outgoing;
	outgoing.id = 0x1D7;
	outgoing.extended = false;
	outgoing.length = 8;
	outgoing.priority = 0; //0-15 lower is higher priority
	
	outgoing.data.byte[0] = 0x80;
	outgoing.data.byte[1] = CalculateVoltage(voltage);
	outgoing.data.byte[2] = 0x00;
	outgoing.data.byte[3] = 0x00;
	outgoing.data.byte[4] = 0x00;
	outgoing.data.byte[5] = 0x00;
	outgoing.data.byte[6] = 0x00;
	outgoing.data.byte[7] = 0x00;
	Can0.sendFrame(outgoing);
}

void printFrame(CAN_FRAME &frame) {
	Serial.print("ID: 0x");
	Serial.print(frame.id, HEX);
	Serial.print(" Len: ");
	Serial.print(frame.length);
	Serial.print(" Data: 0x");
	for (int count = 0; count < frame.length; count++) {
		Serial.print(frame.data.bytes[count], HEX);
		Serial.print(" ");
	}
	Serial.print("\r\n");
}

void loop(){
  CAN_FRAME incoming;
  static unsigned long lastTime = 0;
  float setPointVoltage = 14.0;
  String input = "Hello World!";
  char output[80];
  input.toCharArray(output, 80, 0);

  sprintf(output, "Set: %.2fV", setPointVoltage);

  u8g2.firstPage();
  do
  {
	  u8g2.setFont(u8g2_font_ncenB14_tr);
	  u8g2.drawStr(0, 24, output);

	  while (lastTime++ % 1000000 != 0);
	  
	  u8g2.nextPage();

	  if (Can0.available() > 0) {
		  Can0.read(incoming);
		  printFrame(incoming);
	  }

	  sendVoltageSetPointData(setPointVoltage);

  } while (true);
}


