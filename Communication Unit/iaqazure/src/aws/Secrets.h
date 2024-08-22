#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "ESP32_Sensor"                                //change this
 

const char WIFI_SSID[] = "wifi name";                           //change this
const char WIFI_PASSWORD[] = "wifi password";                   //change this
const char AWS_IOT_ENDPOINT[] = "name.us-east-2.amazonaws.com"; //change this
 
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
your certificate
-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                           //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
your certificate
-----END CERTIFICATE-----
)KEY";
 
// Device Private Key                                           //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
your private key
-----END RSA PRIVATE KEY-----
)KEY";