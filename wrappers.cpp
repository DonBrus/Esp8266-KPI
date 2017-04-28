#include "KPI.h"

void KP::transaction(char type, Triple *t , char query[MAX_QUERY_SIZE], char subid[MAX_SUBID_SIZE] ) {
  byte i = 0, tries = 0;
  bool response = false;
  char c;
  WiFiClient *client;

  if (type != 's' && type != 'u') { //se sono comunicazioni sulla socket comune
    _comm.connect(_ip, _port);
    if (!_comm.connected()) {
      Serial.println("CLIENT ERROR");
      return;
    }
    client = &_comm;
  }
  
  else if (type == 's') {

    if (!_s1.isActive) {  //se la sottoscrizione numero 1 è libera
      _s1.in.connect(_ip, _port);
      if (!_s1.in.connected()) {
        Serial.println("CLIENT ERROR");
        return;
      }
      client = &(_s1.in);
      //non metto qua l's1.active = true perchè devo aspettare fino al confirm alla ricezione della risposta
    }

    else if (!_s2.isActive) {  //se la sottoscrizione numero 2 è libera
      _s2.in.connect(_ip, _port);
      if (!_s2.in.connected()) {
        Serial.println("CLIENT ERROR");
        return;
      }
      client = &(_s2.in);
    }

    else { //non ci sono sottoscrizioni libere
      Serial.println("NO FREE SUBS");
      return;
    }

    //!!!!!!!Possibile approccio per multiple sottoscrizioni : array/lista di sub con un massimo possibile impostato , ciclo for itera per vedere se ce ne sono di libere

  }

  delay(50);

  while (i < 3) {

    switch (i) {

      case 0:

        sendMessage(type, t , query, subid, client);
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
  transaction('j', NULL,"","");
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::insert(Triple *t) {
  transaction('i',t,"","");
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::leave() {
  transaction('l', NULL,"","");
}

//------------------------------------------------------------------------------------------------------------------------------------
/*
 
void KP::query() {
  transaction('l', NULL);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::subscribe() {
  transaction('l', NULL);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::unsubscribe(Subscription sub) {
  transaction('l', NULL);
}
*/
