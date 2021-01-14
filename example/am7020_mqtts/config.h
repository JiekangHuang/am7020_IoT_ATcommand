#ifndef _CONFIG_H
#define _CONFIG_H

#if defined(__AVR__)
#include <avr/pgmspace.h>
#define SIM7020_PROGMEM PROGMEM
#else
#define SIM7020_PROGMEM
#endif

#define UPLOAD_INTERVAL 60000

#define SerialMon Serial
#define MONITOR_BAUDRATE 115200

/* Define the serial console for debug prints, if needed */
// #define SIM7020_DEBUG SerialMon
/* uncomment to dump all AT commands */
// #define DEBUG_DUMP_AT_COMMAND

/* AM7020 module setup: Serial port, baudrate, and reset pin */
#if (defined ARDUINO_AVR_UNO) || (defined ARDUINO_AVR_NANO)
/* Arduino Uno */
#include <SoftwareSerial.h>
SoftwareSerial SoftSerial(8, 9);     // RX:8 TX:9
#define SerialAT SoftSerial
#define AM7020_BAUDRATE 38400
#define AM7020_RESET 7

#elif defined ARDUINO_AVR_MEGA2560
/* Arduino Mega2560 */
#define SerialAT Serial1
#define AM7020_BAUDRATE 115200
#define AM7020_RESET 7

#elif (defined ARDUINO_SAMD_MKRZERO) || (defined ARDUINO_SAMD_MKRWIFI1010)
/* Arduino MKR Series */
#define SerialAT Serial1
#define AM7020_BAUDRATE 115200
#define AM7020_RESET A5

#elif defined ARDUINO_ESP32_DEV
/* ESP32 Boards */
#define SerialAT Serial2
#define AM7020_BAUDRATE 115200
#define AM7020_RESET 5

#else
/* add your own boards and uncomment it */
/*
#define SerialAT            Serial1
#define AM7020_BAUDRATE     9600
#define AM7020_RESET        7
*/

#endif

// for taiwan mobile
#define APN "twm.nbiot"
#define BAND 28

// for CHT
//#define APN               "internet.iot"
//#define BAND              8


// MQTT Setting
#define MQTT_BROKER         "test.mosquitto.org"
#define MQTT_PORT           8883
#define MQTT_USERNAME       ""
#define MQTT_PASSWORD       ""
#define MQTT_TOPIC          "temp/humidity"

/* ---test.mosquitto.org root certificate--- */
const char root_ca[] SIM7020_PROGMEM = {"-----BEGIN CERTIFICATE-----\\r\\n"
                                        "MIIEAzCCAuugAwIBAgIUBY1hlCGvdj4NhBXkZ/uLUZNILAwwDQYJKoZIhvcNAQEL\\r\\n"
                                        "BQAwgZAxCzAJBgNVBAYTAkdCMRcwFQYDVQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwG\\r\\n"
                                        "A1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1vc3F1aXR0bzELMAkGA1UECwwCQ0ExFjAU\\r\\n"
                                        "BgNVBAMMDW1vc3F1aXR0by5vcmcxHzAdBgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hv\\r\\n"
                                        "by5vcmcwHhcNMjAwNjA5MTEwNjM5WhcNMzAwNjA3MTEwNjM5WjCBkDELMAkGA1UE\\r\\n"
                                        "BhMCR0IxFzAVBgNVBAgMDlVuaXRlZCBLaW5nZG9tMQ4wDAYDVQQHDAVEZXJieTES\\r\\n"
                                        "MBAGA1UECgwJTW9zcXVpdHRvMQswCQYDVQQLDAJDQTEWMBQGA1UEAwwNbW9zcXVp\\r\\n"
                                        "dHRvLm9yZzEfMB0GCSqGSIb3DQEJARYQcm9nZXJAYXRjaG9vLm9yZzCCASIwDQYJ\\r\\n"
                                        "KoZIhvcNAQEBBQADggEPADCCAQoCggEBAME0HKmIzfTOwkKLT3THHe+ObdizamPg\\r\\n"
                                        "UZmD64Tf3zJdNeYGYn4CEXbyP6fy3tWc8S2boW6dzrH8SdFf9uo320GJA9B7U1FW\\r\\n"
                                        "Te3xda/Lm3JFfaHjkWw7jBwcauQZjpGINHapHRlpiCZsquAthOgxW9SgDgYlGzEA\\r\\n"
                                        "s06pkEFiMw+qDfLo/sxFKB6vQlFekMeCymjLCbNwPJyqyhFmPWwio/PDMruBTzPH\\r\\n"
                                        "3cioBnrJWKXc3OjXdLGFJOfj7pP0j/dr2LH72eSvv3PQQFl90CZPFhrCUcRHSSxo\\r\\n"
                                        "E6yjGOdnz7f6PveLIB574kQORwt8ePn0yidrTC1ictikED3nHYhMUOUCAwEAAaNT\\r\\n"
                                        "MFEwHQYDVR0OBBYEFPVV6xBUFPiGKDyo5V3+Hbh4N9YSMB8GA1UdIwQYMBaAFPVV\\r\\n"
                                        "6xBUFPiGKDyo5V3+Hbh4N9YSMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEL\\r\\n"
                                        "BQADggEBAGa9kS21N70ThM6/Hj9D7mbVxKLBjVWe2TPsGfbl3rEDfZ+OKRZ2j6AC\\r\\n"
                                        "6r7jb4TZO3dzF2p6dgbrlU71Y/4K0TdzIjRj3cQ3KSm41JvUQ0hZ/c04iGDg/xWf\\r\\n"
                                        "+pp58nfPAYwuerruPNWmlStWAXf0UTqRtg4hQDWBuUFDJTuWuuBvEXudz74eh/wK\\r\\n"
                                        "sMwfu1HFvjy5Z0iMDU8PUDepjVolOCue9ashlS4EB5IECdSR2TItnAIiIwimx839\\r\\n"
                                        "LdUdRudafMu5T5Xma182OC0/u/xRlEm+tvKGGmfFcN0piqVl8OrSPBgIlb+1IKJE\\r\\n"
                                        "m/XriWr/Cq4h/JfB7NTsezVslgkBaoU=\\r\\n"
                                        "-----END CERTIFICATE-----"};

#endif /* _CONFIG_H */
