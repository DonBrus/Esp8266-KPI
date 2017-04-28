#include "KPI.h"
#include <Ticker.h>

KP kpi("Huzza");


#define NETSSID "Alice-55361560"
#define NETPSW  "20c1af385935b78a3mea1385"

//#define NETSSID "GATEWAY"
//#define NETPSW  "abcd1234"

void setup() {
  Serial.begin(57600);

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

  checkList(list);

  kpi.join();

  kpi.insert(list);

  kpi.query("PREFIX ss: <htttp://arduArm.com/ss#> SELECT ?arm ?task WHERE {?arm ss:hasTask ?task}");

  kpi.subscribe("PREFIX ss: <http://arduArm.com/ss#> SELECT ?arm ?task WHERE {?arm ss:hasTask ?task}");

  kpi.leave();


}

void loop() {

}

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

