#ifndef KPI.h
#define KPI.h

#if ARDUINO>=100  //NECESSARIO PER PROGMEM
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WCostants.h"

#endif

#include "codes.h"
#include "templates.h"
#include <ESP8266WiFi.h>

#define DEBUG 1

#define MAX_SSID_LENGTH 50
#define MAX_PSW_LENGTH 50

#define MAX_BUFFER_SIZE 1500
#define MAX_NAME_SIZE 200
#define MAX_CONTENT_SIZE 300
#define MAX_URI_SIZE 200
#define MAX_ID_SIZE 50
#define MAX_SUBID_SIZE 100
#define MAX_QUERY_SIZE 300
#define MAX_VAR_SIZE 200

#define MAX_SUBS 2

#define TIME_BETWEEN_TRIES  300
#define YIELDING_TIME 50

struct Contents {
  char type[MAX_NAME_SIZE] = "";
  char content[MAX_CONTENT_SIZE] = "";
};

struct Triple {
  char subject[MAX_URI_SIZE];
  char predicate[MAX_URI_SIZE];
  char object[MAX_URI_SIZE];
  char subtype[10] = "\"uri\"";
  char objtype[10] = "uri";
  Triple* next;
};

struct Result {
  char var1[MAX_VAR_SIZE] = "";
  char var2[MAX_VAR_SIZE] = "";
  char var3[MAX_VAR_SIZE] = "";
  Result *next;
};


struct Subscription {
  char subID[MAX_SUBID_SIZE];
  WiFiClient in;
  boolean isActive = false;
};


class KP {
  public:

    KP(char ID[] = "", short TR = 1);

    void begin(char SSID[MAX_SSID_LENGTH], char psw[MAX_PSW_LENGTH], short p = 10010, byte a = 0, byte b = 0, byte c = 0, byte d = 0); //Connect to WiFi network , set SIB parameters

    char* getID();
    short getTR();
    byte getState();//returns the current state of the machine (error code)

    bool join();
    bool insert(Triple *t); //RDF
    bool query(char query[MAX_QUERY_SIZE]);  //SPARQL
    bool subscribe(char query[MAX_QUERY_SIZE]);  //SPARQL
    bool notification(); //SPARQL
    bool unsubscribe(byte id);  //SPARQL
    bool leave();

    Result* getResults();



  private:
    char _nodeID[MAX_ID_SIZE];
    short _trID;

    short _port; //port used by the SIB server
    byte _ip[4];
    byte _status;

    Result* _list = NULL;

    WiFiClient _comm, _last; //socket used for main communication, plus one eventually used for the "last will"
    //Subscription _s1 , _s2 ; //two available subscriptions

    Subscription _s;
    //PER SUBSCRIPTIONS (1 & 2)
    WiFiClient _s1 , _s2;
    char _subID1[MAX_SUBID_SIZE], _subID2[MAX_SUBID_SIZE];
    boolean _is1Active = false , _is2Active = false;

    void sendMessage(char type, Triple *t , char subid[MAX_SUBID_SIZE] , WiFiClient *comm, char query[MAX_QUERY_SIZE]);
    char receiveReply(char type, WiFiClient *comm);
    Result *queryParser(char *buffer);
    void transaction(char type, Triple *t , char subid[MAX_SUBID_SIZE], char query[MAX_QUERY_SIZE] );

};

byte processChar(const char c, byte lastState);

#include "utils.h"
#include "wrappers.h"

#endif
