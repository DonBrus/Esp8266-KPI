#include "KPI.h"
#include <Ticker.h>
#include <Wire.h>

KP kpi("Huzzah");

#define NETSSID "Alice-55361560"
#define NETPSW  "20c1af385935b78a3mea1385"
#define IP 10010, 192, 168, 1, 14

//#define NETSSID "GATEWAY"
//#define NETPSW  "abcd1234"
//#define IP 10010, 192, 168, 43, 52

void setup() {
  Serial.begin(115200);
  Wire.begin();   //join i2c as master

  pinMode(12, OUTPUT);  //used for master triggered interrupt
  digitalWrite(12, HIGH);

  do {
    kpi.begin(NETSSID, NETPSW, IP);
  } while (kpi.getState() != 0);

  delay(YIELDING_TIME); //for esp bg functions

  Triple  *list = new Triple;
  Triple  *c;


  strcpy(list->subject, "http://arduArm.com/ss#Huzzah");
  strcpy(list->predicate, "http://arduArm.com/ss#Wants");
  strcpy(list->object, "http://arduArm.com/ss#Friends");
  list->next = new Triple;
  c = list->next;
  strcpy(c->subject, "http://arduArm.com/ss#Huzzah");
  strcpy(c->predicate, "http://arduArm.com/ss#HasFriend");
  strcpy(c->object, "http://arduArm.com/ss#MeArm");
  c->next = NULL;

  checkList(list);

  char q[] = "PREFIX ss: &lt;http://arduArm.com/ss#&gt; SELECT ?task WHERE {ss:MeArm ss:TaskIs ?task} LIMIT 1";

  if (kpi.join()) Serial.println("JOIN OK");
  yield();

  //  if (kpi.insert(list))Serial.println("INSERT OK");
  //  yield();


  if (kpi.query(q)) Serial.println("QUERY OK");
  yield();


  if (kpi.subscribe(q))  Serial.println("SUB OK");
  yield();
  Result *r = kpi.getResults();
  checkResults(r);

  //  kpi.unsubscribe(1);
  //  yield();


  //  if (kpi.leave()) Serial.println("LEAVE OK");
  //  yield();
  //
  //


  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  if (kpi.notification()) {
    Serial.println("NEW RESULTS!");
    Result *r = kpi.getResults();
    checkResults(r);

    if (strstr(r->var1, "#Stop") != NULL) Serial.println("ARM : STOP");
    else  if (strstr(r->var1, "#Task1") != NULL)  Serial.println("ARM : TASK 1");
    else  if (strstr(r->var1, "#Task2") != NULL)  Serial.println("ARM : TASK 2");
    else Serial.println("ARM : TASK NOT FOUND");

  }
  delay(1000);

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

void checkResults(Result *list) {
  Result *index = list;
  byte i = 1;
  while (index != NULL) {
    Serial.print("Result number ");
    Serial.print(i++);
    Serial.println(" is:");
    Serial.println(index->var1);
    Serial.println(index->var2);
    Serial.println(index->var3);
    index = index->next;
  }
}

