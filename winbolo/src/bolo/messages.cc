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

static bolo::Messages *messages;

static char newMessage[FILENAME_MAX]; /* A new message */
static BYTE newMessageFrom;           /* Where it came from */

namespace bolo {

Messages::Messages() {
  top_line_.fill(' ');
  bottom_line_.fill(' ');
  top_line_[MESSAGE_WIDTH - 1] = '\0';
  bottom_line_[MESSAGE_WIDTH - 1] = '\0';
}

void Messages::addMessage(messageType msgType, std::string_view top,
                          std::string_view bottom) {
  switch (msgType) {
    /* 4 Main message Types */
    case messageType::newsWire:
      if (show_newswire_) {
        if (last_message_ != messageType::newsWire) {
          addItem(top, bottom);
        } else {
          addItem("", bottom);
        }
        last_message_ = messageType::newsWire;
      }
      break;
    case messageType::assistant:
      if (show_assistant_) {
        if (last_message_ != messageType::assistant) {
          addItem(top, bottom);
        } else {
          addItem("", bottom);
        }
        last_message_ = messageType::assistant;
      }
      break;
    case messageType::AI:
      if (show_ai_) {
        if (last_message_ != messageType::AI) {
          addItem(top, bottom);
        } else {
          addItem("", bottom);
        }
        last_message_ = messageType::AI;
      }
      break;
    case messageType::network:
      if (show_network_) {
        if (last_message_ != messageType::network) {
          addItem(top, bottom);
        } else {
          addItem("", bottom);
        }
        last_message_ = messageType::network;
      }
      break;
    /* Player Messages */
    case messageType::player0:
      newMessageFrom = BASE_0; /* Using base macro because I am lazy :) */
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player0) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player0;
      break;
    case messageType::player1:
      newMessageFrom = BASE_1;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player1) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player1;
      break;
    case messageType::player2:
      newMessageFrom = BASE_2;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player2) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player2;
      break;
    case messageType::player3:
      newMessageFrom = BASE_3;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player3) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player3;
      break;
    case messageType::player4:
      newMessageFrom = BASE_4;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player4) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player4;
      break;
    case messageType::player5:
      newMessageFrom = BASE_5;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player5) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player5;
      break;
    case messageType::player6:
      newMessageFrom = BASE_6;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player6) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player6;
      break;
    case messageType::player7:
      newMessageFrom = BASE_7;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player7) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player7;
      break;
    case messageType::player8:
      newMessageFrom = BASE_8;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player8) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player8;
      break;
    case messageType::player9:
      newMessageFrom = BASE_9;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player9) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player9;
      break;
    case messageType::player10:
      newMessageFrom = BASE_10;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player10) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player10;
      break;
    case messageType::player11:
      newMessageFrom = BASE_11;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player11) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player11;
      break;
    case messageType::player12:
      newMessageFrom = BASE_12;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player12) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player12;
      break;
    case messageType::player13:
      newMessageFrom = BASE_13;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player13) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player13;
      break;
    case messageType::player14:
      newMessageFrom = BASE_14;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player14) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player14;
      break;
    case messageType::player15:
      newMessageFrom = BASE_15;
      strcpy(newMessage, bolo::utilCtoPString(bottom).c_str());
      if (last_message_ != messageType::player15) {
        addItem(top, bottom);
      } else {
        addItem("", bottom);
      }
      last_message_ = messageType::player15;
      break;
    case messageType::networkStatus:
      if (show_netstat_) {
        newMessageFrom = 20;  // messageType::networkStatus;
        if (last_message_ != messageType::networkStatus) {
          addItem(top, bottom);
        } else {
          addItem("", bottom);
        }
        last_message_ = messageType::networkStatus;
      }
      break;
    case messageType::global:
      addItem(top, bottom);
  }
}

void Messages::addItem(std::string_view top, std::string_view bottom) {
  // Get the longest size for left justification
  int longest = std::max(top.length(), bottom.length());

  // Add the items to the data structure
  for (int i = 0; i < longest; ++i) {
    waiting_messages_.push({i < top.length() ? top[i] : ' ',
                            i < bottom.length() ? bottom[i] : ' '});
  }

  // Add a space between messages.
  waiting_messages_.push({' ', ' '});
}

void Messages::Update() {
  // Only want to do something if the message needs to be scrolled
  if (!waiting_messages_.empty()) {
    // Get the next characters
    std::tie(top_line_[MESSAGE_WIDTH - 1], bottom_line_[MESSAGE_WIDTH - 1]) =
        waiting_messages_.front();

    // Delete the item
    waiting_messages_.pop();

    // Move the message
    for (int i = 0; i < (MESSAGE_WIDTH - 1); ++i) {
      top_line_[i] = top_line_[i + 1];
      bottom_line_[i] = bottom_line_[i + 1];
    }
    top_line_[MESSAGE_WIDTH - 1] = '\0';
    bottom_line_[MESSAGE_WIDTH - 1] = '\0';

    // Update the screen
    if (!threadsGetContext()) {
      screenGetFrontend()->messages(
          std::string_view(top_line_.data(), top_line_.size()),
          std::string_view(bottom_line_.data(), bottom_line_.size()));
    }
  }
}

std::tuple<std::string, std::string> Messages::getMessage() {
  return std::make_tuple(
      std::string(std::string_view(top_line_.data(), top_line_.size())),
      std::string(std::string_view(bottom_line_.data(), bottom_line_.size())));
}

}  // namespace bolo

void messageCreate(void) { messages = new bolo::Messages(); }

void messageDestroy(void) { delete messages; }

void clientMessageAdd(messageType msgType, const char *top,
                      const char *bottom) {
  messages->addMessage(msgType, top, bottom);
}

void messageUpdate(void) { messages->Update(); }

void messageGetMessage(char *top, char *bottom) {
  std::tuple<std::string, std::string> msg = messages->getMessage();
  strcpy(top, std::get<0>(msg).c_str());
  strcpy(bottom, std::get<1>(msg).c_str());
}

void messageSetNewswire(bool isShown) { messages->set_newswire(isShown); }
void messageSetAssistant(bool isShown) { messages->set_assistant(isShown); }
void messageSetAI(bool isShown) { messages->set_ai(isShown); }
void messageSetNetwork(bool isShown) { messages->set_network(isShown); }
void messageSetNetStatus(bool isShown) { messages->set_netstat(isShown); }

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
