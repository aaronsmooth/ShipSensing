
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <xbee.h>
//#include <curl/curl.h>
//#include <wiringPi.h>
//#include <wiringSerial.h>


char message[8] = "00000000";
int leng = 8;

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
	time_t lastHeartbeatTimeStamp;
	time_t messageSentTimeStamp;
	double seconds;
	void *d;
	struct xbee *xbee;
	struct xbee_con *con;
	struct xbee_conAddress address;
	struct xbee_pkt *pkt;
	xbee_err ret;
	int *datLen;
	datLen = &leng;
	int status;	//0 = normal mode, 1 = panic mode, 2 = startup mode
	status = 2;
	double heartbeatTimeElapsed;
	double messageTimeElapsed;
	
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

	int i;
	double temp;
	time(&lastHeartbeatTimeStamp);		//set initial timestamp 
	
	for (;;) {
	  void *p;
	  printf("\nCurrent Status = %i ", status);
	  if (status == 0) {			//normal mode
		/* block until a message was received or 30 minutes have passed since a message was received */
		while(((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE) || (difftime(lastHeartbeatTimeStamp, time(&rawtime))) > 1800);
		/* check if 30 minutes passed that broke the block */
		if (difftime(lastHeartbeatTimeStamp, time(&rawtime)) > 1800)
		{
			time(&lastHeartbeatTimeStamp);
			time(&messageSentTimeStamp);
			setMessage('H', rawtime);
			xbee_conTx(con, NULL, message);
			while(((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE) || (difftime(messageSentTimeStamp, time(&rawtime))) > 120);
			if (difftime(messageSentTimeStamp, time(&rawtime)) > 120) status = 1;
			else
			{       //message was received, process it
                		/* If the packet length was 27 then it's a valid message */
                		//if (pkt->dataLen == 27) {
                        	char temp_char;
                        	for (i = 0; i < leng; i++) {
                                	temp_char = pkt->data[18 + i];
                                	message[i] = temp_char; //update message with received data
                        	}
                        	printf("Rx Data:     ");
                        	for (i = 0; i < leng; i++) {
                                	printf("%x ", message[i]);
                        	}
                        	if (message[0] == 'A') status = 0;      //received acknowledgment, change to normal status
                        	else status = 1;                        //received an unknown message, change to panic status
                        	/* FREE THE PACKET AFTER BEING USED */
                        	if ((ret = xbee_pktFree(pkt)) != XBEE_ENONE) {
                                	printf("%s %i\n", xbee_errorToStr(ret), ret);
                        	}
			}
		}
		else	//a message was received from arduino
		{
		               //message was received, process it
                	/* If the packet length was 27 then it's a valid message */
                	//if (pkt->dataLen == 27) {
                        char temp_char;
                        for (i = 0; i < leng; i++) {
                                temp_char = pkt->data[18 + i];
                                message[i] = temp_char; //update message with received data
                        }
                        printf("Rx Data:     ");
                        for (i = 0; i < leng; i++) {
                                printf("%x ", message[i]);
                        }
                        if (message[0] == 'S' || message[0] == 'N')      //received Ship Arrival/Departure notification, create thread to send data to API
                        {
				
			}
			//else status = 1;                        //received an unknown message, change to panic status
                        /* FREE THE PACKET AFTER BEING USED */
                        if ((ret = xbee_pktFree(pkt)) != XBEE_ENONE) {
                                printf("%s %i\n", xbee_errorToStr(ret), ret);
                        }
                }
	  }
	  if (status == 1) {			//panic mode
		//sound pizzo buzzer
		//blink led
		
	  }
	  if (status == 2) 	//startup mode
	  {
		time(&lastHeartbeatTimeStamp);		//reset the time of the last heartbeat
		time(&messageSentTimeStamp);		//set the time that the last message was sent to Arduino
		setMessage('H', rawtime);		//set the message to be a heart beat
		/* send a message */
		xbee_conTx(con, NULL, message);		//send the message
		/* print the sent message */
		printf("\nTx Message : ");
		for (i = 0; i < 8;i ++) {
			printf("%x ", message[i] - '0');
		}
		printf("\n");
		/* block until a message was received or 2 minutes have passed since message was sent*/
		while(((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE) || (difftime(messageSentTimeStamp, time(&rawtime))) > 120);
			//printf("%s %i\n ",xbee_errorToStr(ret),ret);

		printf("\n");
		if (difftime(messageSentTimeStamp, time(&rawtime)) > 120) {
			status = 1;	//arduino never replied
		}
		else
		{		//message was received, process it
		/* If the packet length was 27 then it's a valid message */
		//if (pkt->dataLen == 27) {
			char temp_char;
			for (i = 0; i < leng; i++) {
				temp_char = pkt->data[18 + i];
				message[i] = temp_char;	//update message with received data
			}
			printf("Rx Data:     ");
			for (i = 0; i < leng; i++) {
				printf("%x ", message[i]);
			}
			if (message[0] == 'A') status = 0;	//received acknowledgment, change to normal status
			else status = 1; 			//received an unknown message, change to panic status
			/* FREE THE PACKET AFTER BEING USED */
			if ((ret = xbee_pktFree(pkt)) != XBEE_ENONE) {
				printf("%s %i\n", xbee_errorToStr(ret), ret);
			}
		}
	  }//end startup status
	
			//usleep(1000000);

	if (p == NULL) break;

	usleep(1000000);
	}

	if ((ret = xbee_conEnd(con)) != XBEE_ENONE) {
		xbee_log(xbee, -1, "xbee_conEnd() returned: %d", ret);
		return ret;
	}

	xbee_shutdown(xbee);

	return 0;
}
