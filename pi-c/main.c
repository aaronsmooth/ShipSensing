
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
#include <xbee.h> 
#include <curl/curl.h> 
#include <ifaddrs.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <net/if.h> 
#include <errno.h>
//#include <wiringPi.h>
//#include <wiringSerial.h>

int mmsi[18] = {265491000,248264000,311695000,235068882,248265000,636011023,566086000,266211000,257613000,355488000,257565000,249904000,636015819,311000102,248225000,236111887,357481000,248223000};
char message[8] = "00000000";
int leng = 8;
int HBTimeout = 1800;
int replyTimeout = 120;
int result, i, strLength;
char globalParams[64];
char *mmsiStr;
int currentMmsi = 265491000;
//char localhost[] = "192.168.0.101";

void clearString(char *string)
{
    strLength = strlen(string);
    for (i = 0; i < strLength; i++)
    {
        string[i] = '\0';
    }
}

void setMessage(char firstChar, time_t rawtime) 
{
	struct tm * timeinfo;
	time(&rawtime);
        timeinfo = localtime (&rawtime);

        message[0] = firstChar + '0';
        message[1] = '0' + convertToHex(timeinfo->tm_sec); //0 seconds
        message[2] = '0' + convertToHex(timeinfo->tm_min); //31 minutes
        message[3] = '0' + convertToHex(timeinfo->tm_hour); //20th hour or 8pm
        message[4] = '0' + convertToHex(timeinfo->tm_wday); //
        message[5] = '0' + convertToHex(timeinfo->tm_mday);
        message[6] = '0' + convertToHex(timeinfo->tm_mon + 1);//months since  Jan
        message[7] = '0' + (timeinfo->tm_year - 100);//years since 1900
}

int convertToHex(int value) 
{
	result = (value / 10) << 4;
	result = result | (value % 10);
	return result;
}

int convertToInt(char val) 
{
	
	result = (val >> 4) * 10;
	result += val & 0x0F;
	return result - 6;
}

void function_pt(void *ptr, size_t size, size_t nmemb, void *stream) {
	//mmsiStr = ptr;
	printf("\nHI");
	printf("\n%d\n", atoi(ptr));
}

unsigned int convertToTM(void) {
	printf("\nHI");
	struct tm rtime;
	rtime.tm_sec = convertToInt(message[1]);
	rtime.tm_min = convertToInt(message[2]);
	rtime.tm_hour = convertToInt(message[3]);
	rtime.tm_wday = convertToInt(message[4]);
	rtime.tm_mday = convertToInt(message[5]);
	rtime.tm_mon = convertToInt(message[6]);
	rtime.tm_year = convertToInt(message[7]);
	printf("\nYear  = %i\n", rtime.tm_year);
	return mktime(&rtime);
	//return rtime;
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
	CURL *curl;
  	CURLcode res;
	char apiData[50];
	int mmsiPtr;
	mmsiPtr = 0;
	struct ifaddrs *myaddrs, *ifa;
	void *in_addr;
	char buf[64];	//the buffer holding the ip address
	char curlStr[64], curlParams[64], tempParams[64], curlMMSI[64], alertType[64];
	clearString(curlParams);
    clearString(tempParams);
    clearString(curlStr);
    clearString(curlMMSI);
    clearString(alertType);
	
	srand(time(NULL));
	
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
	strcat (curlStr, ":8000/activity");
	
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

	double temp;
	char temp_char;
	char * temp_array;
	time(&lastHeartbeatTimeStamp);		//set initial timestamp 
	
	for (;;) {
	  void *p;
	  
	  printf("\nCurrent Status = %i ", status);
	  if (status == 0) {			//normal mode
		printf("\n");
		//time(&lastHeartbeatTimeStamp);
		/* block until a message was received or 30 minutes have passed since a message was received */
		while(((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE) && !(difftime(time(&rawtime), lastHeartbeatTimeStamp) > HBTimeout))
		{
//			printf("\nwaiting");
		}
		printf("\nReceived");
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
                		/* If the packet length was 27 then it's a valid message */
                		//if (pkt->dataLen == 27) {
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
                        //	if ((ret = xbee_pktFree(pkt)) != XBEE_ENONE) {
                          //      	printf("%s %i\n", xbee_errorToStr(ret), ret);
                        //	}
			}
		}
		else	//a message was received from arduino
		{
			printf("\nPkt length = %i", pkt->dataLen);
			printf("\nElse Statement");
		               //message was received, process it
                	/* If the packet length was 27 then it's a valid message */
                	//if (pkt->dataLen == 27) {
			char temp_char;
                        for (i = 0; i < leng; i++) {
				//printf(" %i ", pkt->data[i]);
				//printf(" Message at %i : %i ", i, message[i]);
                                temp_char = pkt->data[18 + i];
                                message[i] = temp_char; //update message with received data
                        }
                        printf("\nRx Data:     ");
                        for (i = 0; i < leng; i++) {
                                printf("%c ", message[i]);
                        }
			printf("\n");
                        if (message[0] == 'S' || message[0] == 'N')      //received Ship Arrival/Departure notification, create thread to send data to API
                        {
				/* In windows, this will init the winsock stuff */ 
  				curl_global_init(CURL_GLOBAL_ALL);
				
				printf("\nCurrent Time from Arduino %i\n", convertToTM());
  				curl = curl_easy_init();
  				if(curl) 
				{
    					/* First set the URL that is about to receive our POST. This URL can
       					just as well be a https:// URL if that is what should receive the
       					data. */
    					curl_easy_setopt(curl, CURLOPT_URL, curlStr);
    					/* Now specify the POST data */
					    clearString(curlMMSI);
                        strcat(curlParams, "mmsi=");
                        currentMmsi = message[0] == 'S' ? mmsi[rand() % 18] : currentMmsi;
                        sprintf(curlMMSI, "%d", currentMmsi);
                        strcat(curlParams, curlMMSI);
                        strcat(curlParams, "&utime=");
                        clearString(curlMMSI);
                        //printf("\nAfter Clear Returned: %s\n", curlMMSI);
                        //gettimeofday(&tv, NULL);
                        //sprintf(curlMMSI, "%d", tv.tv_sec);
                        //strcat(curlParams, curlMMSI);
                        //clearString(curlMMSI);
                        strcat(curlParams, "&st=");
                        alertType[0] = message[0];
                        strcat(curlMMSI, alertType);
                        strcat(curlParams, curlMMSI);
                        clearString(curlMMSI);
						
    					curl_easy_setopt(curl, CURLOPT_POSTFIELDS, curlParams);
						
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
		//printf("\n");
		//printf("\nStuck1");
		/* block until a message was received or 2 minutes have passed since message was sent*/
		while(((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE) && !(difftime(time(&rawtime), messageSentTimeStamp) > replyTimeout))
		{
			//printf("\nstuck2");
		}
			//printf("%s %i\n ",xbee_errorToStr(ret),ret);
		//printf("\nStuck 3");
		printf("\n");
		if (difftime(time(&rawtime), messageSentTimeStamp) > replyTimeout) {
			status = 1;	//arduino never replied
		}
		else
		{		//message was received, process it
		/* If the packet length was 27 then it's a valid message */
		//if (pkt->dataLen == 27) {
			//char temp_char;
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
