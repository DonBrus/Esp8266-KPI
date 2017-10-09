#include "KPI.h"
#include <Ticker.h>
#include <Wire.h>

KP kpi("NodeX");

#define SLAVE_INT_PIN 12
#define SLAVE_INT_IN 14


#define NETSSID "GATEWAY"
#define NETPSW  "abcd1234"
#define IP 10010, 192, 168, 43, 36

boolean armOk = true, reported = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();   //join i2c as master

  pinMode(SLAVE_INT_PIN, OUTPUT);  //used for master triggered interrupt
  digitalWrite(SLAVE_INT_PIN, HIGH);

  pinMode(SLAVE_INT_IN, INPUT);  //used for master triggered interrupt
  attachInterrupt(SLAVE_INT_IN, Report, FALLING);

  do {
    kpi.begin(NETSSID, NETPSW, IP);
  } while (kpi.getState() != 0);

  delay(YIELDING_TIME); //for esp bg functions

  Triple  *list = new Triple;
  Triple  *c;


  strcpy(list->subject, "http://arduArm.com/ss#NodeX");
  strcpy(list->predicate, "http://arduArm.com/ss#IsActive");
  strcpy(list->object, "http://arduArm.com/ss#True");
  list->next = new Triple;
  c = list->next;
  strcpy(c->subject, "http://arduArm.com/ss#NodeX");
  strcpy(c->predicate, "http://arduArm.com/ss#Supervise");
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


  Serial.println("Subbed. Going to Loop...");
}

void loop() {

  static boolean newTask = false;
  static char Task = 's'; //byte to send to the slave whenever there's a change in context

  if (kpi.notification() && armOk) {

    Serial.println("NEW RESULTS!");
    Result *r = kpi.getResults();
    checkResults(r);

    newTask = true;

    if (strstr(r->var1, "#Stop") != NULL) {
      Serial.println("ARM : STOP");
      Task = 's';
    }

    else  if (strstr(r->var1, "#Task1") != NULL) {
      Serial.println("ARM : TASK 1");
      Task = '1';
    }

    else  if (strstr(r->var1, "#Task2") != NULL) {
      Serial.println("ARM : TASK 2");
      Task = '2';
    }

    else {
      Serial.println("ARM : TASK NOT FOUND");
      Task = 'n'; //null , for error management
      newTask = false; //don't wake slave up
    }

    if (newTask) {

      Serial.print("Writing to Slave: ");
      Serial.println(Task);

      digitalWrite(SLAVE_INT_PIN, LOW); //inform slave of new task
      delay(100); //wait a bit so we're sure it's been received
      digitalWrite(SLAVE_INT_PIN, HIGH); //release

      Wire.beginTransmission(8); // transmit to device #8

      Wire.write(Task); //send new Task

      Wire.endTransmission();

      newTask = false;
    }

  }

  else if (!armOk && !reported) {
    Serial.println("REPORTING TO SUB...");

    reported = true;

    Triple  *list = new Triple;

    strcpy(list->subject, "http://arduArm.com/ss#MeArm");
    strcpy(list->predicate, "http://arduArm.com/ss#HasStatus");
    strcpy(list->object, "http://arduArm.com/ss#Faulty");
    list->next = NULL;
    checkList(list);

    if (kpi.insert(list)) Serial.println("REPORTED");

    yield();


    delay(1000);

  }

}

void checkList(Triple * list) {
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

void checkResults(Result * list) {
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

void Report() {

  char c;

  Serial.println("SLAVE REPORTING");

  Wire.requestFrom(8, 1);    // request 1 byte (sensor data or error)

  while (Wire.available()) { // slave may send less than requested
    c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  if (c == 'F' or c == 'f') {
    armOk = false;
  }

}

