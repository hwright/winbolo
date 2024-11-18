/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


/*********************************************************
*Name:          http
*Filename:      http.c
*Author:        John Morrison
*Creation Date: 16/9/01
*Last Modified: 23/9/01
*Purpose:
*  Responsable for sending/receiving http messages
*********************************************************/

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
  #include <winsock2.h>
  #include <windows.h>
  #include "../gui/gamefront.h"
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #define INVALID_SOCKET -1
  #include <netdb.h>
  #define closesocket(X) close(X)
  #include "../gui/linux/preferences.h"
#endif
#include "../bolo/global.h"
#include "winbolonet.h"
#include "../server/servertransport.h"
#include "http.h"

bool httpStarted = false; /* Is the http subsystem operational */
struct sockaddr_in httpAddrServer; /* Winbolo.net Server address */
char wbnHostString[FILENAME_MAX]; /* Preference for name */
char hostString[FILENAME_MAX]; /* Host String Name Host: wbnHostString */
int hostStringLen; /* Length of hostString */
char altIpAddress[FILENAME_MAX];

static unsigned long getaddrbyany(char *sp_name)  {               
  struct hostent *sp_he;
  int i; 

  if(isdigit(*sp_name))
    return inet_addr(sp_name);
  for(i=0;i<100;i++) {                       
    sp_he = gethostbyname(sp_name);
    if(!(sp_he)) {		
#ifdef _WIN32
      Sleep(1);
#else
      sleep(1);
#endif
     if(i>=3) {
       return(0);
     }
   } else break;   
  }

  return sp_he ? *(long*)*sp_he->h_addr_list : 0;
}   

/*********************************************************
*NAME:          httpCreate
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 16/9/01
*PURPOSE:
* Initialises the http module. Initialises winsock etc.
* Returns success.
*
*ARGUMENTS:
* 
*********************************************************/
bool httpCreate() {
  bool returnValue = true;          /* Value to return */
  char prefs[FILENAME_MAX];         /* Preference file */
  struct hostent *phe;              /* Used for DNS lookups */

#ifdef _WIN32
  int ret;  
  WSADATA wsaData;         /* Winsock create data */

  ret = WSAStartup(MAKEWORD(2,0), &wsaData);
  if (ret != 0) {
    returnValue = false;
  }
  strcpy(prefs, PREFERENCE_FILE);
#else
  preferencesGetPreferenceFile(prefs);
#endif

  wbnHostString[0] = EMPTY_CHAR;
  /* Lookup the server */
  GetPrivateProfileString("WINBOLO.NET", "Host", "wbn.winbolo.net", wbnHostString, FILENAME_MAX, prefs);
  WritePrivateProfileString("WINBOLO.NET", "Host", wbnHostString, prefs); /* Write it back if we are a server (we wont have a client config file yet) */
  httpAddrServer.sin_family = AF_INET;
  httpAddrServer.sin_port = htons(80);  // HTTP_SERVER_PORT
  httpAddrServer.sin_addr.s_addr = inet_addr(wbnHostString);
  if (httpAddrServer.sin_addr.s_addr == INADDR_NONE) {
    /* Do hostname lookup */
    phe= gethostbyname(wbnHostString); // wbnHostString
    if (phe == 0) {
      returnValue = false;
      fprintf(stderr, "Unable to locate WinBolo.net host. Winbolo.net disabled.\n");
      #ifdef _WIN32
        WSACleanup();
      #endif
    } else {
      httpAddrServer.sin_addr.s_addr = *((u_long*)phe->h_addr_list[0]);
    }
  }

  sprintf(hostString, "Host: %s\n\n", wbnHostString);
  hostStringLen = (int) strlen(hostString);
  
  httpStarted = returnValue;
  return returnValue;
}

/*********************************************************
*NAME:          httpDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 16/9/01
*PURPOSE:
* Destroys the http module. Cleans up any open libraries
*
*ARGUMENTS:
* 
*********************************************************/
void httpDestroy() {
  if (httpStarted == true) {
    /* Perform any cleanup required */
#ifdef _WIN32
    WSACleanup();
#endif
  }

  httpStarted = false;
}


