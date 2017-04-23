#include "KPI.h"
#include <Ticker.h>

KP kpi;
Ticker yielder;

#define NETSSID "Alice-55361560"
#define NETPSW  "20c1af385935b78a3mea1385"

void wait() {
  static byte count = 0;
  count++;
  if (count > 30) {
    delay(10);
    count = 0;
  }
}

void setup() {
  Serial.begin(57600);
  //yielder.attach(0.1, wait);

  do {
    kpi.begin(NETSSID, NETPSW, 10010, 192, 168, 1, 14);
  } while (kpi.getState() != 0);

  delay(500); //for esp bg functions

  Triple* list = new Triple;
  Triple  *c;


  strcpy(list->subject, "http://ns#a");
  strcpy(list->predicate, "http://ns#b");
  strcpy(list->object, "http://ns#c");
  list->next = new Triple;
  c=list->next;
  strcpy(c->subject, "http://ns#a2");
  strcpy(c->predicate, "http://ns#b2");
  strcpy(c->object, "http://ns#c2");
  c->next=NULL;

  checkList(list);

  kpi.join();

  kpi.insert(list);

  kpi.leave();


}

void loop() {

}

void makeTriple(char baseURI[MAX_URI_SIZE],char sub[MAX_URI_SIZE],char pred[MAX_URI_SIZE],char obj[MAX_URI_SIZE],Triple *list){
  
}

void checkList(Triple *list){
  Triple *index=list;
  byte i=1;
  while(index!=NULL){
    Serial.print("Triple number ");
    Serial.print(i++);
    Serial.println(" is:");
    Serial.println(index->subject);
    Serial.println(index->predicate);
    Serial.println(index->object);
    index=index->next;
  }
}

