#include "KPI.h"

void KP::transaction(char type, Triple *t) {
  byte i = 0, tries = 0;
  bool response = false;
  char c;
<<<<<<< HEAD
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
=======
>>>>>>> parent of e99219e... "create" still standalone

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
<<<<<<< HEAD
  transaction('j', NULL, "", "");
=======
  transaction('j', NULL);
>>>>>>> parent of e99219e... "create" still standalone
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::insert(Triple *t) {
<<<<<<< HEAD
  transaction('i', t, "", "");
=======
  transaction('i', t);
>>>>>>> parent of e99219e... "create" still standalone
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::leave() {
<<<<<<< HEAD
  transaction('l', NULL, "", "");
}

//------------------------------------------------------------------------------------------------------------------------------------


void KP::query(char query[MAX_QUERY_SIZE]) {
  transaction('q', NULL, query, "");
}

//------------------------------------------------------------------------------------------------------------------------------------
/*
  void KP::subscribe() {
  transaction('l', NULL);
  }

  //------------------------------------------------------------------------------------------------------------------------------------

  void KP::unsubscribe(Subscription sub) {
  transaction('l', NULL);
  }
*/
=======
  transaction('l', NULL);
}

>>>>>>> parent of e99219e... "create" still standalone
