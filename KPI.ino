#include "KPI.h"
#include <Ticker.h>

KP kpi("Huzzah");


//#define NETSSID "Alice-55361560"
//#define NETPSW  "20c1af385935b78a3mea1385"

#define NETSSID "GATEWAY"
#define NETPSW  "abcd1234"

void setup() {
  Serial.begin(115200);

  do {
    //kpi.begin(NETSSID, NETPSW, 10010, 192, 168, 1, 14);
    kpi.begin(NETSSID, NETPSW, 10010, 192, 168, 43, 52);
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

  char q[] = "PREFIX ss: &lt;http://arduArm.com/ss#&gt; SELECT ?state WHERE {ss:Huzzah ss:LedIs ?state} LIMIT 1";

  if (kpi.join()) Serial.println("JOIN OK");
  yield();

  //  if (kpi.insert(list))Serial.println("INSERT OK");
  //  yield();


  //  if (kpi.query(q)) Serial.println("QUERY OK");
  //  yield();


  if (kpi.subscribe(q))  Serial.println("SUB OK");
  yield();


  //  kpi.unsubscribe(1);
  //  yield();


  //  if (kpi.leave()) Serial.println("LEAVE OK");
  //  yield();
  //
  //
  //  Result *r = kpi.getResults();
  //  checkResults(r);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {

  if (kpi.notification()) {
    Serial.println("NEW RESULTS!");
    Result *r = kpi.getResults();
    checkResults(r);
    if (strstr(r->var1, "#On") != NULL) digitalWrite(LED_BUILTIN, LOW);
    else  if (strstr(r->var1, "#Off") != NULL)  digitalWrite(LED_BUILTIN, HIGH);
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

