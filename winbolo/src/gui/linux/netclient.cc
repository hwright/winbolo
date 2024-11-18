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
*Name:          netClient
*Filename:      netClient.c
*Author:        John Morrison
*Creation Date: 20/2/99
*LAST MODIFIED: 28/5/00
*Purpose:
*  netClient TCP/UDP packet driver.
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#ifdef _WIN32 
#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#else
#include "SDL.h"
#define timeGetTime() SDL_GetTicks()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "messagebox.h"
#define MessageBox(A, B, C, D) MessageBox(B, C)
#define closesocket(X) close(X)
typedef struct hostent  HOSTENT;
typedef int SOCKET;
typedef int DWORD;
#define SD_BOTH 2
#define BUFFER_PREFIX_SIZE 2
#include "gtk/gtk.h"

typedef GtkWidget HWND;
#endif
#include "../../bolo/global.h"
#include "../../bolo/network.h"
#include "../../bolo/netpacks.h"
#include "../../bolo/udppackets.h"
#include "../../bolo/crc.h"
#include "../../bolo/util.h"
#include "../linresource.h"
#include "../lang.h"
#include "../netclient.h"
#include "../../lzw/lzw.h"



SOCKET myUdpSock = INVALID_SOCKET; /* Our Udp socket */
unsigned short myPort;             /* Our netClient Port number */
struct sockaddr_in addrServer;     /* Server address */
unsigned short serverPort;         /* Server Port */
struct sockaddr_in addrLast;       /* Where the last UDP packet came from */
unsigned short lastPort;           /* Port the last UDP packet came from */
struct sockaddr_in addrUs;         /* Our own machine */
struct sockaddr_in addrTracker;    /* Tracker machine */

/* Thread stuff */

#define BUFFER_PREFIX_SIZE 2


/*********************************************************
*NAME:          netClientCreate
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 29/4/00
*PURPOSE:
* Creates the netClient. Returns whether the operation
* was sucessful or not
*
*ARGUMENTS:
*  port - The port to create on (0== dont care)
*********************************************************/
bool netClientCreate(unsigned short port) {
  bool returnValue;        /* Value to return */
  int ret;                 /* Function returns */
  struct sockaddr_in addr; /* Socket structure */
  struct sockaddr_in name; /* Used to get port */
  int nameLen;             /* Used to set size of name */
  
  nameLen = sizeof(name);
  returnValue = TRUE;
  myPort = port;

  addrServer.sin_family = AF_INET;
  addrLast.sin_family = AF_INET;
  addrUs.sin_family = AF_INET;
  
  /* Create socket */
  if (returnValue == TRUE) {
    myUdpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (myUdpSock == INVALID_SOCKET) {
      returnValue = FALSE;
      MessageBox(NULL, langGetText(STR_NETCLIENTERR_CREATEUDPFAIL), DIALOG_BOX_TITLE, MB_ICONEXCLAMATION);
    } 
  }

  if (returnValue == TRUE) {
    returnValue = netClientSetUdpAsync(TRUE);
  }

  /* Bind to port */
  if (returnValue == TRUE) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(myUdpSock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret != 0) {
      returnValue = FALSE;
      MessageBox(NULL, langGetText(STR_NETCLIENTERR_BINDUDPFAIL), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
    } else {
      /* Get what port we were assigned */
      if (getsockname(myUdpSock, (struct sockaddr*) &name, (socklen_t *)&nameLen) == 0) {
        myPort = ntohs(name.sin_port);
      }
    }
  }

  return returnValue;
}
  
/*********************************************************
*NAME:          netClientDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 20/2/99
*LAST MODIFIED: 30/8/99
*PURPOSE:
* Destroys the netClient.
*
*ARGUMENTS:
*
*********************************************************/
void netClientDestroy(void) {
  if (myUdpSock != INVALID_SOCKET) {
    shutdown(myUdpSock, SD_BOTH);
    closesocket(myUdpSock);
    myUdpSock = INVALID_SOCKET;
  }
}

/*********************************************************
*NAME:          netClientSendUdpLast
*AUTHOR:        John Morrison
*CREATION DATE: 28/8/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sends a udp packet to the last address.
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void netClientSendUdpLast(BYTE *buff, int len) {
  sendto(myUdpSock, (char *) buff, len, 0, (struct sockaddr *)&addrLast, sizeof(addrLast));
}

/*********************************************************
*NAME:          netClientSendUdpServer
*AUTHOR:        John Morrison
*CREATION DATE: 28/8/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sends a udp packet to the server address.
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void netClientSendUdpServer(BYTE *buff, int len) {
  sendto(myUdpSock, (char *) buff, len, 0, (struct sockaddr *)&addrServer, sizeof(addrServer));
}


/*********************************************************
*NAME:          netClientGetServerAddress
*AUTHOR:        John Morrison
*CREATION DATE: 23/2/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Copys the servers address and port
*
*ARGUMENTS:
*  dest - Destination address
*  port - Port of that corresponds to the address
*********************************************************/
void netClientGetServerAddress(struct in_addr *dest, unsigned short *port) {
  memcpy(dest, &(addrServer.sin_addr.s_addr), sizeof(*dest));
  *port = ntohs(addrServer.sin_port);
}

