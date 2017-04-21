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

Contents KP::create(char type, char *state , Triple t) {

  Contents c;

  /*
     trId,nodeId,unsub,sub,obj,pred
     the state coupled with the type of message that is being built will determine whether there's still content to be built;
     otherwise the returning struct will have a field indicating that no more content is going to be added / searched for
  */

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
      else *state = 's';
      break;

    case 'u':
      strcpy_P(c.type, PSTR("parameter name = \"subscription_id\""));
      strcpy(c.content, _subID1);
      *state = 'f';

      break;

    case 's':
      strcpy_P(c.type, PSTR("subject type=\"uri\""));
      strcpy(c.content, t.subject);
      *state = 'p';
      break;

    case 'p':
      strcpy_P(c.type, PSTR("predicate"));
      strcpy(c.content, t.predicate);
      *state = 'o';
      break;

    case 'o':
      strcpy_P(c.type, PSTR("object type=\"uri\""));
      strcpy(c.content, t.object);
      *state = 'f';
      break;

    case 'f':
    case 'z':
      *state = 'z';

  }
  return c;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

void KP::composeMessage(char type, Triple t) {

  #ifdef DEBUG
  Serial.println(F("-----------COMPOSING----------"));
  #endif

  _comm.connect(_ip, _port);
  if(!_comm.connected()){
    Serial.println("CLIENT ERROR");
    return;
  }

  char c, cState = 'i'; //first part of the contents chain will always be the transaction id
  char state, lastState ; // used by the sequence reader

  char tagName[MAX_NAME_SIZE] = {""}, buffer[MAX_BUFFER_SIZE] = {""};

  byte i = 0, count = 0; //i : needed for knowing the length of buffered tag name

  bool readingName = false, last = true;

  char* loc;
  short pos=0;

  Contents curr = create(type, &cState, t);

  switch (type) {
    
    case 'j':
      strcpy(buffer, joinTemplate);
      break;
      
    case 'l':
      strcpy(buffer, leaveTemplate);
      break;
    

  }

  while (cState != 'z') { //z means no more content to be added

    loc = strstr(buffer, curr.type);
    if (loc != NULL) {
      pos=loc-buffer+strlen(curr.type);
      append(buffer, curr.content,pos);
    }

    curr = create(type, &cState, t);

  }

  _comm.print(buffer);
  //_comm.write(4); //EOT


  _status = OK;
  return;


}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------



