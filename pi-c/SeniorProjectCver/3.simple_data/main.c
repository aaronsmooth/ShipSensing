//NO RTC IN THIS VERSION
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
#include <xbee.h> 
#include <curl/curl.h> 
#include <ifaddrs.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <net/if.h> 
#include <errno.h>
//#include <wiringPi.h>
//#include <wiringSerial.h>

/**************VARIABLE DECLARATIONS*****************/
int mmsi[18] = {265491000,248264000,311695000,235068882,248265000,636011023,566086000,266211000,257613000,355488000,257565000,249904000,636015819,311000102,248225000,236111887,357481000,248223000};
char message[8] = "00000000";
int leng = 8;
int HBTimeout = 1800;
int replyTimeout = 120;
int result, strLength;
char globalParams[64];
char * mmsiStr;
char temp_char;
char * temp_array;
int fd, status, mmsiPtr, i;

/********************************************************/
//char localhost[] = "192.168.0.101";

void clearString(char * string) {
	strLength = strlen(string);
	for (i = 0; i < strLength; i++) {
		string[i] = '\0';
	}
	printf("\nArray after clearing: %s\n", string);
}

void setMessage(char firstChar, time_t rawtime) {
	struct tm * timeinfo;
	time(&rawtime);
        timeinfo = localtime (&rawtime);

        message[0] = firstChar + '0';

}


