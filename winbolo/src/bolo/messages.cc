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
  static BYTE lastMessage = globalMessage;

  switch (msgType) {
    /* 4 Main message Types */
    case newsWireMessage:
      if (showNewswire) {
        if (lastMessage != newsWireMessage) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = newsWireMessage;
      }
      break;
    case assistantMessage:
      if (showAssistant) {
        if (lastMessage != assistantMessage) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = assistantMessage;
      }
      break;
    case AIMessage:
      if (showAI) {
        if (lastMessage != AIMessage) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = AIMessage;
      }
      break;
    case networkMessage:
      if (showNetwork) {
        if (lastMessage != networkMessage) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = networkMessage;
      }
      break;
    /* Player Messages */
    case player0Message:
      newMessageFrom = BASE_0; /* Using base macro because I am lazy :) */
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player0Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player0Message;
      break;
    case player1Message:
      newMessageFrom = BASE_1;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player1Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player1Message;
      break;
    case player2Message:
      newMessageFrom = BASE_2;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player2Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player2Message;
      break;
    case player3Message:
      newMessageFrom = BASE_3;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player3Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player3Message;
      break;
    case player4Message:
      newMessageFrom = BASE_4;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player4Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player4Message;
      break;
    case player5Message:
      newMessageFrom = BASE_5;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player5Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player5Message;
      break;
    case player6Message:
      newMessageFrom = BASE_6;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player6Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player6Message;
      break;
    case player7Message:
      newMessageFrom = BASE_7;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player7Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player7Message;
      break;
    case player8Message:
      newMessageFrom = BASE_8;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player8Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player8Message;
      break;
    case player9Message:
      newMessageFrom = BASE_9;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player9Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player9Message;
      break;
    case player10Message:
      newMessageFrom = BASE_10;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player10Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player10Message;
      break;
    case player11Message:
      newMessageFrom = BASE_11;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player11Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player11Message;
      break;
    case player12Message:
      newMessageFrom = BASE_12;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player12Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player12Message;
      break;
    case player13Message:
      newMessageFrom = BASE_13;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player13Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player13Message;
      break;
    case player14Message:
      newMessageFrom = BASE_14;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player14Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player14Message;
      break;
    case player15Message:
      newMessageFrom = BASE_15;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (lastMessage != player15Message) {
        messageAddItem(top, bottom);
      } else {
        messageAddItem((char *)MESSAGE_EMPTY, bottom);
      }
      lastMessage = player15Message;
      break;
    case networkStatus:
      if (showNetStat) {
        newMessageFrom = networkStatus;
        if (lastMessage != networkStatus) {
          messageAddItem(top, bottom);
        } else {
          messageAddItem((char *)MESSAGE_EMPTY, bottom);
        }
        lastMessage = networkStatus;
      }
      break;
    case globalMessage:
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
      frontEndMessages(topLine, bottomLine);
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
