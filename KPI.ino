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


  strcpy(list->subject, "http://ns#a");
  strcpy(list->predicate, "http://ns#b");
  strcpy(list->object, "http://ns#c");
  list->next = NULL;


  kpi.join();

  kpi.insert(list);

  kpi.leave();


}

void loop() {

}

