#include "KPI.h"

void KP::transaction(char type, Triple *t) {
  byte i = 0, tries = 0;
  bool response = false;
  char c;

  while (i < 3) {

    switch (i) {

      case 0:
      
        sendMessage(type, t);
        delay(500);
        break;

      case 1:
      
        receiveReply(type);

        break;

      case 2:
        //store();
        break;
    }

    i++;
  }

  if (_comm.connected()) _comm.stop();

}

void KP::join() {
  transaction('j', NULL);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::insert(Triple *t) {
  transaction('i', t);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::leave() {
  transaction('l', NULL);
}

