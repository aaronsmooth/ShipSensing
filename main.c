#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <lcd.h>

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
	if (!nothing) {
		for(;;) {
			while(!(wiringPiI2CReadReg8(fd, 0x09) & 0x01));
			//int output = wiringPiI2CReadReg8(fd, 0x3D);
			int xOutput = convertOutput(wiringPiI2CReadReg8(fd, 0x03), wiringPiI2CReadReg8(fd, 0x04));
			int yOutput = convertOutput(wiringPiI2CReadReg8(fd, 0x07), wiringPiI2CReadReg8(fd, 0x08));
			int zOutput = convertOutput(wiringPiI2CReadReg8(fd, 0x05), wiringPiI2CReadReg8(fd, 0x06));
			//printf("\ec");
			//printf("xOutput is: %d \n", xOutput);
			//printf("yOutput is: %d \n", yOutput);
			//printf("zOutput is: %d \n", zOutput);
			lcdPrintf(lcd,"x:%5d y:%5d z:%5d         ",xOutput,yOutput,zOutput);
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
