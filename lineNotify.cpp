#include "lineNotify.h"


String lineToken="uvfNqffk0xhyoJRxCIJrd4QqWboSK9sOGHQGXKdFjSB";

extern bool _WAN_;
bool enableLine = false;
/*========================
 * Line Notifiy
 ========================*/
void enableLineBot(bool bOnOff)
{
    enableLine  = bOnOff;
}

void sendMsgByLine(const String &msg)
{
  if(_WAN_ && enableLine)
    LINE.notify(msg);
  else
  {
    Serial.println(msg);
  }


    // Example of sending numbers
//  LINE.notify(2342);          // integer
//  LINE.notify(212.43434,5);   // The real number shows 5 digits.
// // You can choose Line Sticker from https://devdocs.line.me/files/sticker_list.pdf
//  LINE.notifySticker(3,240);        // send Line Sticker with PackageID 3 , StickerID 240
//  LINE.notifySticker("Hello",1,2);  // send Line Sticker with PackageID 1 , StickerID 2 with text
//
//  // For example, send pictures as well url
//  LINE.notifyPicture("https://preview.ibb.co/j6G51n/capture25610417181915334.png");
//  LINE.notifyPicture("jatuthat","https://www.fotoaparat.cz/storage/pm/09/10/23/670915_a5351.jpg");

}

void LineInit(String myIP){
  // set Line Token
  LINE.setToken(lineToken);
  
  sendMsgByLine("the bot started."+ myIP);
}
