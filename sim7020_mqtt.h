/*
sim7020_mqtt.h
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/10/29 下午2:57:02
*/

#ifndef _SIM7020_MQTT_H
#define _SIM7020_MQTT_H

#include "sim7020_modem.h"
#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

#define NUM_OF_SUB 8
#define CONN_BROKER_TIMEOUT_MS 90000

typedef struct {
    const char *topic;
    void (*callback)(const char *msg);
} subscribe_t;

class SIM7020EMQTT : public SIM7020Modem {
  private:
    uint8_t     sub_sum;
    subscribe_t subs[NUM_OF_SUB];

    virtual bool newMQTT(const char *server, uint16_t port);
    virtual bool chkMQTTChOpen(void);
    bool         connMQTT(const char *mqtt_id, const char *username = NULL, const char *password = NULL);
    bool         chkMQTTChConn(void);
    bool         closeMQTTCh(void);
    bool         setSyncMode(uint8_t value);

  public:
    SIM7020EMQTT(Stream &SerialAT);
    ~SIM7020EMQTT();

    bool connBroker(const char *server, uint16_t port = 1883, const char *username = NULL, const char *password = NULL,
                    const char *mqtt_id = NULL);
    bool chkConnBroker(void);
    bool publish(const char *topic, const char *msg, uint8_t qos = 0);
    bool subscribe(const char *topic, void (*callback)(const char *msg), uint8_t qos = 0);
    bool unSubscribe(const char *topic);
    void procSubs(void);
    void ParseSubMsg(void);
};

#endif /* _SIM7020_MQTT_H */