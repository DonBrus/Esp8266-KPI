#define OK 0x00
//error codes relative to WiFi conditions are 0x1y
#define WIFI_OK 0x10
#define WIFI_CONN_ERR 0x11
#define WIFI_CLIENT_ERROR 0x12
#define WIFI_TIME_OUT 0x13

//error codes relative to messages integrity are 0x2y
#define TOO_SHORT 0x21  //if the received message characters count is below a certain threshold
#define SENT_BAD_MESS 0x22  //<SSAP_message /> 
#define TYPE_NOT_EQUAL 0x23 //if the transaction type of the received message doesn't correspond to the expected type
#define NOT_CONFIRMED 0x24 //if the transaction returns an error
#define NO_RESULTS 0x25 //relative to queries (and even notifications?)
#define NO_REPLY 0x26 //timeout due to server not replying back
#define BAD_REPLY 0x27  //happens when certain expected xml tags aren't found inside the file

//error codes relative to general functioning
#define NO_FREE_SUBS 0x31