/*********************************************************
*NAME:          netClientGetServerAddressString
*AUTHOR:        John Morrison
*CREATION DATE: 1/9/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Copys the servers address and port as a string.
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetServerAddressString(char *dest) {
  char strPort[PLAYER_NAME_LAST]; /* Port as a string */
  char *ip;

  ip =inet_ntoa(addrServer.sin_addr);
  strcpy(dest, ip);
  sprintf(strPort, ":%d", ntohs(addrServer.sin_port));
  strcat(dest, strPort);
}


/*********************************************************
*NAME:          netClientSetServerAddress
*AUTHOR:        John Morrison
*CREATION DATE: 24/2/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Sets the servers address and port.
*
*ARGUMENTS:
*  src  - src address
*  port - Port of that corresponds to the address
*********************************************************/
void netClientSetServerAddress(struct in_addr *src, unsigned short port) {
  memcpy(&(addrServer.sin_addr.s_addr), src, sizeof(*src)); 
  addrServer.sin_port = htons(port);
}

/*********************************************************
*NAME:          netClientSetServerPort
*AUTHOR:        John Morrison
*CREATION DATE: 12/1/00
*LAST MODIFIED: 12/1/00
*PURPOSE:
* Sets the servers port.
*
*ARGUMENTS:
*  port - Port of that corresponds to the address
*********************************************************/
void netClientSetServerPort(unsigned short port) {
  addrServer.sin_port = htons(port);
}

/*********************************************************
*NAME:          netClientSetUs
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sets up our IP address.
*
*ARGUMENTS:
*
*********************************************************/
void netClientSetUs(void) {
  char str[FILENAME_MAX]; /* Temp string to hold host name  & IP as string*/
  HOSTENT  *hd;           /* Host Data */
  int ipa;               /* Bytes for each IP address components */
  int ipb;
  int ipc;
  int ipd;

  ipa=ipb=ipc=ipd=0;

  gethostname(str, sizeof(str));
  hd = gethostbyname(str);
  if (hd != nullptr) {
    ipa = hd->h_addr_list[IP_ARRAY0][IP_ARRAY0];
    ipb = hd->h_addr_list[IP_ARRAY0][IP_ARRAY1];
    ipc = hd->h_addr_list[IP_ARRAY0][IP_ARRAY2];
    ipd = hd->h_addr_list[IP_ARRAY0][IP_ARRAY3];
    sprintf(str, "%d.%d.%d.%d",ipa,ipb,ipc,ipd);
  } else {
    strcpy(str, "127.0.0.1\0");
  }
  addrUs.sin_addr.s_addr = inet_addr(str);
  addrUs.sin_port = myPort;
}

/*********************************************************
*NAME:          netClientGetAddress
*AUTHOR:        John Morrison
*CREATION DATE: 11/4/01
*LAST MODIFIED: 11/4/01
*PURPOSE:
* Reverse looks up an ip address
*
*ARGUMENTS:
*  ip   - IP Address to look up
*  dest - Buffer to hold returnm
*********************************************************/
void netClientGetAddress(char  *ip, char *dest) {
  HOSTENT  *hd;           /* Host Data */
  struct sockaddr_in  addr;
  
  addr.sin_addr.s_addr = inet_addr(ip);
  hd = gethostbyaddr((const void*)&addr.sin_addr, sizeof(struct in_addr), AF_INET);
   if (hd == nullptr) {
    strcpy(dest, ip);
  } else {
    strcpy(dest, hd->h_name);
  }
}

/*********************************************************
*NAME:          netClientGetUs
*AUTHOR:        John Morrison
*CREATION DATE: 21/2/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Gets our IP address and port.
*
*ARGUMENTS:
*  dest - Destination address
*  port - Destination port
*********************************************************/
void netClientGetUs(struct in_addr *dest, unsigned short *port) {
  memcpy(dest, &(addrUs.sin_addr), sizeof(*dest));
  *port = myPort;
}

