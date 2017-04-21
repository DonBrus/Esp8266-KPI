#define OK 0x00
//error codes relative to WiFi conditions are 0x1*
#define WIFI_OK 0x10
#define WIFI_CONN_ERR 0x11
#define WIFI_BAD_SHIELD 0x12
#define WIFI_TIME_OUT 0x13

//error codes relative to messages integrity are 0x3*
#define TOO_SHORT 0x21  //if the received message characters count is below a certain threshold
#define BAD_STRUCT 0x22  //happens when certain expected xml tags aren't found inside the file
