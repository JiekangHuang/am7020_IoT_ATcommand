/*
sim7020_http.cpp
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/11/2 下午1:16:02
*/

#include "sim7020_http.h"

SIM7020EHTTP::SIM7020EHTTP(Stream &SerialAT, const char *host) : SIM7020Modem(SerialAT)
{
    this->host = host;
}

SIM7020EHTTP::~SIM7020EHTTP()
{
}

/*   private */
bool SIM7020EHTTP::newHTTP(const char *host, const char *username, const char *password)
{
    /* Create a HTTP Client Instance. refer AT CMD 8.2.1 */
    this->sendAT(GF("+CHTTPCREATE=\"http://"), host, GF("\",\""), username, GF("\",\""), password, GF("\""));
    if (this->waitResponse(30000L, GF("+CHTTPCREATE: 0")) == 1 && this->waitResponse() == 1) {
        return true;
    }
    return false;
}

bool SIM7020EHTTP::chkHTTPChOpen(void)
{
    /* Create a HTTP Client Instance. refer AT CMD 8.2.1 */
    this->sendAT(GF("+CHTTPCREATE?"));
    if (this->waitResponse(10000L, GF("+CHTTPCREATE: 0,")) == 1) {
        int16_t used_state = this->streamGetIntBefore(',');
        this->waitResponse();
        return (used_state == 1);
    }
    return false;
}

bool SIM7020EHTTP::connHTTP(void)
{
    /* Establish the HTTP(S) Connection. refer AT CMD 8.2.3 */
    this->sendAT(GF("+CHTTPCON=0"));
    return (this->waitResponse(30000) == 1);
}

bool SIM7020EHTTP::chkHTTPChConn(void)
{
    /* Establish the HTTP(S) Connection. refer AT CMD 8.2.3 */
    this->sendAT(GF("+CHTTPCON?"));
    if (this->waitResponse(10000L, GF("+CHTTPCON: 0,")) == 1) {
        int16_t conn_state = this->streamGetIntBefore(',');
        this->waitResponse();
        return (conn_state == 1);
    }
    return false;
}

bool SIM7020EHTTP::closeHTTPCh(void)
{
    /* Destroy the HTTP(S) Client Instance. refer AT CMD 8.2.5 */
    this->sendAT(GF("+CHTTPDESTROY=0"));
    return (this->waitResponse(2000) == 1);
}

bool SIM7020EHTTP::disconHTTPCh(void)
{
    /* Close the HTTP(S) Connection. refer AT CMD 8.2.4 */
    this->sendAT(GF("+CHTTPDISCON=0"));
    return (this->waitResponse(2000) == 1);
}

bool SIM7020EHTTP::sendHTTPData(HTTP_METHOD_E method, const char *path, const char *header, const char *content_type,
                                const char *content_string)
{
    /* Send HTTP(S) Package. refer AT CMD 8.2.6 */
    this->sendAT(GF("+CHTTPSEND=0,"), method, GF(",\""), path, GF("\",\""), header, GF("\",\""), content_type,
                 GF("\",\""), content_string, GF("\""));
    return (this->waitResponse(10000) == 1);
}

bool SIM7020EHTTP::startRequest(const char *path, HTTP_METHOD_E method, const char *header, const char *content_type,
                                const char *body)
{
    String temp_header = "";
    if (this->connServer()) {
        this->makeHeader(temp_header, "Host", this->host.c_str());
        if (header) {
            temp_header += header;
            temp_header += "\n";
        }
        this->sendHTTPData(method, path, this->stringToHex(temp_header.c_str()).c_str(), content_type,
                           this->stringToHex(body).c_str());
        /* Header of the Response from Host. refer AT CMD 8.2.13 */
        if (this->waitResponse(30000L, GF("+CHTTPNMIH: 0,")) == 1) {
            this->ParseResponseMsg();
            this->endRequest();
            return true;
        }
    }
    return false;
}

bool SIM7020EHTTP::endRequest(void)
{
    return this->disconHTTPCh();
}

/* util */
String SIM7020EHTTP::stringToHex(const char *data)
{
    String buff = "";
    if (data) {
        size_t data_len = strlen(data);
        buff.reserve(data_len * 2);
        for (size_t ii = 0; ii < data_len; ii++) {
            String hex = String((uint8_t)data[ii], HEX);
            if (hex.length() <= 1) {
                hex = ("0" + hex);
            }
            buff += hex;
        }
    }
    return buff;
}

void SIM7020EHTTP::makeHeader(String &header, const char *key, const char *value)
{
    header += key;
    header += ": ";
    header += value;
    header += "\n";
}

/* public */
bool SIM7020EHTTP::connServer(void)
{
    unsigned long timer = millis() + CONN_SERVER_TIMEOUT_MS;
    while (millis() < timer) {
        if (!this->chkHTTPChOpen()) {
            this->closeHTTPCh();
            this->newHTTP(this->host.c_str());
            continue;
        } else {
            if (!this->chkHTTPChConn()) {
                this->disconHTTPCh();
                this->connHTTP();
                continue;
            }
            return true;
        }
    }
    return false;
}

int8_t SIM7020EHTTP::get(const char *path, const char *header)
{
    this->startRequest(path, HTTPCLIENT_GET, header);
    return this->statusCode;
}

int8_t SIM7020EHTTP::post(const char *path, const char *header)
{
    this->startRequest(path, HTTPCLIENT_POST, header);
    return this->statusCode;
}

int8_t SIM7020EHTTP::post(const char *path, const char *content_type, const char *body)
{
    this->startRequest(path, HTTPCLIENT_POST, NULL, content_type, body);
    return this->statusCode;
}

int8_t SIM7020EHTTP::post(const char *path, const char *header, const char *content_type, const char *body)
{
    this->startRequest(path, HTTPCLIENT_POST, header, content_type, body);
    return this->statusCode;
}

void SIM7020EHTTP::ParseResponseMsg(void)
{
    uint16_t data_len;
    this->statusCode = this->streamGetIntBefore(',');
    data_len         = this->streamGetIntBefore(',');
    DBG(GF("###"), GF("statusCode: "), this->statusCode);
    DBG(GF("###"), GF("header len: "), data_len);

    // read Header of the Response from Host
    for (size_t ii = 0; ii < data_len; ii++) {
        while (!this->streamAvailable())
            ;
        this->streamRead();
    }

    /* Content of The Response from Host. refer AT CMD 8.2.14 */
    if (this->waitResponse(5000L, GF("+CHTTPNMIC: 0,")) == 1) {
        this->streamSkipUntil(',');
        this->streamSkipUntil(',');
        data_len   = this->streamGetIntBefore(',');
        this->body = "";
        this->body.reserve(data_len);

        // read Content of the Response from Host
        // hex string to string
        for (size_t i = 0; i < data_len; i++) {
            char buff[4] = {0, 0, 0, 0}, c;
            this->streamGetLength(buff, 2);
            c = strtol(buff, NULL, 16);
            this->body += c;
        }
        DBG(GF("###"), GF("body: "), this->body);
    }
}

int16_t SIM7020EHTTP::responseStatusCode(void)
{
    int16_t temp     = this->statusCode;
    this->statusCode = 0;
    return temp;
}

String SIM7020EHTTP::responseBody(void)
{
    String temp = this->body;
    this->body  = "";
    return temp;
}
