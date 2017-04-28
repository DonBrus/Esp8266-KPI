#include "KPI.h"

void KP::transaction(char type, Triple *t = NULL , char subid[MAX_SUBID_SIZE] = "" ) {

  WiFiClient *client;

  //!!!!!!!!!!!!!!!NOT UN/SUBSCRIBE : USE COMMON SOCKET

  if (type != 's' && type != 'u') { //se sono comunicazioni sulla socket comune
    _comm.connect(_ip, _port);
    if (!_comm.connected()) {
      Serial.println("CLIENT ERROR");
      return;
    }
    client = &_comm;
  }

  //!!!!!!!!!!!!!!!!SUBSCRIBE : OPEN SOCKET

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

  delay(YIELDING_TIME);

  //!!!!!!!!!!!!!!!!!ACTUAL TRANSACTION

  sendMessage(type, t , subid, client);
  delay(YIELDING_TIME);
  receiveReply(type);
  delay(YIELDING_TIME);

  if (_comm.connected()) _comm.stop();  //NOT FOR SUBSCRIPTION : SOCKET MUST BE LEFT OPENED

}

void KP::join() {
  transaction('j');
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::insert(Triple *t) {
  transaction('i', t);
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::leave() {
  transaction('l');
}

//------------------------------------------------------------------------------------------------------------------------------------


void KP::query(char query[MAX_QUERY_SIZE]) {
  strcpy(_query, query);
  transaction('q');
}

//------------------------------------------------------------------------------------------------------------------------------------

void KP::subscribe(char query[MAX_QUERY_SIZE]) {
  strcpy(_query, query);
  transaction('s');
}

//------------------------------------------------------------------------------------------------------------------------------------
/*
  void KP::unsubscribe(Subscription sub) {
  transaction('l', NULL);
  }
*/
