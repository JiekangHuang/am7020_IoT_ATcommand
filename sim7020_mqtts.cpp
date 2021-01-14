/*
sim7020_mqtts.cpp
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/10/29 下午2:57:32
*/

#include "sim7020_mqtts.h"

SIM7020EMQTTS::SIM7020EMQTTS(Stream &SerialAT, const char *root_ca, const char *client_ca, const char *client_key)
    : SIM7020EMQTT(SerialAT)
{
    this->root_ca    = root_ca;
    this->client_ca  = client_ca;
    this->client_key = client_key;
}

SIM7020EMQTTS::~SIM7020EMQTTS()
{
}

bool SIM7020EMQTTS::sendCertificate(int type, const char *cert)
{
    if (cert == NULL) {
        return false;
    }
    int  total_len = strlen_P(cert);
    int  chunkSize = total_len;
    int  send_len = 0, writted = 0;
    char c;

    while (chunkSize > 0) {
        /*  type 0 : Root CA
            type 1 : Client CA
            type 2 : Client Private Key
        */
        /* Set the Certificate Parameters. refer AT CMD 14.2.6 */
        this->streamWrite(GF("AT+CSETCA="), type, ',', total_len, ',');
        send_len = chunkSize;
        if (send_len > 1000) {
            send_len = 1000;
            chunkSize -= 1000;
        } else {
            chunkSize -= send_len;
        }

        if (chunkSize > 0) {
            // is end = false
            this->streamWrite(GF("1"));
        } else {
            // is end = true
            this->streamWrite(GF("0"));
        }
        // String encoding
        this->streamWrite(GF(",0,\""));
        // send Certificate
        for (int ii = 0; ii < send_len; ii++) {
            c = pgm_read_byte_near(cert + (writted + ii));
            this->streamWrite(c);
        }
        writted += send_len;
        this->streamWrite(GF("\"" GSM_NL));
        if (this->waitResponse() != 1) {
            return false;
        }
        DBG("### send_len : ", send_len);
    }
    return true;
}

bool SIM7020EMQTTS::setCert(void)
{
    if (this->root_ca == NULL) {
        return false;
    }
    bool res = true;
    /* Set the Certificate Parameters */
    res &= this->sendCertificate(0, this->root_ca);
    if (this->client_ca && this->client_key) {
        res &= this->sendCertificate(1, this->client_ca);
        res &= this->sendCertificate(2, this->client_key);
    }
    return res;
}

bool SIM7020EMQTTS::newMQTT(const char *server, uint16_t port)
{
    if (this->setCert()) {
        /* New MQTTS. refer AT CMD 11.2.10 */
        this->sendAT(GF("+CMQTTSNEW=\""), server, GF("\","), port, GF(",30000,1132"));
        if (this->waitResponse(30000L, GF("+CMQTTSNEW: 0")) == 1 && this->waitResponse() == 1) {
            return true;
        }
    }
    return false;
}

bool SIM7020EMQTTS::chkMQTTChOpen(void)
{
    /* New MQTTS. refer AT CMD 11.2.10 */
    this->sendAT(GF("+CMQTTSNEW?"));
    if (this->waitResponse(10000L, GF("+CMQTTSNEW: 0,")) == 1) {
        int16_t used_state = this->streamGetIntBefore(',');
        this->streamSkipUntil('\n');
        this->waitResponse();
        return (used_state == 1);
    }
    return false;
}
