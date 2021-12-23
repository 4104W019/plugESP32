#ifndef _LINENOTIFY_H_
#define _LINENOTIFY_H_BSVR_H_
#include "Arduino.h"

#include <TridentTD_LineNotify.h>

void LineInit(String myIP);
void sendMsgByLine(const String &msg);
void enableLineBot(bool bOnOff);

#endif