/*********************************************************
*NAME:          httpDecodeData
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 16/9/01
*PURPOSE:
* Decodes the byte buffer and returns its decoded length
* (-1 for error)
*
*ARGUMENTS:
* buff - The source byte buffer
* len  - The source byte buffer length
*********************************************************/
int httpDecodeData(BYTE *buff, int len) {
  int returnValue; /* Value to return  */
  int count;       /* Looping variable */

  count = 0;
  returnValue = 0;
  while (count < len) {
    if (buff[count] == '%') {
      count++;
      buff[returnValue] = HTTP_HEXTOINT(buff[count]) * 16 + HTTP_HEXTOINT(buff[count+1]);
      returnValue++;
      count+=2;
    } else {
      buff[returnValue] = buff[count];
      count++;
      returnValue++;
    }
  }
  return returnValue;
}


/*********************************************************
*NAME:          httpEncodeData
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 16/9/01
*PURPOSE:
* Encodes a byte buffer and returns its encoded length
* (-1 for error)
*
*ARGUMENTS:
* src    - The source byte buffer
* len    - The source byte buffer length
* dest   - The destination byte buffer
* maxLen - Maximum length of the byte buffer
*********************************************************/
int httpEncodeData(BYTE *src, int len, BYTE *dest, int maxLen) {
  int count;
  int encodeCount;
  char temp[3];

  count = 0;
  encodeCount = 0;
  while (count < len && encodeCount < maxLen) {
    if ((src[count]>='0' && src[count]<='9') || (src[count] >= 'a' && src[count]<= 'z') || (src[count]>= 'A' && src[count]<= 'Z')) {
      /* Valid */
      dest[encodeCount] = src[count];
      encodeCount++;
    } else { 
      /* Encode it */
      sprintf(temp, "%x", src[count]);
      dest[encodeCount] = '%';
      encodeCount++;
      if (src[count] <= 0xF) {
        dest[encodeCount] = '0';
        encodeCount++;
        dest[encodeCount] = temp[0];
        encodeCount++;
      } else {
        dest[encodeCount] = temp[0];
        encodeCount++;
        dest[encodeCount] = temp[1];
        encodeCount++;
      }
    }
    count++;
  }

  if (encodeCount > maxLen) {
    //returnValue = -1;
  }
  return encodeCount;
}

/*********************************************************
*NAME:          httpSendData
*AUTHOR:        John Morrison
*CREATION DATE: 19/9/01
*LAST MODIFIED: 19/9/01
*PURPOSE:
* Sends the data to the server, including http headers
* returns the length of the resulting message or -1 if
* an error occurs
*
*ARGUMENTS:
* sock    - Socket used to send.
* message - The message to send to the WinBolo.net server
* len     - Length of the data
*********************************************************/
int httpSendData(SOCKET sock, BYTE *message, int len) {
  int returnValue = -1; /* Value to return */
  BYTE *buff;
  int encodeLen;

  buff = new BYTE[len * 3];
  
  if (buff != NULL) {
    encodeLen = httpEncodeData(message, len, buff, (len * 3));
    if (encodeLen != -1) {
      returnValue = (int) send(sock, HTTP_SEND_HEADER, (int) strlen(HTTP_SEND_HEADER), 0);
      if (returnValue > 0) {
        returnValue = send(sock, buff, encodeLen, 0);
//DEBUG
//        printf("sending: %s",HTTP_SEND_HEADER);
//        fwrite(buff, encodeLen, 1, stdout);
//	fwrite(" HTTP/1.0\n", 10, 1, stdout);
//	fwrite(hostString, hostStringLen, 1, stdout);

	      /* Send the host */
    	  send(sock, " HTTP/1.0\n", 10, 0);
	      send(sock, hostString, hostStringLen, 0);
      }
    }
    delete[] buff;
  }
  return returnValue;
  
}

