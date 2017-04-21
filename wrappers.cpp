#include "KPI.h"

void KP::transaction(char type, Triple c) {
  byte i = 0;
  while (i < 3) {

    switch (i) {

      case 0:
        composeMessage(type, c);
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
  Triple foo;
  transaction('j', foo);

}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::rdfInsert(Triple cont) {
  transaction('i', cont);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::rdfRemove(Triple cont) {
  transaction('r', cont);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::rdfQuery(Triple cont) {
  transaction('q', cont);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::rdfSubscribe(Triple cont) {
  transaction('s', cont);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::leave() {
  Triple foo;
  transaction('l', foo);
}