/*********************************************************
*NAME:          netClientGetUsStr
*AUTHOR:        John Morrison
*CREATION DATE: 3/3/99
*LAST MODIFIED: 28/8/993/3/99
*PURPOSE:
* Gets our IP address and port as a string
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetUsStr(char *dest) {
  char port[32]; /* Temp pointer to string */
  char *ip;      /* Holds the ip address */
  ip = inet_ntoa(addrUs.sin_addr);
  strcpy(dest, ip);
  sprintf(port, ":%d", myPort);
  strcat(dest, port);
}

/* FIXME */
bool netClientUdpPingServer(BYTE *buff, int *len, bool wantCrc, bool addNonReliable) {
  bool returnValue;           /* Value to return */
  struct sockaddr_in from;    /* Socket returned from */
  int fromlen = sizeof(from); /* Size of socket */
  DWORD tick;                 /* Current tick used to determine time out */
  unsigned int timeOut;       /* Time out */
  BYTE crcA, crcB; /* CRC Bytes */

  returnValue = TRUE;
  if (addNonReliable == TRUE) {
    buff[(*len)] = UDP_NON_RELIABLE_PACKET;
    (*len)++;
  }
  if (wantCrc == TRUE) {
    CRCCalcBytes(buff, *len, &crcA, &crcB);
    buff[*len] = crcA;
    buff[(*len)+1] = crcB;
    (*len) += 2;
  }

  timeOut = 0;
  tick = timeGetTime();
  sendto(myUdpSock, (char *) buff, *len, 0, (struct sockaddr *)&addrServer, sizeof(addrServer));
  *len = 0;
  while (*len <= 0 && timeOut <= TIME_OUT) {
    *len = recvfrom(myUdpSock, (char *) buff, MAX_UDPPACKET_SIZE, 0, (struct sockaddr *)&from, (socklen_t*)&fromlen);
/*      if (*len == SOCKET_ERROR) {
      if (WSAGetLastError() != WSAEWOULDBLOCK ) {
Do we want to bail out? I don't think so
        MessageBox(NULL, "b", "bad", MB_OK); (/
      } 
   } */
    timeOut = timeGetTime() - tick;
  }

  if (*len <= 0) {
    returnValue = FALSE;
  }
  return returnValue;
}

/*********************************************************
*NAME:          netClientUdpPing
*AUTHOR:        John Morrison
*CREATION DATE: 24/02/99
*LAST MODIFIED: 19/08/02
*PURPOSE:
* Sends a packet and waits till a repsonse packet arrives
* Returns FALSE if it times out.
*
*ARGUMENTS:
*  buff - Packet to send (also returned)
*  len  - Length of the packet (also returned)
*  dest - Destination address
*  port - Destination port
*  wantCrc - TRUE if we should add a CRC to this packet
*  addNoReliable - If true adds the non-reliable packet
*                  marker to the packet
*********************************************************/
bool netClientUdpPing(BYTE *buff, int *len, const char *dest, unsigned short port, bool wantCrc, bool addNonReliable) {
  bool returnValue;           /* Value to return */
  struct sockaddr_in addr;    /* Socket to send to */
  struct sockaddr_in from;    /* Socket returned from */
  struct hostent *phe;        /* Used for DNS lookups */
  int fromlen = sizeof(from); /* Size of socket */
  DWORD tick;                 /* Current tick used to determine time out */
  unsigned int timeOut;       /* Time out */
  BYTE crcA, crcB; /* CRC Bytes */

  returnValue = TRUE;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(dest);
  if (addr.sin_addr.s_addr == INADDR_NONE) {
    /* Do hostname lookup */
    phe= gethostbyname(dest);
    if (phe == nullptr) {
      returnValue = FALSE;
    } else {
      addr.sin_addr.s_addr = *((u_long*)phe->h_addr_list[0]);
    }
  }
  if (returnValue == TRUE) {
    if (addNonReliable == TRUE) {
      buff[(*len)] = UDP_NON_RELIABLE_PACKET;
      (*len)++;
    }
    if (wantCrc == TRUE) {
      CRCCalcBytes(buff, (*len), &crcA, &crcB);
      buff[(*len)] = crcA;
      buff[(*len)+1] = crcB;
      (*len) += 2;
    }

    timeOut = 0;
    tick = timeGetTime();
    sendto(myUdpSock, (char *) buff, *len, 0, (struct sockaddr *)&addr, sizeof(addr));
    *len = 0;
    while (*len <= 0 && timeOut <= TIME_OUT) {
      *len = recvfrom(myUdpSock, (char *) buff, MAX_UDPPACKET_SIZE, 0, (struct sockaddr *)&from, (socklen_t*)&fromlen);
/*      if (*len == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK ) {
 Do we want to bail out? I don't think so
          MessageBox(NULL, "b", "bad", MB_OK); (/
        } 
     } */
      memcpy(&addrLast, &(from.sin_addr), sizeof(from.sin_addr));
      lastPort = from.sin_port;
      timeOut = timeGetTime() - tick;
    }

    if (*len <= 0) {
      returnValue = FALSE;
    }
  }
  return returnValue;
}

