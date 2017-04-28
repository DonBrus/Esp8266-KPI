#include "KPI.h"

KP::KP(char ID[], short TR ) {

  if (strcmp(ID, "") == 0) { //generate a random id if left blank by the user with chars between a-z
    for (byte i = 0; i < MAX_ID_SIZE; i++) {
      ID[i] = random(97, 122);
    }
  }

  strcpy(_nodeID, ID);
  _trID = TR;
  _status = OK;

}

void KP::begin(char SSID[MAX_SSID_LENGTH], char psw[MAX_PSW_LENGTH], short p, byte a, byte b, byte c, byte d) { //a,b,c,d : bytes of the IP address,to be put in order
  if (!WiFiConnect(SSID, psw)) _status = WIFI_CONN_ERR;
  _port = p;
  _ip[0] = a;
  _ip[1] = b;
  _ip[2] = c;
  _ip[3] = d;
}

byte KP::getState() {
  return _status;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

void KP::sendMessage(char type, Triple *t , char subid[MAX_SUBID_SIZE] , WiFiClient *comm) {

  #ifdef DEBUG
  Serial.println(F("-----------COMPOSING----------"));
  #endif

  char state = 'i'; //first part of the contents chain will always be the transaction id

  char buffer[MAX_BUFFER_SIZE] = "";

  char* loc;
  short pos = 0;

  Triple *index = t;

  Contents curr;

  switch (type) {

    case 'j':
      strcpy(buffer, joinTemplate);
      break;

    case 'i':
      strcpy(buffer, insertTemplate);
      break;

    case 'q':
      strcpy(buffer, queryTemplate);
      break;

    case 's':
      strcpy(buffer, subscribeTemplate);
      break;

    case 'l':
      strcpy(buffer, leaveTemplate);
      break;

  }

  //----------------------START MESSAGE CREATION--------------------

  do {

    switch (state) {

      case 'i':   //TRANSACTION ID
        strcpy_P(curr.type, PSTR("<transaction_id>"));
        itoa(_trID, curr.content, 10);
        state = 'n';
        break;

      case 'n':   //NODE ID
        strcpy_P(curr.type, PSTR("<node_id>"));
        strcpy(curr.content, _nodeID);

        if (type == 'j' || type == 'l') state = 'z'; //"finished"
        else state = type; //per ogni altra transazione type = stato

        break;

      case 's':   //SUBSCRIBE (same field of query)
        ;

      case 'q':   //QUERY
        strcpy_P(curr.type, PSTR("<parameter name = \"query\">"));
        strcpy(curr.content, _query);
        state = 'z';
        break;

      case 'u':   //UNSUBSCRIBE
        strcpy_P(curr.type, PSTR("<parameter name = \"subscription_id\">"));
        strcpy(curr.content, subid);
        state = 'z';

        break;

    }

    loc = strstr(buffer, curr.type);  //locate the tag substring inside the template

    if (loc != NULL) {
      pos = loc - buffer + strlen(curr.type); //posizionati alla fine del tag cercato
      append(buffer, curr.content, pos);
    }

    else {
      Serial.println(F("SUBSTRING NOT FOUND"));
      return;
    }

    //----------------------TRIPLE INSERT--------------------------

    if (state == 'i') { //INSERT set of triples to insert into the buffer
      loc = strstr(buffer, "<triple_list>");  //trova il punto di inizio della lista

      if (loc != NULL) {

        char  triple[100 * 2] = ""; //17= 1 + 2*strlen("<triple>")
        pos = loc - buffer + strlen("<triple_list>"); //posizionati all'inizio della lista

        while (index != NULL) {  //fill the triple string buffer with details of the triple

          strcat_P(triple, PSTR("<triple><subject type="));
          strcat(triple, index->subtype);
          strcat_P(triple, PSTR(">"));
          strcat(triple, index->subject); //copia il soggetto della stringa buffer della tripla
          strcat_P(triple, PSTR("</subject><predicate>")); //copia i campi xml accessori
          strcat(triple, index->predicate);
          strcat_P(triple, PSTR("</predicate><object type=\"uri\">"));
          strcat(triple, index->object);
          strcat_P(triple, PSTR("</object></triple>"));


          append(buffer, triple, pos);  //metti la tripla nel buffer (pos deve essere su '>' di <triple_list>

          pos = pos + strlen(triple); //aggiorna la posizione per eventuale altre triple

          strcpy(triple, ""); //svuota il buffer per la tripla

          index = index->next; //aggiorna il puntatore
        }

      }
      state = 'z'; //finisci di generare contenuto per l'insert
    }

    delay(YIELDING_TIME);

  } while (state != 'z') ;

  //Serial.println(buffer);
  comm->print(buffer);

  _status = OK;
  return;

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

void KP::receiveReply(char type) {

  char buffer[MAX_BUFFER_SIZE] = "";
  char search[50] = "", result[50] = "";
  char state = 'c', *loc = NULL; //partire da integrità logica del messaggio (es JOIN=JOIN)

  short i = 0, pos = 0;

  byte tries = 0;

  bool replied = false;

  Serial.println(F("WAITING FOR RESPONSE"));

  while (!replied && tries++ <= 3) {
    if (_comm.available()) {
      char c;
      replied = true;
      while (_comm.available()) {
        buffer[i++] = _comm.read();
      }
    }
    else {
      delay(TIME_BETWEEN_TRIES);
    }
  }

  if (!replied) {
    Serial.println(F("NO REPLY"));
    return;
  }

  else Serial.println(buffer);

  //ricerca dei vari risultati
  //!!!!!!!!!!!!!!!!! -> Fare il check che la transazione nella risposta == quello che ci si aspetta (es. JOIN in risposta a JOIN)

  do {

    switch (state) {

      case 'i': //integrity
        switch (type) {
          case 'j':
            ;
        }


      case 'c': //message type
        strcpy_P(search, PSTR("<message_type>")); //CONFIRM ; INDICATION ;
        if (type == 'j' || type == 'l' || type == 'u' || type == 'i')  state = 'f'; //finished
        else return; //altri tipi
        break;

      case 's': //sub_id, for subscribe and notification
        break;

      case 't': //triples , for query and notification
        break;


      case 'f':
        break;

    }

    loc = strstr(buffer, search); //ricerca il risultato
    Serial.print("Searching for : ");
    Serial.println(search);

    if (loc != NULL) {
      pos = loc - buffer + strlen(search);

      for (i = pos; buffer[i] != '<'; i++) {
        result[i - pos] = buffer[i];
      }

      Serial.print("RESULT: ");
      Serial.println(result);
    }


  } while (state != 'f');



}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

