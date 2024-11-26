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
 *Name:          Messages
 *Filename:      messages.c
 *Author:        John Morrison
 *Creation Date: 03/01/99
 *Last Modified: 20/01/02
 *Purpose:
 *  Responsable for scrolling messages.
 *********************************************************/

#include "messages.h"

#include <string.h>

#include "backend.h"
#include "frontend.h"
#include "global.h"
#include "util.h"

/* Module Variables */

/* Messages */
static char topLine[MESSAGE_WIDTH];
static char bottomLine[MESSAGE_WIDTH];

static char newMessage[FILENAME_MAX]; /* A new message */
static BYTE newMessageFrom;           /* Where it came from */

/* What types to show */
static bool showNewswire = true;
static bool showAssistant = true;
static bool showAI = false;
static bool showNetwork = false;
static bool showNetStat = true;

/* Queue DS for waiting messages */
static message msg;

/*********************************************************
 *NAME:          messageCreate
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/1/99
 *LAST MODIFIED:  3/1/99
 *PURPOSE:
 *  Sets up the messages data structure
 *
 *ARGUMENTS:
 *
 *********************************************************/
void messageCreate(void) {
  BYTE count; /* Looping variable */

  msg = nullptr;
  showNewswire = true;
  showAssistant = true;
  showAI = false;
  showNetwork = false;
  showNetStat = true;
  for (count = 0; count < MESSAGE_WIDTH; count++) {
    topLine[count] = MESSAGE_BLANK;
    bottomLine[count] = MESSAGE_BLANK;
  }
  topLine[MESSAGE_WIDTH - 1] = END_OF_STRING;
  bottomLine[MESSAGE_WIDTH - 1] = END_OF_STRING;
}

/*********************************************************
 *NAME:          messageDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/1/99
 *LAST MODIFIED:  3/1/99
 *PURPOSE:
 *  Destroys and frees memory for the message data
 *  structure
 *
 *ARGUMENTS:
 *
 *********************************************************/
void messageDestroy(void) {
  message q;

  while (!IsEmpty(msg)) {
    q = msg;
    msg = MessageTail(q);
    delete q;
  }
}

/*********************************************************
 *NAME:          clientMessageAdd
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/1/99
 *LAST MODIFIED:  4/7/00
 *PURPOSE:
 *  Functions call this to display a message. They must
 *  pass the message type so that it can be determined
 *  whether the header should be printed etc.
 *
 *ARGUMENTS:
 *  msgType - The type of the message
 *  top     - The message to print in the top line
 *  bottom  - The message to print in the bottom line
 *********************************************************/
void clientMessageAdd(messageType msgType, const char *top,
                      const char *bottom) {
  static messageType lastMessage = messageType::global;

  switch (msgType) {
    /* 4 Main message Types */
    case messageType::newsWire:
      if (showNewswire) {
        if (lastMessage != messageType::newsWire) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = messageType::newsWire;
      }
      break;
    case messageType::assistant:
      if (showAssistant) {
        if (lastMessage != messageType::assistant) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = messageType::assistant;
      }
      break;
    case messageType::AI:
      if (showAI) {
        if (lastMessage != messageType::AI) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = messageType::AI;
      }
      break;
    case messageType::network:
      if (showNetwork) {
        if (lastMessage != messageType::network) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = messageType::network;
      }
      break;
    /* Player Messages */
    case messageType::player0:
      newMessageFrom = BASE_0; /* Using base macro because I am lazy :) */
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player0) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player0;
      break;
    case messageType::player1:
      newMessageFrom = BASE_1;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player1) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player1;
      break;
    case messageType::player2:
      newMessageFrom = BASE_2;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player2) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player2;
      break;
    case messageType::player3:
      newMessageFrom = BASE_3;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player3) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player3;
      break;
    case messageType::player4:
      newMessageFrom = BASE_4;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player4) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player4;
      break;
    case messageType::player5:
      newMessageFrom = BASE_5;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player5) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player5;
      break;
    case messageType::player6:
      newMessageFrom = BASE_6;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player6) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player6;
      break;
    case messageType::player7:
      newMessageFrom = BASE_7;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player7) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player7;
      break;
    case messageType::player8:
      newMessageFrom = BASE_8;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player8) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player8;
      break;
    case messageType::player9:
      newMessageFrom = BASE_9;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player9) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player9;
      break;
    case messageType::player10:
      newMessageFrom = BASE_10;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player10) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player10;
      break;
    case messageType::player11:
      newMessageFrom = BASE_11;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player11) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player11;
      break;
    case messageType::player12:
      newMessageFrom = BASE_12;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player12) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player12;
      break;
    case messageType::player13:
      newMessageFrom = BASE_13;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player13) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player13;
      break;
    case messageType::player14:
      newMessageFrom = BASE_14;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player14) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player14;
      break;
    case messageType::player15:
      newMessageFrom = BASE_15;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != messageType::player15) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = messageType::player15;
      break;
    case messageType::networkStatus:
      if (showNetStat) {
        newMessageFrom = 20;  // messageType::networkStatus;
        if (lastMessage != messageType::networkStatus) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = messageType::networkStatus;
      }
      break;
    case messageType::global:
    default:
      messageAddItem(top, bottom);
  }
}

