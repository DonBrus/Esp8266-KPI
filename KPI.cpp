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

Result* KP::getResults() {
  return _list;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------

void KP::sendMessage(char type, Triple *t , char subid[MAX_SUBID_SIZE] , WiFiClient *comm, char query[MAX_QUERY_SIZE]) {

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

    case 'u':
      strcpy(buffer, unsubscribeTemplate);
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
        strcpy(curr.content, query);
        state = 'z';
        break;

      case 'u':   //UNSUBSCRIBE
        strcpy_P(curr.type, PSTR("<parameter name = \"subscription_id\">"));
        strcpy(curr.content, subid);
        state = 'z';

        break;

    }

    Serial.println(curr.type);
    Serial.println(curr.content);
    
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

    if (state == 'i' && type == 'i') { //INSERT set of triples to insert into the buffer

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

    //    delay(YIELDING_TIME);

  } while (state != 'z') ;

  Serial.println(buffer);
  delay(YIELDING_TIME);
  comm->println(buffer);
  _status = OK;
  Serial.println("SENT");

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

char KP::receiveReply(char type, WiFiClient *comm) {

  char buffer[MAX_BUFFER_SIZE] = "";
  char search[50] = "";
  char state = 'i', last; //partire da integrità logica del messaggio (es JOIN=JOIN)
  char *loc = NULL;

  short i = 0, pos = 0;

  byte tries = 0;

  bool replied = false;

  Serial.println(F("WAITING FOR RESPONSE"));

  while (!replied && tries++ <= 3) {

    if (comm->available()) {
      char c;
      replied = true;
      while (comm->available()) { //read the response
        buffer[i++] = comm->read();
      }
    }

    else {
      delay(TIME_BETWEEN_TRIES);
    }

  }

  if (!replied) {
    Serial.println(F("NO REPLY"));
    return NO_REPLY;
  }

  else Serial.println(buffer);

  delay(YIELDING_TIME);

  //!!!!!!!!!!!!!!!!!  PARSING !!!!!!!!!!!!!!!!!!!!

  //Check se la risposta è sensata o è stato inviato un messaggio errato : <SSAP_message />
  if (strcmp_P(buffer, PSTR("<SSAP_message />")) == 0) {
    return SENT_BAD_MESS;
  }

  do {

    last = state;

    switch (state) {

      //Cerca transaction type -> prendi prima lettera -> lowercase -> compara al type
      case 'i': //integrity
        strcpy_P(search, PSTR("<transaction_type>"));
        state = 'c';
        break;

      case 'c': //message type
        strcpy_P(search, PSTR("<message_type>")); //CONFIRM ; INDICATION ;
        if (type == 'j' || type == 'l' || type == 'u' || type == 'i')  state = 'f'; //finished
        else if (type == 'q') state = 'r'; //for queries , only results are importante
        else state = 's'; //for sub and not. first get the subscription id
        break;

      case 's': //sub_id, for subscribe and notification
        strcpy_P(search, PSTR("<parameter name=\"subscription_id\">"));
        if (type == 's') state = 'f'; //subscribe only needs the subscription id , notif goes on reading the results
        else state='r'; //for notifications
        break;

      case 'r': //results , for query and notification
        _list = queryParser(buffer);
        if (_list == NULL) return NO_RESULTS;
        return OK;  //stop here as we've searched everything we had to search for, as queries and notifications don't need the following part at this point

    }

    loc = strstr(buffer, search); //ricerca il risultato

    Serial.print("Searching for : ");
    Serial.println(search);

    char result[100] = "";

    if (loc != NULL) {

      pos = loc - buffer + strlen(search);

      for (i = pos; buffer[i] != '<'; i++) {
        result[i - pos] = buffer[i];
      }

      Serial.print("RESULT: ");
      Serial.println(result);

    }

    else return BAD_REPLY; //if tags like transaction_type or confirmation aren't to be found inside the buffer , there's been a problem while receiving  the response

    //!!!!!!!!!!!!!!!!! the two checks below aren't to be made for subscription notifications as their xml fields don't match those ones of the others

    if (type != 'n' && last == 'i' && tolower(result[0]) != type) return TYPE_NOT_EQUAL;
    /*that is , if during the integrity check the first letter ( to lowercase) of the received transaction isn't equal to what we were expecting , there's been an error
       example type 'i' (during an insert) , message : <transaction_type>JOIN</transaction_type>
       result = JOIN , result[0]=J , tolower -> j , 'i'!='j' -> return an error
    */

    if (type != 'n' && last == 'c' && strcmp(result, "CONFIRM") != 0) { //whatever transaction , an error occured and it hasn't been confirmed (could be a re join , not joined , etc..)
    return NOT_CONFIRMED;
  }

  if (type == 's' && last == 's') {
    if (comm == &_s1) strcpy(_subID1, result); //se abbiamo attivato la sub1 ...
      else strcpy(_subID2, result); //o la sub 2
    }

  } while (state != 'f');

  return OK;

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------

/*
   This function helps the receiveReply function to build the list of results in case of a query or notification transaction
   It iterates the reply message in search of results looking for <result> and </result> tags ; variable can be either be 0 (no results), 1,2 or 3
   For every result , a new Result struct node is created , filled (if possible) with variables , then moves on the next one (if any)
   Return values can be either be an empty list or a non-empty one ; in the former case ,this is considered as a NO_RESULTS error (and KP::_list is set as NULL); while in the latter
   case , the list pointer is then moved inside the object and can be requested by the user via the getResults method, which returns the handler to the last created list.
*/
Result* KP::queryParser(char *buffer) {

  if (strstr(buffer, "<results />") != NULL)  return NULL; //this tag is found in the reply whenever there are no results

  char *iloc,  *ibin, *ebin;
  byte var = 0;

  iloc = strstr(buffer, "<result>"); //try to find a result

  Result *list = new Result, *index = list; //create first result struct of the list (we know there's gotta be at least one)

  while (iloc != NULL) { //for every new result

    ibin = strstr(iloc, "<binding"); //go search for the variable binding

    while (ibin != NULL & var < 3) {

      char varb[MAX_VAR_SIZE] = "";
      byte size = 0;
      ibin = strstr(ibin, ">"); //move start of binding to end of <binding..> tag , that is at the start of the variable
      ebin = strstr(ibin, "</binding>"); //also  use this other pointer to get to the end of the variable
      size = ebin - ibin; //size of the variable
      strncpy(varb, ibin + 1, size - 1); //+1 , -1 to take into account "<" and ">"

      Serial.println(varb); //debug

      if (var == 0) strcpy(index->var1, varb);  //fill consecutive fields of the current result (from none , up to three variables)
      else if (var == 1) strcpy(index->var2, varb);
      else strcpy(index->var3, varb);

      ibin = strstr(ibin, "<binding");  //start looking for another binding
      var++;  //up to three variable max
      delay(0);

    }

    var = 0; //reset variables counter
    iloc = strstr(iloc + 1, "<result>"); //go search for another result (+1 to avoid repeating always the same one)
    if (iloc == NULL)  index->next = NULL;
    else {
      index->next = new Result;
      index = index->next;
    }

  }
  return list;
}
