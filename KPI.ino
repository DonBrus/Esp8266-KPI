#include "KPI.h"
#include <Ticker.h>

KP kpi("Huzza");
//Ticker yielder;

#define NETSSID "Alice-55361560"
#define NETPSW  "20c1af385935b78a3mea1385"

//#define NETSSID "GATEWAY"
//#define NETPSW  "abcd1234"

void wait() {
  static byte count = 0;
  count++;
  if (count > 30) {
    yield();
    count = 0;
  }
}

void setup() {
  Serial.begin(57600);
  //yielder.attach(0.5, wait);

  do {
    kpi.begin(NETSSID, NETPSW, 10010, 192, 168, 1, 14);
    //kpi.begin(NETSSID, NETPSW, 10010, 192, 168, 43, 52);
  } while (kpi.getState() != 0);

  delay(500); //for esp bg functions

  Triple  *list = new Triple;
  Triple  *c;


  strcpy(list->subject, "http://ns#a");
  strcpy(list->predicate, "http://ns#b");
  strcpy(list->object, "http://ns#c");
  list->next = new Triple;
  c = list->next;
  strcpy(c->subject, "http://ns#a2");
  strcpy(c->predicate, "http://ns#b2");
  strcpy(c->object, "http://ns#c2");
  c->next = NULL;

  //  makeTriple("http://ns#","Mary","Has","Lamb",list);
  //  c=list;
  //  makeTriple("http://ns#","Jack","Is","Dull",c->next);
  //  c=c
  //  c->next=NULL;

  checkList(list);

  kpi.join();

  //kpi.insert(list);

  kpi.query("PREFIX ss: <htttp://arduArm.com/ss#> SELECT ?arm ?task WHERE {?arm ss:hasTask ?task}");

  kpi.leave();


}

void loop() {

}

//void makeTriple(char baseURI[MAX_URI_SIZE],char sub[MAX_URI_SIZE],char pred[MAX_URI_SIZE],char obj[MAX_URI_SIZE],Triple *i){
//  char tmp[MAX_URI_SIZE*2];
//  i=new Triple;
//
//  strcat(tmp,baseURI);
//  strcat(tmp,sub);
//  strcpy(i->subject,tmp);
//  strcpy(tmp,"");
//
//  strcat(tmp,baseURI);
//  strcat(tmp,pred);
//  strcpy(i->predicate,tmp);
//  strcpy(tmp,"");
//
//  strcat(tmp,baseURI);
//  strcat(tmp,obj);
//  strcpy(i->object,tmp);
//  strcpy(tmp,"");
//
//}

void checkList(Triple *list) {
  Triple *index = list;
  byte i = 1;
  while (index != NULL) {
    Serial.print("Triple number ");
    Serial.print(i++);
    Serial.println(" is:");
    Serial.println(index->subject);
    Serial.println(index->predicate);
    Serial.println(index->object);
    index = index->next;
  }
}