/*********************************************************
 *NAME:          messageAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/1/99
 *LAST MODIFIED:  3/1/99
 *PURPOSE:
 *  Adds an item to the message data structure.
 *
 *ARGUMENTS:
 *  top    - The message to print in the top line
 *  bottom - The message to print in the bottom line
 *********************************************************/
void messageAddItem(const char *top, const char *bottom) {
  message q;     /* temp Pointer */
  message prev;  /* temp pointer */
  message add;   /* Item to add */
  int lenTop;    /* Lengths of the item */
  int lenBottom; /* Length of the bottom string */
  int count;     /* Looping variable */
  int longest;   /* Longest item */
  bool newQ;     /* Denotes a new queue */

  /* Get the location to add it to */

  newQ = false;

  if (IsEmpty(msg)) {
    newQ = true;
    msg = new messageObj;
    msg->next = nullptr;
  }

  prev = q = msg;
  while (NonEmpty(q)) {
    prev = q;
    q = MessageTail(q);
  }
  q = prev;

  /* Get the longest of the two strings */
  lenTop = (int)strlen(top);
  lenBottom = (int)strlen(bottom);
  if (lenTop > lenBottom) {
    longest = lenTop;
  } else {
    longest = lenBottom;
  }

  /* Add the items to the data structure */
  count = 0;
  while (count <= (longest)) {
    add = new messageObj;
    if (count < lenTop) {
      add->topLine = top[count];
    } else {
      add->topLine = MESSAGE_BLANK;
    }
    if (count < lenBottom) {
      add->bottomLine = bottom[count];
    } else {
      add->bottomLine = MESSAGE_BLANK;
    }
    add->next = nullptr;
    q->next = add;
    q = MessageTail(q);
    count++;
  }

  if (newQ) {
    q = msg;
    msg = MessageTail(q);
    delete q;
  }
}

/*********************************************************
 *NAME:          messageUpdate
 *AUTHOR:        John Morrison
 *CREATION DATE: 03/01/99
 *LAST MODIFIED: 20/01/02
 *PURPOSE:
 *  Updates the scrolling message
 *
 *ARGUMENTS:
 *
 *********************************************************/
void messageUpdate(void) {
  message q;  /* temp Pointer */
  BYTE count; /* Looping variable */

  /* Only want to do something if the message needs to be scrolled */
  if (NonEmpty(msg)) {
    /* Get the next charectors */
    topLine[MESSAGE_WIDTH - 1] = MessageHeadTop(msg);
    bottomLine[MESSAGE_WIDTH - 1] = MessageHeadBottom(msg);
    /* Delete the item */
    q = msg;
    msg = MessageTail(q);
    delete q;

    /* Move the message */
    count = 0;
    while (count < (MESSAGE_WIDTH - 1)) {
      topLine[count] = topLine[count + 1];
      bottomLine[count] = bottomLine[count + 1];
      count++;
    }
    topLine[MESSAGE_WIDTH - 1] = END_OF_STRING;
    bottomLine[MESSAGE_WIDTH - 1] = END_OF_STRING;
    /* Update the screen */
    if (!threadsGetContext()) {
      screenGetFrontend()->messages(topLine, bottomLine);
    }
  }
}