/*********************************************************
*NAME:          netClientUdpSendNoWait
*AUTHOR:        John Morrison
*CREATION DATE: 26/2/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Sends a packet to the string address given
*
*ARGUMENTS:
*  buff - Packet to send
*  len  - Length of the packet
*  dest - Destination address
*  port - Destination port
*********************************************************/
void netClientSendUdpNoWait(BYTE *buff, int len, char *dest, unsigned short port) {
  struct sockaddr_in addr;    /* Socket to send to */

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(dest);
  sendto(myUdpSock, (char *) buff, len, 0, (struct sockaddr *)&addr, sizeof(addr));
}

/*********************************************************
*NAME:          netClientGetLastStr
*AUTHOR:        John Morrison
*CREATION DATE: 27/3/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Copies the location of the last packet into a string
*
*ARGUMENTS:
*  dest - Destination string
*********************************************************/
void netClientGetLastStr(char *dest) {
  char *temp; /* Temp string pointer */

  temp = inet_ntoa(addrLast.sin_addr);
  strcpy(dest, temp);
}

/*********************************************************
*NAME:          netClientGetLast
*AUTHOR:        John Morrison
*CREATION DATE: 23/12/99
*LAST MODIFIED: 23/12/99
*PURPOSE:
* Copies the location of the last packet.
*
*ARGUMENTS:
*  dest - Destination.
*  port - Destination port.
*********************************************************/
void netClientGetLast(struct in_addr  *dest, unsigned short *port) {
  memcpy(dest, &addrLast, sizeof(addrLast));
  *port = lastPort;
}

/*********************************************************
*NAME:          netClientSetUseEvents
*AUTHOR:        John Morrison
*CREATION DATE: 5/6/99
*LAST MODIFIED: 28/8/99
*PURPOSE:
* Sets our netClient socket to use windows notificaion on 
* incoming packets. Returns sucess.
*
*ARGUMENTS:
*
*********************************************************/
bool netClientSetUseEvents(void) {
  bool returnValue; /* Value to return */
  int ret;          /* Function return */

  returnValue = TRUE;
  ret = 0;
/*  ret = WSAAsyncSelect( mySock, windowWnd(), WSA_READ, FD_READ); */
  if (ret < 0) {
    returnValue = FALSE;
    MessageBox(NULL, langGetText(STR_NETCLIENTERR_CHAINFAIL), DIALOG_BOX_TITLE, MB_ICONINFORMATION);
  }
  return returnValue;
}

/*********************************************************
*NAME:          netClientUdpCheck
*AUTHOR:        John Morrison
*CREATION DATE: 28/5/00
*LAST MODIFIED: 28/5/00
*PURPOSE:
* Thread function responsible for checking for UDP Data.
*
*ARGUMENTS:
*
*********************************************************/
void netClientUdpCheck(void) {
  char info[MAX_UDPPACKET_SIZE];
  int packetLen;
  struct sockaddr_in from;
  int fromlen = sizeof(from);

  packetLen = recvfrom(myUdpSock, info, MAX_UDPPACKET_SIZE, 0, (struct sockaddr *)&from, (socklen_t *)&fromlen);
  while (packetLen > 0) {
    /* We have data - Yah! */
    memcpy(&addrLast, &from, (size_t) sizeof(from));
    lastPort = from.sin_port;
    netUdpPacketArrive((BYTE *) info, packetLen, lastPort);
    packetLen = recvfrom(myUdpSock, info, sizeof(info), 0, (struct sockaddr *)&from, (socklen_t *)&fromlen);
  }
}

/*********************************************************
*NAME:          netClientSetUdpAsync
*AUTHOR:        John Morrison
*CREATION DATE: 30/8/99
*LAST MODIFIED: 30/8/99
*PURPOSE:
* Sets the netClients UDP socket to be blocking/nonblocking
* Returns success.
*
*ARGUMENTS:
*  on - TRUE for non-block, FALSE for blocking
*********************************************************/
bool netClientSetUdpAsync(bool on) {
  int ret;          /* Function return value */
  bool returnValue; /* Value to return */
  
  returnValue = TRUE;
  if (on == TRUE) {
    ret = fcntl(myUdpSock, F_SETFL, O_NONBLOCK | fcntl(myUdpSock, F_GETFL));
  } else {
    ret = fcntl(myUdpSock, F_SETFL, ~O_NONBLOCK | fcntl(myUdpSock, F_GETFL));
  }

  if (ret == SOCKET_ERROR) {
    returnValue = FALSE;
  }
  return returnValue;
}

