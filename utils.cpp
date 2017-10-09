#include "utils.h"

bool WiFiConnect(const char ssid[], const char pass[]) {

  WiFi.begin(ssid, pass);

  unsigned long startingTime = millis();

  while (WiFi.status() != WL_CONNECTED) {

    if (millis() - startingTime > WIFI_TIMEOUT) {

      #ifdef DEBUG
      Serial.println(F("WiFi timed out!"));
      #endif

      return WIFI_TIME_OUT;
    }

    delay(50);
    Serial.print(".");

  }

  return WIFI_OK;

}

//--------------------------------------------------------------------------------------------------------------

void append(char buffer[], const char cont[], int pos) {
  char tmp[MAX_BUFFER_SIZE] = {""};

  strncpy(tmp, buffer, pos); // copy at most first pos characters
  short len = strlen(tmp);
  strcpy(tmp + len, cont); // copy all of cont[] at the end
  len += strlen(cont);  // increase the length by length of cont[]
  strcpy(tmp + len, buffer + pos); // copy the rest

  strcpy(buffer, tmp);
}

