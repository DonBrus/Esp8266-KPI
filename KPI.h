#ifndef KPI.h
#define KPI.h


#if ARDUINO>=100
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

#ifdef ADV_DEBUG
  #include <MemoryFree.h>
#endif

#define MAX_SSID_LENGTH 20
#define MAX_PSW_LENGTH 20

#define MAX_BUFFER_SIZE 1000
#define MAX_NAME_SIZE 30
#define MAX_CONTENT_SIZE 30
#define MAX_URI_SIZE 30
#define MAX_ID_SIZE 30
#define MAX_SUBID_SIZE 30
#define MAX_QUERY_SIZE 200

#define MAX_SUBS 2

#define TIME_BETWEEN_TRIES  300

struct Contents {
  char type[MAX_NAME_SIZE] = "";
  char content[MAX_CONTENT_SIZE] = "";
};

struct Triple {
  char subject[MAX_URI_SIZE * 2];
  char predicate[MAX_URI_SIZE * 2];
  char object[MAX_URI_SIZE * 2];
  char subtype[10] = "uri";
  char objtype[10] = "uri";
  Triple* next;
};

struct Subscription{
  char subID[MAX_SUBID_SIZE];
  WiFiClient in;
  boolean isActive=false;
};


class KP {
  public:

    KP(char ID[] = "", short TR = 1);

    void begin(char SSID[MAX_SSID_LENGTH], char psw[MAX_PSW_LENGTH], short p = 10010, byte a = 0, byte b = 0, byte c = 0, byte d = 0); //Connect to WiFi network , then to

    char* getID();
    //void setID(char id[]); //default initialization at creation , unless explicitly defined in the instance call
    short getTR();
    byte getState();//returns the current state of the machine (error code)

    void join();
    void insert(Triple *t); //RDF
    void query(char *q);  //SPARQL
    void subscribe(char *q);  //SPARQL
    void unsubscribe(char sub);  //SPARQL
    void leave();



  private:
    char _nodeID[MAX_ID_SIZE];
    short _trID;

    short _port; //port used by the SIB server
    byte _ip[4];
    byte _status;

    WiFiClient _comm, _last; //socket used for main communication, plus one eventually used for the "last will"
    Subscription _s1 , _s2 ; //two available subscriptions

    void sendMessage(char type, Triple *t ,char query[MAX_QUERY_SIZE], char subid[MAX_SUBID_SIZE] ,WiFiClient *comm);
    void receiveReply(char type);

    void parserHelper(char type,char *state,char *search);
    Contents create(char type, char *state,char query[MAX_QUERY_SIZE], char subid[MAX_SUBID_SIZE]);
    void transaction(char type, Triple *t ,char query[MAX_QUERY_SIZE], char subid[MAX_SUBID_SIZE] );

};

byte processChar(const char c, byte lastState);

#include "utils.h"
#include "wrappers.h"

#endif