/*********************************************************
*NAME:          httpRecvData
*AUTHOR:        John Morrison
*CREATION DATE: 23/9/01
*LAST MODIFIED: 23/9/01
*PURPOSE:
* Revieves the data from the server, Checks headers
* returns the length of the resulting data or -1 if
* an error occurs
*
*ARGUMENTS:
* sock    - Data Socket
* buff    - Data buffer to write to
* maxSize - The maximum size of the response
*********************************************************/
int httpRecvData(SOCKET sock, BYTE *buff, int maxSize) {
  int returnValue = -1; /* Value to return */
  int size = 0;         /* Size of data recieved */
  int ret;              /* Function return Value */
  bool found;
  
  BYTE *ptr;

  ret = recv(sock, buff, maxSize, 0);
  while (ret > 0 && size < maxSize) {
    size += ret;
    ret = recv(sock, buff+size, maxSize-size, 0);
  }

  if (ret >= 0) {
    /* Strip out the header */
    ptr = buff;
    found = false;
    while (((ptr-buff) < size-3) && found == false) {
      if (*ptr == '\r' && *(ptr+1) == '\n' && *(ptr+2) == '\r' && *(ptr+3) == '\n') {
        found = true;
      } else {
        ptr++;
      }
    }
    if (found == true) {
      ptr += 4; /* Skip over \r\n\r\n */
      returnValue = (int) (size - (ptr - buff));
      memcpy(buff, ptr, returnValue);
    } else {
      returnValue = -1;
    }
  } else {
    returnValue = -1;
  }

  return returnValue; /* Value to return */
}

/*********************************************************
*NAME:          httpSendMessage
*AUTHOR:        John Morrison
*CREATION DATE: 16/9/01
*LAST MODIFIED: 23/9/01
*PURPOSE:
* Sends a message to the WinBolo.net server via http and
* returns the length of the resulting message or -1 if
* an error occurs
*
*ARGUMENTS:
* message - The message to send to the WinBolo.net server
* len     - Length of the data
* respose - A buffer to hold the response
* maxSize - The maximum size of the response
*********************************************************/
int httpSendMessage(BYTE *message, int len, BYTE *response, int maxSize) {
  int returnValue =-1; /* Value to return */
  int ret;             /* Function return value */ 
  SOCKET sock;         /* Socket used     */
  struct sockaddr_in ourAddress; /*This structure is to allow us to bind an ip to the socket */
  static unsigned long addressToBind=0; /*This is the address we're going to bind, this is the address winbolo.net will show */
  

  if (httpStarted == true) {
	ourAddress.sin_family = AF_INET;
	ourAddress.sin_port = 0; /*setting this to zero should have it just grab a ephemeral port*/
	ourAddress.sin_addr.s_addr = INADDR_ANY; /* grabs the ip from the default interface */
	/* 
	  This is were we test for an -addr and then put it into the above variable, havn't coded this yet.
	*/
	if(strlen(altIpAddress)>1){
		if(addressToBind==0){
			addressToBind = getaddrbyany(altIpAddress);
		}
		if(addressToBind>0){
			ourAddress.sin_addr.s_addr = addressToBind;
		}
	}
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    returnValue = true;
    if (sock <= 0) {
      returnValue = -1;
    } else {
	  /* bind the socket here, so that it will use the IP address we've set in the above code when it connects to winbolo.net*/
      ret = bind(sock,(struct sockaddr*)&ourAddress,sizeof(ourAddress));
	  if(ret < 0) {
	    returnValue = -1;
	  } else {
		ret = 0;
	    ret = connect(sock, (struct sockaddr *) &httpAddrServer, sizeof(httpAddrServer));
        if (ret == SOCKET_ERROR) {
          returnValue = false;
		}
	  }
    }

    if (returnValue == true) {
      returnValue = httpSendData(sock, message, len);
    }
    if (returnValue >0) {
      /* Get the data */
      returnValue = httpRecvData(sock, response, maxSize);
      shutdown(sock, SD_BOTH);
      closesocket(sock);
    }
  }
  return returnValue;
}