/*********************************************************
 *NAME:          messageGetMessage
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/1/98
 *LAST MODIFIED: 1/1/98
 *PURPOSE:
 *  Copys the messages on screen into the given variables
 *
 *ARGUMENTS:
 *  top    - The message to print in the top line
 *  bottom - The message to print in the bottom line
 *********************************************************/
void messageGetMessage(char *top, char *bottom) {
  strcpy(top, topLine);
  strcpy(bottom, bottomLine);
}

/*********************************************************
 *NAME:          messageSetNewswire
 *AUTHOR:        John Morrison
 *CREATION DATE: 8/1/98
 *LAST MODIFIED: 8/1/98
 *PURPOSE:
 *  Sets the state of newswire messages
 *
 *ARGUMENTS:
 *  isShown - Is this type of message shown
 *********************************************************/
void messageSetNewswire(bool isShown) { showNewswire = isShown; }

/*********************************************************
 *NAME:          messageSetAssistant
 *AUTHOR:        John Morrison
 *CREATION DATE: 8/1/98
 *LAST MODIFIED: 8/1/98
 *PURPOSE:
 *  Sets the state of assistant messages
 *
 *ARGUMENTS:
 *  isShown - Is this type of message shown
 *********************************************************/
void messageSetAssistant(bool isShown) { showAssistant = isShown; }

/*********************************************************
 *NAME:          messageSetAI
 *AUTHOR:        John Morrison
 *CREATION DATE: 8/1/98
 *LAST MODIFIED: 8/1/98
 *PURPOSE:
 *  Sets the state of AI messages
 *
 *ARGUMENTS:
 *  isShown - Is this type of message shown
 *********************************************************/
void messageSetAI(bool isShown) { showAI = isShown; }

/*********************************************************
 *NAME:          messageSetNetwork
 *AUTHOR:        John Morrison
 *CREATION DATE: 8/1/98
 *LAST MODIFIED: 8/1/98
 *PURPOSE:
 *  Sets the state of network messages
 *
 *ARGUMENTS:
 *  isShown - Is this type of message shown
 *********************************************************/
void messageSetNetwork(bool isShown) { showNetwork = isShown; }

/*********************************************************
 *NAME:          messageSetNetStatus
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/6/00
 *LAST MODIFIED: 1/6/00
 *PURPOSE:
 *  Sets the state of network status messages
 *
 *ARGUMENTS:
 *  isShown - Is this type of message shown
 *********************************************************/
void messageSetNetStatus(bool isShown) { showNetStat = isShown; }

/*********************************************************
 *NAME:          messageIsNewMessage
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/11/99
 *LAST MODIFIED: 26/11/99
 *PURPOSE:
 *  Returns whether a new message has arrived or not.
 *
 *ARGUMENTS:
 *
 *********************************************************/
bool messageIsNewMessage() {
  bool returnValue; /* Value to return */

  returnValue = true;
  if (newMessage[0] == '\0') {
    returnValue = false;
  }
  return returnValue;
}

/*********************************************************
 *NAME:          messageGetNewMessage
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/11/99
 *LAST MODIFIED: 20/01/02
 *PURPOSE:
 *  Gets the new message. Returns where the message
 *  originated from. NOTE: PlayerBitmap presently unused
 *
 *ARGUMENTS:
 *  dest         - Destination for the message
 *  playerBitmap - Bitmap of players that recieved it
 *********************************************************/
BYTE messageGetNewMessage(char *dest, unsigned long **playerBitmap) {
  strcpy(dest, newMessage);
  playerBitmap = nullptr; /* Presently unused */
  newMessage[0] = '\0';
  return newMessageFrom;
}
