#include <wiringPi.h>
#include <lcd.h>

int main(int argc, char *argv[]) {
	char *msg = argv[0];
	wiringPiSetup();
	int fd =  lcdInit(2,16,4,11,10,0,1,2,3,0,0,0,0);
	lcdClear(fd);
	lcdHome(fd);
	lcdPrintf(fd,"howdy");
	return 0;
}
