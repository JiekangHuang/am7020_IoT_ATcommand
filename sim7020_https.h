/*
sim7020_https.h
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/11/2 下午1:16:02
*/

#ifndef _SIM7020_HTTPS_H
#define _SIM7020_HTTPS_H

#include "sim7020_http.h"

class SIM7020EHTTPS : public SIM7020EHTTP {
  private:
    const char *root_ca;
    const char *client_ca;
    const char *client_key;

    bool newHTTP(const char *host, const char *username = NULL, const char *password = NULL);
    bool sendHTTPSinglePackages(uint8_t not_end_flag, int total_len, const char *data);
    bool sendHTTPCertMultiPackages(uint8_t not_end_flag, int total_len, const char *cert);
    int  calcPackageLen(const char *host, const char *username = NULL, const char *password = NULL);
    bool chkHTTPChOpen(void);
    bool endRequest(void);

  public:
    SIM7020EHTTPS(Stream &SerialAT, const char *host, const char *root_ca, const char *client_ca = NULL,
                  const char *client_key = NULL);
    ~SIM7020EHTTPS();
};

#endif /* _SIM7020_HTTPS_H */