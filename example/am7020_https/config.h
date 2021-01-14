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
//#define TINY_GSM_DEBUG SerialMon
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

/* set GSM PIN */
#define GSM_PIN ""

// for taiwan mobile
#define APN "twm.nbiot"
#define BAND 28

// for CHT
//#define APN               "internet.iot"
//#define BAND              8

#define HTTP_SERVER "httpbin.org"
#define HTTP_GET_API "/anything"
#define HTTP_POST_API HTTP_GET_API

/* ---https://httpbin.org root certificate--- */
const char root_ca[] SIM7020_PROGMEM = {"-----BEGIN CERTIFICATE-----\n"
                                        "MIIEdTCCA12gAwIBAgIJAKcOSkw0grd/MA0GCSqGSIb3DQEBCwUAMGgxCzAJBgNV\n"
                                        "BAYTAlVTMSUwIwYDVQQKExxTdGFyZmllbGQgVGVjaG5vbG9naWVzLCBJbmMuMTIw\n"
                                        "MAYDVQQLEylTdGFyZmllbGQgQ2xhc3MgMiBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0\n"
                                        "eTAeFw0wOTA5MDIwMDAwMDBaFw0zNDA2MjgxNzM5MTZaMIGYMQswCQYDVQQGEwJV\n"
                                        "UzEQMA4GA1UECBMHQXJpem9uYTETMBEGA1UEBxMKU2NvdHRzZGFsZTElMCMGA1UE\n"
                                        "ChMcU3RhcmZpZWxkIFRlY2hub2xvZ2llcywgSW5jLjE7MDkGA1UEAxMyU3RhcmZp\n"
                                        "ZWxkIFNlcnZpY2VzIFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IC0gRzIwggEi\n"
                                        "MA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDVDDrEKvlO4vW+GZdfjohTsR8/\n"
                                        "y8+fIBNtKTrID30892t2OGPZNmCom15cAICyL1l/9of5JUOG52kbUpqQ4XHj2C0N\n"
                                        "Tm/2yEnZtvMaVq4rtnQU68/7JuMauh2WLmo7WJSJR1b/JaCTcFOD2oR0FMNnngRo\n"
                                        "Ot+OQFodSk7PQ5E751bWAHDLUu57fa4657wx+UX2wmDPE1kCK4DMNEffud6QZW0C\n"
                                        "zyyRpqbn3oUYSXxmTqM6bam17jQuug0DuDPfR+uxa40l2ZvOgdFFRjKWcIfeAg5J\n"
                                        "Q4W2bHO7ZOphQazJ1FTfhy/HIrImzJ9ZVGif/L4qL8RVHHVAYBeFAlU5i38FAgMB\n"
                                        "AAGjgfAwge0wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0O\n"
                                        "BBYEFJxfAN+qAdcwKziIorhtSpzyEZGDMB8GA1UdIwQYMBaAFL9ft9HO3R+G9FtV\n"
                                        "rNzXEMIOqYjnME8GCCsGAQUFBwEBBEMwQTAcBggrBgEFBQcwAYYQaHR0cDovL28u\n"
                                        "c3MyLnVzLzAhBggrBgEFBQcwAoYVaHR0cDovL3guc3MyLnVzL3guY2VyMCYGA1Ud\n"
                                        "HwQfMB0wG6AZoBeGFWh0dHA6Ly9zLnNzMi51cy9yLmNybDARBgNVHSAECjAIMAYG\n"
                                        "BFUdIAAwDQYJKoZIhvcNAQELBQADggEBACMd44pXyn3pF3lM8R5V/cxTbj5HD9/G\n"
                                        "VfKyBDbtgB9TxF00KGu+x1X8Z+rLP3+QsjPNG1gQggL4+C/1E2DUBc7xgQjB3ad1\n"
                                        "l08YuW3e95ORCLp+QCztweq7dp4zBncdDQh/U90bZKuCJ/Fp1U1ervShw3WnWEQt\n"
                                        "8jxwmKy6abaVd38PMV4s/KCHOkdp8Hlf9BRUpJVeEXgSYCfOn8J3/yNTd126/+pZ\n"
                                        "59vPr5KW7ySaNRB6nJHGDn2Z9j8Z3/VyVOEVqQdZe4O/Ui5GjLIAZHYcSNPYeehu\n"
                                        "VsyuLAOQ1xk4meTKCRlb/weWsKh/NEnfVqn3sF/tM+2MR7cwA130A4w=\n"
                                        "-----END CERTIFICATE-----"};

#endif /* _CONFIG_H */