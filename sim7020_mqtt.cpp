/*
sim7020_mqtt.cpp
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/10/29 下午2:57:32
*/

#include "sim7020_mqtt.h"

SIM7020EMQTT::SIM7020EMQTT(Stream &SerialAT) : SIM7020Modem(SerialAT)
{
    this->sub_sum = 0;
}

SIM7020EMQTT::~SIM7020EMQTT()
{
}

/* MQTT/MQTTS */
/* private */
bool SIM7020EMQTT::newMQTT(const char *server, uint16_t port)
{
    /* New MQTT. refer AT CMD 11.2.1 */
    this->sendAT(GF("+CMQNEW=\""), server, GF("\","), port, GF(",30000,1132"));
    if (this->waitResponse(30000L, GF("+CMQNEW: 0")) == 1 && this->waitResponse() == 1) {
        return true;
    }
    return false;
}

bool SIM7020EMQTT::chkMQTTChOpen(void)
{
    /* New MQTT. refer AT CMD 11.2.1 */
    this->sendAT(GF("+CMQNEW?"));
    if (this->waitResponse(10000L, GF("+CMQNEW: 0,")) == 1) {
        int16_t used_state = this->streamGetIntBefore(',');
        this->streamSkipUntil('\n');
        this->waitResponse();
        return (used_state == 1);
    }
    return false;
}

bool SIM7020EMQTT::connMQTT(const char *mqtt_id, const char *username, const char *password)
{
    // FIXME: MQTT ID & cleanSession要留給User設定?
    /* Send MQTT Connection Packet. refer AT CMD 11.2.2 */
    this->sendAT(GF("+CMQCON=0,4,\""), mqtt_id, GF("\",20000,0,0,\""), username, GF("\",\""), password, GF("\""));
    return (this->waitResponse(30000) == 1);
}

bool SIM7020EMQTT::chkMQTTChConn(void)
{
    /* Send MQTT Connection Packet. refer AT CMD 11.2.2 */
    this->sendAT(GF("+CMQCON?"));
    if (this->waitResponse(10000L, GF("+CMQCON: 0,")) == 1) {
        int16_t conn_state = this->streamGetIntBefore(',');
        this->waitResponse();
        return (conn_state == 1);
    }
    return false;
}

bool SIM7020EMQTT::closeMQTTCh(void)
{
    /* Disconnect MQTT. refer AT CMD 11.2.3 */
    this->sendAT(GF("+CMQDISCON=0"));
    return (this->waitResponse(2000) == 1);
}

bool SIM7020EMQTT::setSyncMode(uint8_t value)
{
    /* Configure MQTT Synchronization Mode. refer AT CMD 11.2.14 */
    this->sendAT(GF("+CMQTSYNC="), value);
    return (this->waitResponse(2000) == 1);
}

/* public */
bool SIM7020EMQTT::connBroker(const char *server, uint16_t port, const char *username, const char *password,
                              const char *mqtt_id)
{
    // Note: 超過keepalive_interval時間會自動斷開。
    unsigned long timer = millis() + CONN_BROKER_TIMEOUT_MS;
    String        temp_mqtt_id;
    if (mqtt_id == NULL) {
        temp_mqtt_id = ("sim7020_mqttid_" + String(random(65535)));
    } else {
        temp_mqtt_id = mqtt_id;
    }

    while (millis() < timer) {
        if (!this->chkMQTTChOpen()) {
            // Delay is used here because the SIM7020 module has a bug.
            delay(1000);
            this->closeMQTTCh();
            if (this->setSyncMode((uint8_t) true)) {
                this->newMQTT(server, port);
            }
            continue;
        } else {
            if (!this->chkMQTTChConn()) {
                this->connMQTT(temp_mqtt_id.c_str(), username, password);
                continue;
            }
            return true;
        }
    }
    return false;
}

bool SIM7020EMQTT::chkConnBroker(void)
{
    return this->chkMQTTChConn();
}

bool SIM7020EMQTT::publish(const char *topic, const char *msg, uint8_t qos)
{
    // TODO: 檢查topic長度是否超過128個字元
    /* Send MQTT Publish Packet. refer AT CMD 11.2.6 */
    this->sendAT(GF("+CMQPUB=0,\""), topic, GF("\","), qos, GF(",0,0,"), strlen(msg), GF(",\""), msg, GF("\""));
    return (this->waitResponse(10000) == 1);
}

bool SIM7020EMQTT::subscribe(const char *topic, void (*callback)(const char *msg), uint8_t qos)
{
    // TODO: 檢查topic長度是否超過128個字元
    if (this->sub_sum <= NUM_OF_SUB) {
        /* Send MQTT Subscribe Packet. refer AT CMD 11.2.4 */
        this->sendAT(GF("+CMQSUB=0,\""), topic, GF("\","), qos);
        this->waitResponse(10000);
        this->subs[this->sub_sum].topic      = topic;
        this->subs[this->sub_sum++].callback = callback;
        DBG(GF("###"), GF("Subscribe Topic["), topic, GF("]Successfully."));
        /* Note: 此library有開啟MQTT Synchronization Mode，只要訂閱數量未超過設定上限(NUM_OF_SUB)都將視為訂閱成功。 */
        return true;
    } else {
        DBG(GF("###"), GF("Subscription limit exceeded !"));
    }
    return false;
}

bool SIM7020EMQTT::unSubscribe(const char *topic)
{
    /* Send MQTT Unsubscribe Packet. refer AT CMD 11.2.5 */
    this->sendAT(GF("+CMQUNSUB=0,\""), topic, GF("\""));
    return (this->waitResponse(10000L) == 1);
}

void SIM7020EMQTT::procSubs(void)
{
    if (this->waitResponse((uint32_t)0, GF("+CMQPUB: ")) == 1) {
        this->ParseSubMsg();
    }
}

void SIM7020EMQTT::ParseSubMsg(void)
{
    // 1234"\n
    String topic, msg;
    if (this->streamSkipUntil(',')) {
        topic = this->streamGetStringBefore(',');
        topic = topic.substring(1, topic.length() - 1);
        if (this->streamSkipUntil('\"')) {
            msg = this->streamGetStringBefore('\n');
            msg = msg.substring(0, msg.length() - 2);
            DBG(GF("### Topic: "), topic);
            DBG(GF("### Msg: "), msg);
            for (int ii = 0; ii < this->sub_sum; ii++) {
                if (strcmp(topic.c_str(), this->subs[ii].topic) == 0) {
                    this->subs[ii].callback(msg.c_str());
                    break;
                }
            }
        }
    }
}