/*********************************************************
*NAME:          netClientSetTracker
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 21/11/99
*PURPOSE:
* Sets the trackers details up. Returns success.
*
*ARGUMENTS:
*  address - Address of the tracker
*  port    - Port of the tracker
*********************************************************/
bool netClientSetTracker(const char *address, unsigned short port) {
  bool returnValue;    /* Value to return      */
  struct hostent *phe; /* Used for DNS lookups */
  
  returnValue = TRUE;
  addrTracker.sin_family = AF_INET;
  addrTracker.sin_port = htons(port);
  addrTracker.sin_addr.s_addr = inet_addr(address);
  if (addrTracker.sin_addr.s_addr == INADDR_NONE) {
    /* Not an IP Address. Do a hostname lookup */
    phe= gethostbyname(address);
    if (phe == nullptr) {
      returnValue = FALSE;
    } else {
      addrTracker.sin_addr.s_addr = *((u_long*)phe->h_addr_list[0]);
    }
  }

  return returnValue;
}

/*********************************************************
*NAME:          netClientSendUdpTracker
*AUTHOR:        John Morrison
*CREATION DATE: 13/11/99
*LAST MODIFIED: 13/11/99
*PURPOSE:
* Sends a udp packet to the tracker
*
*ARGUMENTS:
*  buff  - Buffer to send 
*  len   - length of the buffer
*********************************************************/
void netClientSendUdpTracker(BYTE *buff, int len) {
  sendto(myUdpSock, (char *) buff, len, 0, (struct sockaddr *)&addrTracker, sizeof(addrTracker));
}

//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------


int gameFinderGetParamFromLine(char *src, char *dest, const char *pattern) {
  int returnValue; /* Value to return */
  char *ptr;
  int count;

  dest[0] = '\0';
  count = 0;
  returnValue = -1;
  if (strncmp(src, pattern, strlen(pattern)) == 0) {
    returnValue = strlen(pattern) + 1;
    ptr = src + returnValue;
    while (*ptr != '\n') {
      count++;
      ptr++;
    }
    strncat(dest, src+returnValue, count);
    strcat(dest, "\0");
    
  }

  return returnValue;
}


int gameFinderReplace(char *buff, int len) {
  int returnValue = -1;
  char *ptr = buff;
  while (strncmp(ptr, "\r\n", 2) != 0  && returnValue < len) {
    returnValue++;
    ptr++;
  }
  if (returnValue >= len) {
    returnValue = -1;
  } else {
    *ptr = '\0';
  }
  return returnValue+1;
}

char * gameFinderProcessMotd(char *buff, char *motd, int numLines) {
  int count;
  char *ptr;
  char param[1024];

  ptr = buff;
  motd[0] = '\0';
  count = 0;
  while (count < numLines) {
    gameFinderGetParamFromLine(ptr, param, "MOTD");
    strcat(motd, param);
    strcat(motd, "\n");
    count++;
    ptr += gameFinderReplace(ptr, 20000);
    ptr++;
    ptr++;
  }
  return ptr;
}

unsigned short gameFrontGameLocation(char *argItem, char *trackerAddr) {
  char *tmp;
  tmp = strtok(argItem, ":");
  strcpy(trackerAddr, tmp);
  tmp = strtok(nullptr, ":");
  return atoi(tmp);
}


bool gameFinderYesNoToBool(char *str) {
  bool returnValue;
  returnValue = FALSE;
  if (str[0] == 'Y' || str[0] == 'y') {
    returnValue = TRUE;
  }
  return returnValue;
}

