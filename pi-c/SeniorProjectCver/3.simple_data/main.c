
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xbee.h>

#include <wiringPi.h>
#include <wiringSerial.h>


char message[8] = "00000000";
int leng = 8;
/*
void myCB(struct xbee *xbee, struct xbee_con *con, struct xbee_pkt **pkt, void **data) {
	if ((*pkt)->dataLen > 0) {
		if ((*pkt)->data[0] == '@') {
			xbee_conCallbackSet(con, NULL, NULL);
			printf("*** DISABLED CALLBACK... ***\n");
		}
		printf("rx: [%s]\n", (*pkt)->data);
	}
	printf("tx: %d\n", xbee_conTx(con, NULL, "Hello\r\n"));
}
*/
void setMessage(char firstChar, time_t rawtime) {
	struct tm * timeinfo;
	time(&rawtime);
        timeinfo = localtime (&rawtime);

        message[0] = firstChar + '0';
        message[1] = timeinfo->tm_sec + '0'; //0 seconds
        message[2] = timeinfo->tm_min + '0'; //31 minutes
        message[3] = timeinfo->tm_hour + '0'; //20th hour or 8pm
        message[4] = timeinfo->tm_wday + '0'; //
        message[5] = timeinfo->tm_mday + '0';
        message[6] = (timeinfo->tm_mon + 1) + '0';//months since  Jan
        message[7] = (timeinfo->tm_year - 100) + '0';//years since 1900
}

int main(void) {
	int fd;
        time_t rawtime;
	void *d;
	struct xbee *xbee;
	struct xbee_con *con;
	struct xbee_conAddress address;
	struct xbee_pkt *pkt;
	xbee_err ret;
	int *datLen;
	datLen = &leng;

	if ((ret = xbee_setup(&xbee, "xbeeZB", "/dev/ttyAMA0", 9600)) != XBEE_ENONE) {
		printf("ret: %d (%s)\n", ret, xbee_errorToStr(ret));
		return ret;
	}

	memset(&address, 0, sizeof(address));
	address.addr64_enabled = 1;
	address.addr64[0] = 0x00;
	address.addr64[1] = 0x13;
	address.addr64[2] = 0xA2;
	address.addr64[3] = 0x00;
	address.addr64[4] = 0x40;
	address.addr64[5] = 0xA0;
	address.addr64[6] = 0x4B;
	address.addr64[7] = 0x60;
	if ((ret = xbee_conNew(xbee, &con, "Data", &address)) != XBEE_ENONE) {
		xbee_log(xbee, -1, "xbee_conNew() returned: %d (%s)", ret, xbee_errorToStr(ret));
		return ret;
	}

	if ((ret = xbee_conDataSet(con, xbee, NULL)) != XBEE_ENONE) {
		xbee_log(xbee, -1, "xbee_conDataSet() returned: %d", ret);
		return ret;
	}
/*
	if ((ret = xbee_conCallbackSet(con, myCB, NULL)) != XBEE_ENONE) {
		xbee_log(xbee, -1, "xbee_conCallbackSet() returned: %d", ret);
		return ret;
	}
*/
	int i;
	/* kick off the chain reaction! */
	for (;;) {
		void *p;
		setMessage('H', rawtime);
		/* send a message */
		xbee_conTx(con, NULL, message);
		/* print the sent message */
		printf("\nTx Message : ");
		for (i = 0; i < 8;i ++) {
			printf("%x ", message[i] - '0');
		}
		printf("\n");
		//usleep(2000000);
		/* block until a message was received */
		while((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE);
			//printf("%s %i\n ",xbee_errorToStr(ret),ret);

		printf("\n");
		
		/* If the packet length was 27 then it's a valid message */
		//if (pkt->dataLen == 27) {
			for (i = 0; i < leng; i++) {
				message[i] = pkt->data[18 + i];	//update message with received data
			}
			printf("Rx Data:     ");
			for (i = 0; i < leng; i++) {
				printf("%x ", message[i]);
			}
		//}
			//usleep(1000000);
		if ((ret = xbee_pktFree(pkt)) != XBEE_ENONE) {
			printf("%s %i\n", xbee_errorToStr(ret), ret);
		}
			//usleep(1000000);

		if (p == NULL) break;

		//usleep(1000000);
	}

	if ((ret = xbee_conEnd(con)) != XBEE_ENONE) {
		xbee_log(xbee, -1, "xbee_conEnd() returned: %d", ret);
		return ret;
	}

	xbee_shutdown(xbee);

	return 0;
}