long httpGetFileLength(char *fileName) {
  long returnValue = -1; /* Value to return */
  FILE *fp;

  fp = fopen(fileName, "rb");
  if (fp) {
    returnValue = 0;
    while (!feof(fp)) {
      fgetc(fp);
      returnValue++;
    }
    fclose(fp);
  }

  return returnValue;
}

bool httpSendLogFile2(char *fileName, BYTE *key, bool wantFeedback, long fileLength) {
  bool returnValue;
  FILE *fp;
  SOCKET sock;
  char sKey[33];
  char header[1024];
  char boundry[512];
  char contentLength[512];
  char tmp[64];
  BYTE block[512];
  int count = 0;
  long fileCount = 0;
  int sendBytes;
  int ret;
  int randNum;
  long length;

  fp = fopen(fileName, "rb");
  if (fp == NULL) {
    return false;
  }

  srand((unsigned int) time(NULL));
  randNum = rand() % 10;
  memset(sKey, 0, 33);
  memcpy(sKey, key, WINBOLONET_KEY_LEN);
  sprintf(boundry, "---------------------------%d", randNum);
  while (count < 12) {
    randNum = rand() % 10;
    sprintf(tmp, "%d", randNum);
    strcat(boundry, tmp);
    count++;
  }

  sprintf(contentLength, "--%s\r\nContent-Disposition: form-data; name=\"logfile\"; filename=\"log.dat\"\r\nContent-Type: application/octet-stream\r\n\r\n", boundry); 
  length = (long) ((long) strlen(contentLength)) + fileLength + (long) strlen(boundry) + 7;

  sprintf(header, "%s%s HTTP/1.0\r\nContent-Type: multipart/form-data; boundary=%s\r\nContent-Length: %ld\r\n%s", HTTP_POST_HEADER, sKey, boundry, length, hostString);  
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  returnValue = true;
  if (sock <= 0) {
    returnValue = false;
  } else {
    ret = connect(sock, (struct sockaddr *) &httpAddrServer, sizeof(httpAddrServer));
    if (ret == SOCKET_ERROR) {
      returnValue = false;
    }


    ret = send(sock, header, (int) strlen(header), 0);
    if (ret != -1) {
      ret = send(sock, contentLength, (int) strlen(contentLength), 0);
    }
    while (fileCount < fileLength && ret != -1) {
      if (fileCount + 512 < fileLength) {
        fread(block, 512, 1, fp);
        fileCount += 512;
        sendBytes = 512;
      } else {
        fread(block, fileLength - fileCount, 1, fp);
        sendBytes = fileLength - fileCount;
        fileCount = fileLength;
      }
      ret = send(sock, block, sendBytes, 0);
    }

    /* Send end of content marker */
    if (ret != -1) {
      sprintf(header, "\r\n--%s--\r\n", boundry);
      ret = send(sock, header, (int) strlen(header), 0);
    }

    /* Get data back */
    if (ret != -1) {
      returnValue = httpRecvData(sock, (BYTE *) header, 1024);
    }
    shutdown(sock, SD_BOTH);
    closesocket(sock);
  }


  fclose(fp);

  return returnValue;

}


bool httpSendLogFile(char *fileName, BYTE *key, bool wantFeedback) {
  long fileLength;

  if (httpStarted == true && fileName != NULL && key != NULL) {
    fileLength = httpGetFileLength(fileName);
    if (fileLength != -1) {
      return httpSendLogFile2(fileName, key, wantFeedback, fileLength);
    }
  }
  return false;
}

/*********************************************************
*NAME:          httpSetAltIpAddress
*AUTHOR:        Minhiriath	
*CREATION DATE: 14/3/2009
*LAST MODIFIED: 14/3/2009
*PURPOSE:
* This function sets the alternate ip address to use on multi-ip servers.
*
*ARGUMENTS:
* iptoset - the alternate ip address to use.
*********************************************************/
void httpSetAltIpAddress(const char *iptoset){
	strcpy(altIpAddress,iptoset);
}

