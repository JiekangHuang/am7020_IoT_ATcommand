#include "config.h"
#include "sim7020.h"

#ifdef DEBUG_DUMP_AT_COMMAND
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
SIM7020NB      nb(debugger, AM7020_RESET);
SIM7020EMQTTS  mqtts(debugger, root_ca);
#else
SIM7020NB     nb(SerialAT, AM7020_RESET);
SIM7020EMQTTS mqtts(SerialAT, root_ca);
#endif

void nbConnect(void);
void reConnBroker(void);
void callback(const char *msg);

void setup()
{
    SerialMon.begin(MONITOR_BAUDRATE);
    SerialAT.begin(AM7020_BAUDRATE);
    nbConnect();
    reConnBroker();
}

void loop()
{
    static unsigned long chk_net_timer  = 0;
    static unsigned long pub_data_timer = 0;

    if (millis() > chk_net_timer) {
        chk_net_timer = millis() + 10000;
        if (!nb.chkNet()) {
            nbConnect();
        }
        reConnBroker();
    }

    if (millis() > pub_data_timer) {
        pub_data_timer = millis() + UPLOAD_INTERVAL;
        SerialMon.print(F("publish: "));
        SerialMon.print(millis());
        if (mqtts.publish(MQTT_TOPIC, String(millis()).c_str())) {
            SerialMon.println(F("  success"));
        } else {
            SerialMon.println(F("  Fail"));
        }
    }
    mqtts.procSubs();
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

void reConnBroker(void)
{
    if (!mqtts.chkConnBroker()) {
        SerialMon.print(F("Connecting to "));
        SerialMon.print(MQTT_BROKER);
        SerialMon.print(F("..."));
        if (mqtts.connBroker(MQTT_BROKER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD, "MY_AM7020_TEST_MQTTID")) {
            SerialMon.println(F(" success"));
            SerialMon.print(F("subscribe: " MQTT_TOPIC));
            if (mqtts.subscribe(MQTT_TOPIC, callback)) {
                SerialMon.println(F(" success"));
            } else {
                SerialMon.println(F(" fail"));
            }
        } else {
            SerialMon.println(F(" fail"));
        }
    }
}

void callback(const char *msg)
{
    SerialMon.print(MQTT_TOPIC ": ");
    SerialMon.println(msg);
}
