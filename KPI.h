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
#define MAX_URI_SIZE 15
#define MAX_ID_SIZE 20
#define MAX_SUBSCRIPTION_SIZE 30

#define TIME_BETWEEN_TRIES  300
#define YIELDING 
#define YIELDING_TIME 20

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
<<<<<<< HEAD
    void query(char query[MAX_QUERY_SIZE]);  //SPARQL
    void subscribe(char *q);  //SPARQL
    void unsubscribe(char sub);  //SPARQL
=======
    void query(Triple *t);  //SPARQL
    void subscribe(Triple *t);  //SPARQL
    void unsubscribe(Triple *t);  //SPARQL
>>>>>>> parent of e99219e... "create" still standalone
    void leave();



  private:
    char _nodeID[MAX_ID_SIZE];
    short _trID;

    short _port; //port used by the SIB server
    byte _ip[4];
    char _subID1[MAX_SUBSCRIPTION_SIZE];
    char _subID2[MAX_SUBSCRIPTION_SIZE];

    byte _status;

    WiFiClient _comm; //socket used for main communication
    WiFiClient _s1 , _s2 , _last; //two socket available for communication , plus one eventually used for the "last will"


    void receive();

    void sendMessage(char code, Triple *t);
    void receiveReply(char type);

    void parserHelper(char type,char *state,char *search);
    Contents create(char type, char *state);
    void transaction(char type, Triple *t);

};

byte processChar(const char c, byte lastState);

#include "utils.h"
#include "wrappers.h"

#endif
