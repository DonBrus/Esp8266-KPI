#include "KPI.h"

void KP::transaction(char type, Triple *t) {
  byte i = 0;
  while (i < 3) {

    switch (i) {

      case 0:
        sendMessage(type, t);
        delay(500);
        break;

      case 1:
        //receive();

        Serial.println("WAITING FOR RESPONSE");
        char c;
        while (_comm.available()) {
          c = _comm.read();
          Serial.print(c);
        }


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
  transaction('l',NULL);
}

