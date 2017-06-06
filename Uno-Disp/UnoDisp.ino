#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_VCOMH0_1_4W_HW_SPI u8g2(U8G2_R0, 9, 8);

void setup()
{
  /* add setup code here */
	u8g2.begin();

	// initialize serial:
	Serial.begin(9600);
}

void loop()
{
	String input = "Hello World!";
	char output[80];

  /* add main program code here */
	while (true)
	{
		u8g2.firstPage();

		if (Serial.available)
		{
			input = Serial.readStringUntil('\n');
			input.toCharArray(output, 80, 0);
		}
			
		do {
				
			u8g2.setFont(u8g2_font_ncenB14_tr);
			u8g2.drawStr(0, 24, output);

		} while (u8g2.nextPage() && Serial.available == 0);
	}
}
