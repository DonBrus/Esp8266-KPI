#include "KPI.h"

#define SUB_DEBUG 0

void KP::transaction(char type, Triple *t = NULL , char subid[MAX_SUBID_SIZE] = "", char query[MAX_QUERY_SIZE] = "" ) {

  WiFiClient *client;

  //!!!!!!!!!!!!!!!NOT UN/SUBSCRIBE : USE COMMON SOCKET

  if (type != 's') { //se sono comunicazioni sulla socket comune
    _comm.connect(_ip, _port);
    if (!_comm.connected()) {
      Serial.println("CLIENT ERROR");
      return;
    }
    client = &_comm;
  }

  //!!!!!!!!!!!!!!!!SUBSCRIBE : OPEN SOCKET

  else  {

    if (!_s.isActive && SUB_DEBUG) {
      Serial.println("SUB PROVA");
      _s.in.connect(_ip, _port);
      if (!_s.in.connected()) {
        Serial.println("CLIENT ERROR");
        return;
      }
      client = &_s.in;
    }

    else if (!_is1Active) {  //se la sottoscrizione numero 1 è libera
      Serial.println("SUB N. 1");
      _s1.connect(_ip, _port);
      if (!_s1.connected()) {
        Serial.println("CLIENT ERROR");
        return;
      }
      client = &_s1;
      //non metto qua l's1.active = true perchè devo aspettare fino al confirm alla ricezione della risposta
    }

    else if (!_is2Active) {
      if (!_s2.connected()) {
        Serial.println("CLIENT ERROR");
        return;
      }
      client = &_s2;
    }

    else { //non ci sono sottoscrizioni libere
      Serial.println("NO FREE SUBS");
      return;
    }

    //!!!!!!!Possibile approccio per multiple sottoscrizioni : array/lista di sub con un massimo possibile impostato , ciclo for itera per vedere se ce ne sono di libere
    //con una lista ogni nodo possiede una subscription ID , quando è ora di chiuderne una bisognerebbe iterare su tutta lista in cerca dell'ID,se viene trovato si chiude il client
    // e si toglie il nodo -> abbastanza intensivo dal punto di vista della memoria del micro

  }

  //!!!!!!!!!!!!!!!!!ACTUAL TRANSACTION

  client->flush();
  sendMessage(type, t , subid, client, query);
  _status = receiveReply(type, client);
  delay(YIELDING_TIME);


  if (_comm.connected()) _comm.stop();  //NOT FOR SUBSCRIPTION : SOCKET MUST BE LEFT OPENED
  
  //if (_status != OK  && type == 's') client->stop(); //se le subscription non sono andate a buon fine , chiudi le socket
  else if (client == &_s1){
    _is1Active = true;  //se abbiamo attivato la sub1 ...
    Serial.println(_subID1);
  }
  else _is2Active = true; //o la sub 2

  _trID += 2; //aggiorna il transaction ID

}

bool KP::join() {
  transaction('j');
  if (_status == OK) return true;
  return false;
}

//------------------------------------------------------------------------------------------------------------------------------------

bool KP::insert(Triple *t) {
  transaction('i', t);
  if (_status == OK) return true;
  return false;
}

//------------------------------------------------------------------------------------------------------------------------------------

bool KP::leave() {
  transaction('l');
  if (_status == OK) return true;
  return false;
}

//------------------------------------------------------------------------------------------------------------------------------------


bool KP::query(char query[MAX_QUERY_SIZE]) {
  transaction('q', NULL, "", query);
  if (_status == OK) return true;
  return false;
}

//------------------------------------------------------------------------------------------------------------------------------------

bool KP::subscribe(char query[MAX_QUERY_SIZE]) {
  transaction('s', NULL, "", query);
  if (_status == OK) return true;
  return false;
}

//------------------------------------------------------------------------------------------------------------------------------------

bool KP::notification() {
  
  if(_is1Active && _s1.available()) _status = receiveReply('n', &_s1);
  else if(_is2Active && _s2.available())  _status = receiveReply('n', &_s2);
  else return false ;

  if (_status == OK) return true;
  return false;
}

//------------------------------------------------------------------------------------------------------------------------------------

bool KP::unsubscribe(byte id) {
  if(id==1 && _is1Active){
    transaction('u', NULL, _subID1);
    _s1.stop();
  }
  else if(id==2 && _is2Active) {
    transaction('u', NULL, _subID2);
    _s2.stop();
  }
  else return false ;
  
  if(_status==OK) return true;
  return false;
}