int main(void) {
	int *datLen;
	datLen = &leng;
	time_t rawtime, lastHeartbeatTimeStamp, messageSentTimeStamp;
	double seconds, temp;
	void *d;
	struct xbee *xbee;
	struct xbee_con *con;
	struct xbee_conAddress address;
	struct xbee_pkt *pkt;
	struct timeval tv;
	xbee_err ret;

	status = 2; //0 = normal mode, 1 = panic mode, 2 = startup mode
	double heartbeatTimeElapsed, messageTimeElapsed;
	CURL *curl;
	//CURL *curl2;
	CURLcode res;// res2;
	char apiData[50];
	mmsiPtr = 0;
	struct ifaddrs *myaddrs, *ifa;
	void *in_addr;
	char buf[64];	//the buffer holding the ip address
	char curlStr[64], curlParams[64], tempParams[64], curlMMSI[64], alertType[64];
	//mmsiStr = &curlMMSI;
	//strcpy(curlParams, buf);
	//strcpy(tempParams, buf);
	//strcpy(globalParams, buf);
	//strcpy(curlMMSI, buf);
	//strcpy(alertType, buf);
	//printf("\n%s\n", curlStr);
	clearString(curlParams);
	clearString(tempParams);

	
	srand (time(NULL));
	
	if(getifaddrs(&myaddrs) != 0)
    	{
       		perror("getifaddrs");
        	exit(1);
    	}

    	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next)
    	{
        	if (ifa->ifa_addr == NULL)
            		continue;
        	if (!(ifa->ifa_flags & IFF_UP))
            		continue;

        	switch (ifa->ifa_addr->sa_family)
        	{
            		case AF_INET:
            		{
                		struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
                		in_addr = &s4->sin_addr;
                		break;
            		}

            		case AF_INET6:
            		{
               			struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
                		in_addr = &s6->sin6_addr;
                		break;
            		}

            		default:
                		continue;
        	}

        	if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, buf, sizeof(buf)))
        	{
            		printf("%s: inet_ntop failed!\n", ifa->ifa_name);
        	}
        	else
        	{
            		printf("%s: %s\n", ifa->ifa_name, buf);
        	}
    	}
	strcpy(curlStr, buf);//put IP address into curlStr array
	strcat(curlStr, ":8000/activity");
	
	if ((ret = xbee_setup(&xbee, "xbeeZB", "/dev/ttyUSB0", 9600)) != XBEE_ENONE) {
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
	address.addr64[5] = 0xB1;
	address.addr64[6] = 0xDB;
	address.addr64[7] = 0x2A;
	if ((ret = xbee_conNew(xbee, &con, "Data", &address)) != XBEE_ENONE) {
		xbee_log(xbee, -1, "xbee_conNew() returned: %d (%s)", ret, xbee_errorToStr(ret));
		return ret;
	}

	if ((ret = xbee_conDataSet(con, xbee, NULL)) != XBEE_ENONE) {
		xbee_log(xbee, -1, "xbee_conDataSet() returned: %d", ret);
		return ret;
	}


	time(&lastHeartbeatTimeStamp);		//set initial timestamp 
	
	for (;;) {
	  void *p;
	  
	  printf("\nParams: %s\n", tempParams);
	  
	  printf("\nCurrent Status = %i ", status);
	  if (status == 0) {			//normal mode
		printf("\n");
		/* block until a message was received or 30 minutes have passed since a message was received */
		while(((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE) && !(difftime(time(&rawtime), lastHeartbeatTimeStamp) > HBTimeout));
		/* check if 30 minutes passed that broke the block */
		if (difftime(time(&rawtime), lastHeartbeatTimeStamp) > HBTimeout)
		{
			time(&lastHeartbeatTimeStamp);
			time(&messageSentTimeStamp);
			setMessage('H', rawtime);
			xbee_conTx(con, NULL, message);
			while(((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE) && !(difftime(time(&rawtime), messageSentTimeStamp) > replyTimeout));
			if (difftime(messageSentTimeStamp, time(&rawtime)) > 120) status = 1;
			else
			{       //message was received, process it
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
			}
		}
		else	//a message was received from arduino
		{
			printf("\nPkt length = %i", pkt->dataLen);
			printf("\nElse Statement");
		    //message was received, process it
			for (i = 0; i < leng; i++) {
					temp_char = pkt->data[18 + i];
					message[i] = temp_char; //update message with received data
			}
			printf("\nRx Data:     ");
			for (i = 0; i < leng; i++) {
				printf("%i ", message[i]);
			}
			printf("\n");
			if (message[0] == 'S' || message[0] == 'N')      //received Ship Arrival/Departure notification, create thread to send data to API
			{
				/* In windows, this will init the winsock stuff */ 
  				curl_global_init(CURL_GLOBAL_ALL);
				
				/* get a curl handle */ 
  				curl = curl_easy_init();
				
  				if(curl) 
				{
    					/* First set the URL that is about to receive our POST. This URL can
       					just as well be a https:// URL if that is what should receive the
       					data. */
					
					curl_easy_setopt(curl, CURLOPT_URL, curlStr);
    				/* Now specify the POST data */
					//temp_array = message[0];
					//strcpy(tempParams, curlParams);
					
					
					//strcpy(globalParams, tempParams);
					//printf("\n%s\n", globalParams);
					//curlParams = globalParams;
					//strcat(apiData, fillAPIData()); 
					//strcat(curlParams, "mmsi=248223000&utime=1400114211&st=");
					clearString(curlMMSI);
					strcat(curlParams, "mmsi=");
					sprintf(curlMMSI, "%d", mmsi[rand() % 18]);
					strcat(curlParams, curlMMSI);
					strcat(curlParams, "&utime=");
					clearString(curlMMSI);
					printf("\nAfter Clear Returned: %s\n", curlMMSI);
					gettimeofday(&tv, NULL);
					sprintf(curlMMSI, "%d", tv.tv_sec);
					strcat(curlParams, curlMMSI);
					clearString(curlMMSI);
					alertType[0] = message[0];
					strcat(curlMMSI, alertType);
					strcat(curlParams, curlMMSI);
					clearString(curlMMSI);
					
					printf("\nCurl Params: %s\n", curlParams);
					
					curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curlParams);
					//printf("\n%s\n", globalParams);
					/* Perform the request, res will get the return code */ 
					res = curl_easy_perform(curl);
					/* Check for errors */ 
					if(res != CURLE_OK)
						fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
    				/* always cleanup */ 
					curl_easy_cleanup(curl);
  				}
  				curl_global_cleanup();
			}
		}
		//else status = 1;                        //received an unknown message, change to panic status
        /* FREE THE PACKET AFTER BEING USED */
		if ((ret = xbee_pktFree(pkt)) != XBEE_ENONE) {
				printf("%s %i\n", xbee_errorToStr(ret), ret);
		} 
	  }
	  if (status == 1) {			//panic mode
		//sound pizzo buzzer
		//blink led
		status = 0;
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
			printf("%i ", message[i] - '0');
		}
		/* block until a message was received or 2 minutes have passed since message was sent*/
		while(((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE) && !(difftime(time(&rawtime), messageSentTimeStamp) > replyTimeout))
		{
		}
		printf("\n");
		if (difftime(time(&rawtime), messageSentTimeStamp) > replyTimeout) {
			status = 1;	//arduino never replied
		}
		else
		{		//message was received, process it

			for (i = 0; i < leng; i++) {
				temp_char = pkt->data[18 + i];
				message[i] = temp_char;	//update message with received data
			}
			printf("Rx Data:     ");
			for (i = 0; i < leng; i++) {
				printf("%i ", message[i]);
			}
			if (message[0] == 65) status = 0;	//received acknowledgment, change to normal status
			else status = 1; 			//received an unknown message, change to panic status
			/* FREE THE PACKET AFTER BEING USED */
			if ((ret = xbee_pktFree(pkt)) != XBEE_ENONE) {
				printf("%s %i\n", xbee_errorToStr(ret), ret);
			}
		}
	  }//end startup status

	if (p == NULL) break;

	}

	if ((ret = xbee_conEnd(con)) != XBEE_ENONE) {
		xbee_log(xbee, -1, "xbee_conEnd() returned: %d", ret);
		return ret;
	}

	xbee_shutdown(xbee);

	return 0;
}
