#include "KPI.h"

#define WIFI_TIMEOUT 10000

bool WiFiConnect(const char ssid[], const char pass[]);

void append(char buffer[], const char cont[], int pos);
