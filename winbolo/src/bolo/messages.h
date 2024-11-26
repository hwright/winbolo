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
 *Filename:      messages.h
 *Author:        John Morrison
 *Creation Date: 3/1/99
 *Last Modified: 1/6/00
 *Purpose:
 *  Responsable for scrolling messages.
 *********************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <array>
#include <queue>
#include <string_view>

#include "global.h"

#define IsEmpty(list) ((list) == NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define MessageTail(list) ((list)->next);
#define MessageHeadTop(list) ((list)->topLine);
#define MessageHeadBottom(list) ((list)->bottomLine);

/* Message String Macros */
#define MESSAGE_QUOTES "\""

/* Width of the screen */
#define MESSAGE_WIDTH 68
/* End of string marker */
#define END_OF_STRING '\0'
/* Blank space marker */
#define MESSAGE_BLANK ' '

/* An Empty Message */
#define MESSAGE_EMPTY " \0"

/* Time between screen updates */
#define MESSAGE_SCROLL_TIME 4 /* Was 5 prior to 1.09 */

// Different kinds of messages
enum class messageType {
  newsWire,  // Differnt Message types
  assistant,
  AI,
  network,
  networkStatus,
  player0,  // Player Number messages
  player1,
  player2,
  player3,
  player4,
  player5,
  player6,
  player7,
  player8,
  player9,
  player10,
  player11,
  player12,
  player13,
  player14,
  player15,
  global  // Message must be printed
};

namespace bolo {

class Messages {
 public:
  Messages();

  // Display a new message of the given type.
  //
  // ARGUMENTS:
  //  msgType - The type of the message
  //  top     - The message to print in the top line
  //  bottom  - The message to print in the bottom line
  void addMessage(messageType msgType, std::string_view top,
                  std::string_view bottom);

  // Update the scrolling message
  void Update();

  // Return the current messages on screen.
  std::tuple<std::string, std::string> getMessage();

  // Show or hide various message types
  void set_newswire(bool show) { show_newswire_ = show; }
  void set_assistant(bool show) { show_assistant_ = show; }
  void set_ai(bool show) { show_ai_ = show; }
  void set_network(bool show) { show_network_ = show; }
  void set_netstat(bool show) { show_netstat_ = show; }

 private:
  // Add an item to the message data structure.
  //
  // ARGUMENTS:
  //  top    - The message to print in the top line
  //  bottom - The message to print in the bottom line
  void addItem(std::string_view top, std::string_view bottom);

  // The messages
  std::array<char, MESSAGE_WIDTH> top_line_;
  std::array<char, MESSAGE_WIDTH> bottom_line_;

  // Queue for waiting messages.
  std::queue<std::tuple<char, char>> waiting_messages_;

  messageType last_message_ = messageType::global;

  // What types to show
  bool show_newswire_ = true;
  bool show_assistant_ = true;
  bool show_ai_ = false;
  bool show_network_ = false;
  bool show_netstat_ = true;
};

}  // namespace bolo

typedef struct messageObj *message;
struct messageObj {
  message next;
  char topLine;
  char bottomLine;
};

/* Offset to a player message */
#define PLAYER_MESSAGE_OFFSET 5

/* Prototypes */

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
void messageCreate(void);

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
void messageDestroy(void);

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
void clientMessageAdd(messageType msgType, const char *top, const char *bottom);

/*********************************************************
 *NAME:          serverMessageAdd
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
void serverMessageAdd(messageType msgType, const char *top, const char *bottom);

/*********************************************************
 *NAME:          messageAdd
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/1/99
 *LAST MODIFIED:  3/1/99
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
void messageAdd(messageType msgType, const char *top, const char *bottom);

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
void messageAddItem(const char *top, const char *bottom);

/*********************************************************
 *NAME:          messageUpdate
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/1/99
 *LAST MODIFIED:  3/1/99
 *PURPOSE:
 *  Updates the scrolling message
 *
 *ARGUMENTS:
 *
 *********************************************************/
void messageUpdate(void);

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
void messageGetMessage(char *top, char *bottom);

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
void messageSetNewswire(bool isShown);

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
void messageSetAssistant(bool isShown);

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
void messageSetAI(bool isShown);

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
void messageSetNetwork(bool isShown);

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
void messageSetNetStatus(bool isShown);

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
bool messageIsNewMessage();

/*********************************************************
 *NAME:          messageGetNewMessage
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/11/99
 *LAST MODIFIED: 26/11/99
 *PURPOSE:
 *  Gets the new message. Returns where the message
 *  originated from. NOTE: PlayerBitmap presently unused
 *
 *ARGUMENTS:
 *  dest         - Destination for the message
 *  playerBitmap - Bitmap of players that recieved it
 *********************************************************/
BYTE messageGetNewMessage(char *dest, unsigned long **playerBitmap);

#endif /* MESSAGE_H */
