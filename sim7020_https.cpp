/*
sim7020_https.cpp
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/11/2 下午1:16:02
*/

#include "sim7020_https.h"

SIM7020EHTTPS::SIM7020EHTTPS(Stream &SerialAT, const char *host, const char *root_ca, const char *client_ca,
                             const char *client_key)
    : SIM7020EHTTP(SerialAT, host)
{
    this->root_ca    = root_ca;
    this->client_ca  = client_ca;
    this->client_key = client_key;
}

SIM7020EHTTPS::~SIM7020EHTTPS()
{
}

/* private */
bool SIM7020EHTTPS::newHTTP(const char *host, const char *username, const char *password)
{
    if (this->root_ca == NULL) {
        return false;
    }
    String temp = "";
    temp.reserve(50);
    int total_len = this->calcPackageLen(host, username, password);

    temp += "https://";
    temp += host;
    this->sendHTTPSinglePackages(1, total_len, temp.c_str());
    this->sendHTTPSinglePackages(1, total_len, username);
    this->sendHTTPSinglePackages(1, total_len, password);
    /* send root ca len & root ca*/
    temp = String(strlen_P(this->root_ca) * 2);     // hex string
    this->sendHTTPSinglePackages(1, total_len, temp.c_str());
    this->sendHTTPCertMultiPackages(1, total_len, this->root_ca);
    /* send client ca len & client ca*/
    if (this->client_ca) {
        temp = String(strlen_P(this->client_ca) * 2);     // hex string
    } else {
        temp = "0";
    }
    this->sendHTTPSinglePackages(1, total_len, temp.c_str());
    this->sendHTTPCertMultiPackages(1, total_len, this->client_ca);
    /* send client key len & client key*/
    if (this->client_key) {
        temp = String(strlen_P(this->client_key) * 2);     // hex string
        this->sendHTTPSinglePackages(1, total_len, temp.c_str());
        this->sendHTTPCertMultiPackages(0, total_len, this->client_key);
    } else {
        temp = "0";
        this->sendHTTPSinglePackages(0, total_len, temp.c_str());
    }

    if (this->waitResponse(60000L, GF("+CHTTPCREATEEXT: 0")) == 1 && this->waitResponse() == 1) {
        return true;
    }
    return false;
}

bool SIM7020EHTTPS::sendHTTPSinglePackages(uint8_t not_end_flag, int total_len, const char *data)
{
    if (data == NULL) {
        this->sendAT(GF("+CHTTPCREATEEXT="), not_end_flag, GF(","), total_len, GF(","), 1, GF(",\",\""));
        this->waitResponse();
        return false;
    }
    int data_len = strlen(data);
    if (data_len > 1000) {
        DBG(GF("### data len too long!!"));
        return false;
    }
    /* Create a HTTPS Client Instance by Multi Packages for a Long Size Command. refer AT CMD 8.2.2 */
    this->sendAT(GF("+CHTTPCREATEEXT=1,"), total_len, GF(","), data_len, GF(",\""), data, GF("\""));
    if (this->waitResponse() != 1) {
        return false;
    }
    this->sendAT(GF("+CHTTPCREATEEXT="), not_end_flag, GF(","), total_len, GF(","), 1, GF(",\",\""));
    this->waitResponse();
    return true;
}

bool SIM7020EHTTPS::sendHTTPCertMultiPackages(uint8_t not_end_flag, int total_len, const char *cert)
{
    if (cert == NULL) {
        this->sendAT(GF("+CHTTPCREATEEXT="), not_end_flag, GF(","), total_len, GF(","), 1, GF(",\",\""));
        this->waitResponse();
        return false;
    }
    int    cert_len  = (strlen_P(cert) * 2);
    int    chunkSize = cert_len;
    int    send_len = 0, writted = 0;
    char   c;
    String hex;

    while (chunkSize > 0) {
        send_len = chunkSize;
        if (send_len > 1000) {
            send_len = 1000;
            chunkSize -= 1000;
        } else {
            chunkSize -= send_len;
        }
        /* Create a HTTPS Client Instance by Multi Packages for a Long Size Command. refer AT CMD 8.2.2 */
        this->streamWrite(GF("AT+CHTTPCREATEEXT="), not_end_flag, GF(","), total_len, GF(","), send_len, GF(",\""));

        // send Certificate
        for (int ii = 0; ii < (send_len / 2); ii++) {
            c   = pgm_read_byte_near(cert + (writted + ii));
            hex = String(c, HEX);
            if (hex.length() <= 1) {
                hex = ("0" + hex);
            }
            this->streamWrite(hex.c_str());
        }
        writted += (send_len / 2);

        this->streamWrite(GF("\"" GSM_NL));
        if (this->waitResponse() != 1) {
            return false;
        }
        DBG("### send_len : ", send_len);
    }
    if (not_end_flag == 1) {
        this->sendAT(GF("+CHTTPCREATEEXT="), not_end_flag, GF(","), total_len, GF(","), 1, GF(",\",\""));
        this->waitResponse();
    }
    return true;
}

int SIM7020EHTTPS::calcPackageLen(const char *host, const char *username, const char *password)
{
    /* doman name len*/
    int total_len = (strlen("https://") + strlen(host) + 1);
    /* user & pass len*/
    // FIXME: String or HEX ??
    if (username && password) {
        total_len += (strlen(username) + 1 + strlen(password) + 1);

    } else {
        total_len += 2;
    }
    /* root_ca_len len & root_ca len*/
    total_len += (String(strlen_P(this->root_ca) * 2).length() + 1 + (strlen_P(this->root_ca) * 2) + 1);

    /* client_ca_len len & client_ca len*/
    if (this->client_ca) {
        total_len += (String(strlen_P(this->client_ca) * 2).length() + 1 + (strlen_P(this->client_ca) * 2) + 1);

    } else {
        total_len += 3;
    }
    /* client_key_len len & client_key len*/
    if (this->client_key) {
        total_len += (String(strlen_P(this->client_key) * 2).length() + 1 + (strlen_P(this->client_key) * 2));

    } else {
        total_len += 2;
    }
    return total_len;
}

bool SIM7020EHTTPS::chkHTTPChOpen(void)
{
    /* Create a HTTPS Client Instance by Multi Packages for a Long Size Command. refer AT CMD 8.2.2 */
    this->sendAT(GF("+CHTTPCREATEEXT?"));
    if (this->waitResponse(10000L, GF("+CHTTPCREATEEXT: 0,")) == 1) {
        int16_t used_state = this->streamGetIntBefore(',');
        this->waitResponse();
        return (used_state == 1);
    }
    return false;
}

bool SIM7020EHTTPS::endRequest(void)
{
    return this->closeHTTPCh();
}