void gameFinderProcessV1(currentGames *cg, char *buff, int len, char *motd) {
  char param[1024];
  char *ptr;
  int numMotdLines = 0;
  int count = 0;
  int numGames = 0;
  char gameNum[100];
  char gameId[100];
  char address[255];
  unsigned short port;
  char mapName[255];
  char version[255];
  gameType game;
  BYTE players;
  BYTE bases;
  BYTE pills;
  bool password;
  bool hiddenMines;
  aiType ai;

  ptr = buff;
  if (gameFinderGetParamFromLine(buff, param, "MOTDL") != -1) {
    /* Process Message of the day */
    numMotdLines = atoi(param);
    ptr += gameFinderReplace(ptr, 20000);
    ptr++;
    ptr++;
    ptr = gameFinderProcessMotd(ptr, motd, numMotdLines);
  }
  if (gameFinderGetParamFromLine(ptr, param, "NGAMES") != -1) {
     numGames = atoi(param);
  }
  if (numGames > 0) {
    ptr += gameFinderReplace(ptr, 20000);
    ptr++;
    ptr++;
    while (count < numGames) {
      sprintf(gameNum, "%.03d", count);
      strcpy(gameId, "GAME");
      strcat(gameId, gameNum);
      gameFinderGetParamFromLine(ptr, param, gameId);
      port = gameFrontGameLocation(param, address);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, version, "VERSION");
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, mapName, "MAP");
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, "TYPE");
      if (param[0] == 'o' || param[0] == 'O') {
        game = gameOpen;
      } else if (param[0] == 't' || param[0] == 'T') {
        game = gameTournament;
      } else {
        game = gameStrictTournament;
      }
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, "PLAYERS");
      players = atoi(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, "BASES");
      bases = atoi(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, "PILLBOXES");
      pills = atoi(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, "HIDMINES");
      hiddenMines = gameFinderYesNoToBool(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, "PASSWORD");
      password = gameFinderYesNoToBool(param);
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      gameFinderGetParamFromLine(ptr, param, "BRAINS");
      if (param[0] == 'n' || param[0] == 'N') {
        ai = aiNone;
      } else if (strlen(param) > 3) {
        ai = aiYesAdvantage;
      } else {
        ai = aiYes;
      }
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      ptr += gameFinderReplace(ptr, 20000);
      ptr++;
      ptr++;
      /* Add it */
      currentGamesAddItem(cg, address, port, mapName, version, players, bases, pills, hiddenMines, game, ai, password);
      count++;
    }
  }
}

void gameFinderProcess(HWND *hWnd, currentGames *cg, char *buff, int len, char *motd) {
  char *ptr;
  char *ptr2;
  char param[256];
  int amount;
  int version;
  
  ptr2 =ptr = buff;
  amount = gameFinderGetParamFromLine(buff, param, "TVERSION");
  ptr2 += amount;
  amount = gameFinderReplace(ptr, 20000);
  ptr += amount;
  ptr++;
  ptr++;
  version = atoi(ptr2);
  switch (version) {
    case 1:
    gameFinderProcessV1(cg, ptr, 20000, motd);
     break;
   default:
   MessageBox(NULL, langGetText(STR_NETCLIENTERR_TRACKERVERSIONFAIL), DIALOG_BOX_TITLE, MB_OK);
  }
}

void gameFinderProcessBroadcast(currentGames *cg, INFO_PACKET *info, struct in_addr *pack) {
  char mapName[MAP_STR_SIZE];
  char version[MAP_STR_SIZE];
  bool password;
  bool mines;
  char address[FILENAME_MAX];

  utilPtoCString(info->mapname, mapName);
  password = TRUE;
  if (info->has_password == 0) {
    password = FALSE;
  }
  mines = FALSE;
  if (info->allow_mines & 0x80) {
    mines = TRUE;
  }
  if (info->gameid.serveraddress.s_addr == 0) {
    strcpy(address, inet_ntoa(*pack));
  } else {
    strcpy(address, inet_ntoa(info->gameid.serveraddress));
  }
  sprintf(version, "%d.%d%d", info->h.versionMajor, info->h.versionMinor, info->h.versionRevision);
  currentGamesAddItem(cg, address, info->gameid.serverport, mapName, version, (BYTE) info->num_players, (BYTE) info->free_bases, (BYTE) info->free_pills, mines, (gameType)info->gametype, (aiType) info->allow_AI, password);
}

//-------------------------------------------------------------
////-------------------------------------------------------------
////-------------------------------------------------------------
////-------------------------------------------------------------
////-------------------------------------------------------------

