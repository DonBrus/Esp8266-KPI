#include "KPI.h"
#include <Ticker.h>

KP kpi;
Ticker yielder;

#define NETSSID "Alice-55361560"
#define NETPSW  "20c1af385935b78a3mea1385"

void wait(){
  static byte count=0;
  count++;
  if(count>30){
    delay(10);
    count=0;
  }
}

void setup() {
  Serial.begin(57600);
  //yielder.attach(0.1, wait);

  do {
    kpi.begin(NETSSID, NETPSW, 10010, 192, 168, 1, 14);
  } while (kpi.getState() != 0);

  delay(500); //for esp bg functions

  Triple* first,*index;
  
  first=new Triple;
  strcpy(first->subject,"1");
  first->next=new Triple;
  index=first->next;
  strcpy(index->subject,"2");
  index->next=NULL;

  index=first;
  do{
    Serial.println(index->subject);
    index=index->next;
  }while(index->next!=NULL);
  

  kpi.join();
  kpi.join();

  kpi.leave();


}

void loop() {

}

