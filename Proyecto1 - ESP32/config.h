/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "tru20844"
#define IO_KEY       "aio_Bgxv43fWChaltjOZHqz9c3nlE2HY"

#define WIFI_SSID "iPhone de Rodris"
#define WIFI_PASS "Dgarcia2005"

#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