/*********************************************************
*NAME:          netClientFindTrackedGames
*AUTHOR:        John Morrison
*CREATION DATE: 19/1/00
*LAST MODIFIED: 23/1/00
*PURPOSE:
* Looks up the tracker address and ports and downloads a
* list of current games. Returns success
*
*ARGUMENTS:
*  hWnd           - Window handle of the calling dialog
*  cg             - Pointer to the currentGames structure
*  trackerAddress - Address of the tracker to use
*  port           - Port of the tracker
*  motd           - Buffer to hold the message of the day
*********************************************************/
bool netClientFindTrackedGames(HWND *hWnd, currentGames *cg, char *trackerAddress, unsigned short port, char *motd) {
  bool returnValue;        /* Value to return */
  int ret;                 /* Function returns */
  struct sockaddr_in con;  /* Address to connect to */
  SOCKET sock;             /* Socket to use    */
  struct hostent *phe;     /* Used for DNS lookups */
  BYTE buff[4096];         /* Results buffer */
  BYTE *b;
  BYTE *ptr;               /* Buffer pointer */
  int len = 0;
  int new_bytes_read;
  char strBuff[FILENAME_MAX];
  bool done;
  DWORD tick, timeOut;
  char txt[256];

  sock = INVALID_SOCKET;
  returnValue = TRUE;
  b = new BYTE[128*1024];
  ptr = b;


  strcpy((char *) buff, "Status: ");
  strcat((char *) buff, langGetText(STR_NETCLIENT_TRACKERCONNECT));
  sprintf(strBuff, "%s:%d", trackerAddress, port);
  strcat((char *) buff, strBuff);
  gtk_label_set_text(GTK_LABEL(hWnd), (char *) buff);
  GDK_THREADS_LEAVE();
  while(g_main_iteration(FALSE));
  GDK_THREADS_ENTER();
 
  /* Create TCP Socket*/
  if (returnValue == TRUE) {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
      returnValue = FALSE;
      sprintf(txt, "Status: %s", langGetText(STR_NETCLIENTERR_CREATETCPFAIL));
      gtk_label_set_text(GTK_LABEL(hWnd), txt);
    }
  }

  if (returnValue == TRUE) {
    con.sin_family = AF_INET;
    con.sin_port = htons(port);
    con.sin_addr.s_addr = inet_addr(trackerAddress);
    if (con.sin_addr.s_addr == INADDR_NONE) {
      /* Not an IP Address. Do a hostname lookup */
      phe = gethostbyname(trackerAddress);
      if (phe == nullptr) {
        returnValue = FALSE;
      sprintf(txt, "Status: %s", langGetText(STR_NETCLIENTERR_TRACKERDNSFAIL));
        gtk_label_set_text(GTK_LABEL(hWnd), txt);
      } else {
        con.sin_addr.s_addr = *((u_long*)phe->h_addr_list[0]);
      }
    }
  }

  /* Connect */
  if (returnValue == TRUE) {
    ret = connect(sock, (struct sockaddr *) &con, sizeof(con));
    if (ret == SOCKET_ERROR) {
      sprintf(txt, "Status: %s", langGetText(STR_NETCLIENTERR_TRACKERCONNECTFAIL));
      gtk_label_set_text(GTK_LABEL(hWnd), txt);
     returnValue = FALSE;
    }
  }

  if (returnValue == TRUE) {
    ret = fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL));
    if (ret == SOCKET_ERROR) {
      gtk_label_set_text(GTK_LABEL(hWnd), "Status: Error setting socket to non blocking mode.");
      returnValue = FALSE;
    }
  }
  if (returnValue == TRUE) {
    sprintf(txt, "Status: %s", langGetText(STR_NETCLIENT_TRACKERGETRESPONSE));
    gtk_label_set_text(GTK_LABEL(hWnd), txt);
    len = 0;
    ptr = b;
    new_bytes_read = recv(sock, ptr+len, 1024*128-len, 0);
    tick = timeGetTime();
    timeOut = 0;
    done = FALSE;
    while (timeOut <= 10000 && done == FALSE) {
 
      if (new_bytes_read > 0) {
        len += new_bytes_read;
      } else if (new_bytes_read == SOCKET_ERROR) {
        if (errno != EAGAIN) {
          done = TRUE;
        }
      } else if (new_bytes_read == 0) {
        done = TRUE;
      }
      timeOut = timeGetTime() - tick;
        GDK_THREADS_LEAVE();
      while(g_main_iteration(FALSE));
      GDK_THREADS_ENTER(); 
      new_bytes_read = recv(sock, ptr+len, 1024*128-len, 0);
    }
    if (len == 0) {
      sprintf(txt, "Status: %s", langGetText(STR_NETCLIENTERR_TRACKERNODATA));
      gtk_label_set_text(GTK_LABEL(hWnd), txt);
      returnValue = FALSE;
    } else {
      sprintf(txt, "Status: %s", langGetText(STR_NETCLIENT_TRACKERPROCESSRESPONSE));
      gtk_label_set_text(GTK_LABEL(hWnd), txt);
      gameFinderProcess(hWnd, cg, (char *)b, len, motd);
    }
 }

  delete[] b;
  return returnValue;
}

