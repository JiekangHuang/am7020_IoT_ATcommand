/*
sim7020_mqtts.h
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/10/29 下午2:57:02
*/

#ifndef _SIM7020_MQTTS_H
#define _SIM7020_MQTTS_H

#include "sim7020_mqtt.h"

#if defined(__AVR__)
#include <avr/pgmspace.h>
#endif

class SIM7020EMQTTS : public SIM7020EMQTT {
  private:
    const char *root_ca;
    const char *client_ca;
    const char *client_key;

    bool sendCertificate(int type, const char *cert);
    bool setCert(void);
    bool newMQTT(const char *server, uint16_t port);
    bool chkMQTTChOpen(void);

  public:
    SIM7020EMQTTS(Stream &SerialAT, const char *root_ca, const char *client_ca = NULL, const char *client_key = NULL);
    ~SIM7020EMQTTS();
};

#endif