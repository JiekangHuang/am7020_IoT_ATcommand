/*
sim7020_http.h
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/11/2 下午1:16:02
*/

#ifndef _SIM7020_HTTP_H
#define _SIM7020_HTTP_H

#include "sim7020_modem.h"

#define CONN_SERVER_TIMEOUT_MS 90000

typedef enum
{
    HTTPCLIENT_GET,
    HTTPCLIENT_POST,
    HTTPCLIENT_PUT,
    HTTPCLIENT_DELETE,
} HTTP_METHOD_E;

class SIM7020EHTTP : public SIM7020Modem {
  private:
    String  host;
    int16_t statusCode;
    String  body;

    virtual bool newHTTP(const char *host, const char *username = NULL, const char *password = NULL);
    virtual bool chkHTTPChOpen(void);
    bool         connHTTP(void);
    bool         chkHTTPChConn(void);
    bool         disconHTTPCh(void);
    bool         sendHTTPData(HTTP_METHOD_E method, const char *path, const char *header, const char *content_type,
                              const char *content_string);
    bool         startRequest(const char *path, HTTP_METHOD_E method, const char *header = NULL,
                              const char *content_type = NULL, const char *body = NULL);
    virtual bool endRequest(void);

    /* util */
    String stringToHex(const char *data);
    void   makeHeader(String &header, const char *key, const char *value);

  protected:
    bool closeHTTPCh(void);

  public:
    SIM7020EHTTP(Stream &SerialAT, const char *host);
    ~SIM7020EHTTP();
    bool   connServer(void);
    int8_t get(const char *path, const char *header = NULL);

    int8_t post(const char *path, const char *header = NULL);
    int8_t post(const char *path, const char *content_type, const char *body);
    int8_t post(const char *path, const char *header, const char *content_type, const char *body);
    // int8_t put(const char *path, const char *header);
    // int8_t del(const char *path, const char *header);
    void    ParseResponseMsg(void);
    int16_t responseStatusCode(void);
    String  responseBody(void);
    void    ParseSubMsg(void) {}
};

#endif /* _SIM7020_HTTP_H */