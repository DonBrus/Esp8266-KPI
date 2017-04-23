#include "KPI.h"

KP::KP(char ID[], short TR ) {
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

void KP::receive() {

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

void KP::analyzeMessage() {

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

Contents KP::create(char type, char *state) {

  Contents c;

  switch (*state) {

    case 'i':
      strcpy_P(c.type, PSTR("<transaction_id>"));
      itoa(_trID, c.content, 10);
      *state = 'n';
      break;

    case 'n':
      strcpy_P(c.type, PSTR("<node_id>"));
      strcpy(c.content, _nodeID);
      if (type == 'j' || type == 'l') *state = 'f'; //"finished"
      else if (type == 'u') *state = 'u';
      else *state = 't';
      break;

    case 'u':
      strcpy_P(c.type, PSTR("<parameter name = \"subscription_id\">"));
      strcpy(c.content, _subID1);
      *state = 'f';

      break;

    //    case 's':
    //      strcpy_P(c.type, PSTR("<subject type=\"uri\">"));
    //      strcpy(c.content, t.subject);
    //      *state = 'p';
    //      break;
    //
    //    case 'p':
    //      strcpy_P(c.type, PSTR("<predicate>"));
    //      strcpy(c.content, t.predicate);
    //      *state = 'o';
    //      break;
    //
    //    case 'o':
    //      strcpy_P(c.type, PSTR("<object type=\"uri\">"));
    //      strcpy(c.content, t.object);
    //      *state = 'f';
    //      break;

    case 'f':
    case 'z':
      *state = 'z';

  }
  return c;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

void KP::sendMessage(char type, Triple *t) {

  #ifdef DEBUG
  Serial.println(F("-----------COMPOSING----------"));
  #endif

  _comm.connect(_ip, _port);
  if (!_comm.connected()) {
    Serial.println("CLIENT ERROR");
    return;
  }

  char cState = 'i'; //first part of the contents chain will always be the transaction id

  char buffer[MAX_BUFFER_SIZE * 2] = {""};

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

    case 'l':
      strcpy(buffer, leaveTemplate);
      break;


  }

  while (cState != 'z') { //z means no more content to be added

    curr = create(type, &cState);

    loc = strstr(buffer, curr.type);  //locate the tag substring inside the template

    if (loc != NULL) {
      pos = loc - buffer + strlen(curr.type); //posizionati alla fine del tag cercato
      append(buffer, curr.content, pos);
    }



    if (cState == 't' && type == 'i') { //set of triples to insert into the buffer
      loc = strstr(buffer, "<triple_list>");  //trova il punto di inizio della lista

      if (loc != NULL) {

        char  triple[100 * 2] = ""; //17= 1 + 2*strlen("<triple>")
        pos = loc - buffer + strlen("<triple_list>"); //posizionati all'inizio della lista

        while (index != NULL) {  //fill the triple string buffer with details of the triple

          strcat_P(triple, PSTR("<triple><subject type=\"uri\">"));
          strcat(triple, index->subject); //copia il soggetto della stringa buffer della tripla
          strcat_P(triple, PSTR("</subject><predicate>")); //copia i campi xml accessori
          strcat(triple, index->predicate);
          strcat_P(triple, PSTR("</predicate><object type=\"uri\">"));
          strcat(triple, index->object);
          strcat_P(triple, PSTR("</object></triple>"));

          Serial.println(triple);

          append(buffer, triple, pos);  //metti la tripla nel buffer (pos deve essere su '>' di <triple_list>

          Serial.println(buffer);

          pos = pos + strlen(triple); //aggiorna la posizione per eventuale altre triple

          strcpy(triple, ""); //svuota il buffer per la tripla

          index = index->next; //aggiorna il puntatore
        }

      }
      cState = 'f'; //finisci di generare contenuto per l'insert
    }

  }

  Serial.println(buffer);
  _comm.print(buffer);
  //_comm.write(4); //EOT


  _status = OK;
  return;


}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------