/*********************************************************
*NAME:          netClientFindBroadcastGames
*AUTHOR:        John Morrison
*CREATION DATE: 4/6/00
*LAST MODIFIED: 4/6/00
*PURPOSE:
* Gets a list of current games on you by using your 
* broadcast address.
*
*ARGUMENTS:
*  hWnd           - Window handle of the calling dialog
*  cg             - Pointer to the currentGames structure
*********************************************************/
bool netClientFindBroadcastGames(HWND *hWnd, currentGames *cg) {
  bool returnValue;        /* Value to return */
  int ret;                 /* Function returns */
  struct sockaddr_in con;  /* Address to connect to */
  struct sockaddr_in from; /* Address the packet came from .*/
  struct sockaddr_in addr; /* Address to bind to  */
  SOCKET sock;             /* Socket to use    */
  BYTE *ptr;               /* Buffer pointer */
  int len = 0;
  BYTE buff[MAX_UDPPACKET_SIZE] = INFOREQUESTHEADER; /* Data Buffer */
  int timeOut;
  DWORD tick;
  int fromlen;
  struct sockaddr_in last;
  int szlast;
  char txt[256];

  szlast = sizeof(last);
  sock = INVALID_SOCKET;
  returnValue = TRUE;
  ptr = buff;
  /* Create UDP Socket*/
  if (returnValue == TRUE) {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
      returnValue = FALSE;
      sprintf(txt, "Status: %s", langGetText(STR_NETCLIENTERR_CREATEUDPFAIL));
      gtk_label_set_text(GTK_LABEL(hWnd), txt);
    }
  }
  /* Bind to port */
  if (returnValue == TRUE) {
    addr.sin_family = AF_INET;
    addr.sin_port = INADDR_ANY;
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret != 0) {
      returnValue = FALSE;
      MessageBox(NULL, langGetText(STR_NETCLIENTERR_BINDUDPFAIL), DIALOG_BOX_TITLE, MB_OK);
    }
  }

  /* Set to non blocking */
  if (returnValue == TRUE) {
     ret = fcntl(sock, F_SETFL, O_NONBLOCK | fcntl(sock, F_GETFL));
     if (ret == SOCKET_ERROR) {
      returnValue = FALSE;
      sprintf(txt, "Status: %s",  langGetText(STR_NETCLIENTERR_TRACKERNOBLOCK));
      gtk_label_set_text(GTK_LABEL(hWnd), txt);
    }
  }
  /* Broadcast socket option */
  if (returnValue == TRUE) {
    int c;
    c =1;
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &c , sizeof(c));
    if (ret != 0) {
      returnValue = FALSE;
      gtk_label_set_text(GTK_LABEL(hWnd), "Status: Error setting socket options");
    }
  }

  /* Do Broadcast */
  if (returnValue == TRUE) {
    con.sin_family = AF_INET;
    con.sin_port = htons(27500); /* Fixme: Run time settable? */
    con.sin_addr.s_addr = INADDR_BROADCAST;
    ret = sendto(sock, buff, BOLOPACKET_REQUEST_SIZE, 0, (struct sockaddr *)&con, sizeof(con));
    if (ret == SOCKET_ERROR) {
      sprintf(txt, "Status: %s", langGetText(STR_NETCLIENTERR_BROADCAST));
      gtk_label_set_text(GTK_LABEL(hWnd), txt); 
      returnValue = FALSE;
    }
  }

  usleep(50);
  if (returnValue == TRUE) {
    /* Get responses */
    sprintf(txt, "Status: %s", langGetText(STR_NETCLIENT_GETRESPONSES));
    gtk_label_set_text(GTK_LABEL(hWnd), txt);
    len = recvfrom(sock, ptr+len, sizeof(buff)-len, 0, (struct sockaddr *) &last, (socklen_t*)&szlast);
    timeOut = 0;
    tick = timeGetTime();
    while (timeOut <= 10000) { //TIME_OUT
      if (len == sizeof(INFO_PACKET)) {
        if (strncmp((char *) buff, BOLO_SIGNITURE, BOLO_SIGNITURE_SIZE) == 0 && buff[BOLO_VERSION_MAJORPOS] == BOLO_VERSION_MAJOR && buff[BOLO_VERSION_MINORPOS] == BOLO_VERSION_MINOR && buff[BOLO_VERSION_REVISIONPOS] == BOLO_VERSION_REVISION && buff[BOLOPACKET_REQUEST_TYPEPOS] == BOLOPACKET_INFORESPONSE) {
        /* Process response */
          gameFinderProcessBroadcast(cg, (INFO_PACKET *) buff, &(last.sin_addr));
        }
      }
      GDK_THREADS_LEAVE();
      while(g_main_iteration(FALSE));
      GDK_THREADS_ENTER();
      usleep(50);
      timeOut = timeGetTime() - tick;
      len = recvfrom(sock, buff, MAX_UDPPACKET_SIZE, 0, (struct sockaddr *)&from, (socklen_t*)&fromlen);
    }
  }
  if (sock != INVALID_SOCKET) {
    shutdown(sock, SD_BOTH);
    closesocket(sock);
  }

  return returnValue;
}


