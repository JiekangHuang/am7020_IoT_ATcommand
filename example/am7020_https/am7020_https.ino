#include "config.h"
#include "sim7020.h"

#ifdef DEBUG_DUMP_AT_COMMAND
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
SIM7020NB      nb(debugger, AM7020_RESET);
SIM7020EHTTPS  https(debugger, HTTP_SERVER, root_ca);
#else
SIM7020NB     nb(SerialAT, AM7020_RESET);
SIM7020EHTTPS https(SerialAT, HTTP_SERVER, root_ca);
#endif

void nbConnect(void);

void setup()
{
    SerialMon.begin(MONITOR_BAUDRATE);
    SerialAT.begin(AM7020_BAUDRATE);

    nbConnect();
}

void loop()
{

    static unsigned long chk_net_timer  = 0;
    static unsigned long get_data_timer = 0;

    int    state_code;
    String body;

    if (millis() > chk_net_timer) {
        chk_net_timer = millis() + 10000;
        if (!nb.chkNet()) {
            nbConnect();
        }
    }

    if (millis() > get_data_timer) {
        get_data_timer = millis() + UPLOAD_INTERVAL;
        /* Get Most Recent Data */
        SerialMon.println(F("HTTP Get..."));
        https.get(HTTP_GET_API);
        state_code = https.responseStatusCode();
        body       = https.responseBody();

        SerialMon.print(F("GET state code = "));
        SerialMon.println(state_code);
        SerialMon.print(F("body = "));
        SerialMon.println(body);

        /* Create Data */
        SerialMon.println(F("HTTP Post..."));
        https.post(HTTP_POST_API, "application/json", "{\"value\": \"POST\"}");

        state_code = https.responseStatusCode();
        body       = https.responseBody();
        SerialMon.print(F("POST state code = "));
        SerialMon.println(state_code);
        SerialMon.print(F("body = "));
        SerialMon.println(body);
    }
}

void nbConnect(void)
{
    SerialMon.println(F("Initializing modem..."));
    while (!nb.init() || !nb.nbiotConnect(APN, BAND)) {
        SerialMon.print(F("."));
    };

    SerialMon.print(F("Waiting for network..."));
    while (!nb.waitForNetwork()) {
        SerialMon.print(F("."));
        delay(5000);
    }
    SerialMon.println(F(" success"));
}