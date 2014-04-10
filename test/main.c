#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <lcd.h>
#include <math.h>

int convertOutput(int msb, int lsb);

int main(int argc, char *argv[]) {
	int fd;
	fd = wiringPiI2CSetup(0x1E);
	wiringPiSetup();
	int lcd = lcdInit(2,16,4,11,10,0,1,2,3,0,0,0,0);
	lcdClear(lcd);
	lcdHome(lcd);
	int nothing;
	nothing = wiringPiI2CWriteReg8(fd, 0x02, 0x00);
	wiringPiI2CWriteReg8(fd, 0x00, 0x70);
	wiringPiI2CWriteReg8(fd, 0x01, 0xE0);
	if (!nothing) {
		for(;;) {
			while(!(wiringPiI2CReadReg8(fd, 0x09) & 0x01));
			float xOutput = convertOutput(wiringPiI2CReadReg8(fd, 0x03), wiringPiI2CReadReg8(fd, 0x04))/(float)230;
			float yOutput = convertOutput(wiringPiI2CReadReg8(fd, 0x07), wiringPiI2CReadReg8(fd, 0x08))/(float)230;
			float zOutput = convertOutput(wiringPiI2CReadReg8(fd, 0x05), wiringPiI2CReadReg8(fd, 0x06))/(float)230;;
			printf("\ec");
			printf("xOutput is: %6.2f \n", xOutput);
			printf("yOutput is: %6.2f \n", yOutput);
			printf("zOutput is: %6.2f \n", zOutput);
			float magnitude = sqrt(pow(xOutput,2)+pow(yOutput,2)+pow(zOutput,2));
			printf("Gauss Magnitude is: %8.6f\n", magnitude);
			float heading = atan2(yOutput, xOutput);
			if(heading < 0){
				heading += 2*M_PI;
			}else if(heading > 2*M_PI){
				heading -= 2*M_PI;
			}
			heading = heading * (float)180/M_PI;
			printf("Heading in degrees is: %4.1f\n", heading);
			float pitch = atan2(sqrt(pow(xOutput,2)+pow(yOutput,2)), zOutput);
			if(pitch < 0){
			pitch += 2*M_PI;
			}else if(pitch > 2*M_PI){
			pitch -= 2*M_PI;
			}
			pitch = pitch * (float)180/M_PI;
			printf("Pitch in degrees is: %4.1f\n", pitch);
			// LCD screen is formatted as following
			// x-gauss  y-gauss  z-gauss
			// vector-gauss-magnitude  x-y-heading-in-degrees  z-pitch-in-degrees
			// Example:
			// 1234567890123456 16x2 character LCD
			// -1.23-1.23 -1.23
			// 1.234567 360 360
			lcdClear(lcd);
			lcdHome(lcd);
			lcdPrintf(lcd,"% 1.2f% 1.2f % 1.2f%8.6f %3.0f %3.0f",xOutput,yOutput,zOutput,magnitude,heading,pitch);
		}
	}
	return 0;

}
int convertOutput(int msb, int lsb) {
	int result = 0;
	int aMSB = msb & 0x0F;
	int aLSB;
	if (aMSB >= 0x08) { 		//value is negative
		aLSB = ~lsb + 1;
		if (aLSB > 255) {	//carry from LSB to MSB
			aLSB = aLSB & 0xFF;
			aMSB = ~aMSB + 1;
		} else {		//no carry from LSB to MSB
			aMSB = ~aMSB;
		}
		result = ( ((aMSB << 8) & 0xFF0) + aLSB);
	} else { 			//value is positive
		result = aMSB + lsb;
	}
	if (result < -2048) {		//value should never be below -2048
		result =  -2048;
	}
	if (result > 2047 ) {		//value should never be above 2047
		result = 2047;
	}
	return result;
}
