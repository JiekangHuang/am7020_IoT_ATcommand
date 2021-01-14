/*
sim7020_nb.h
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/10/29 下午2:45:34
*/

#ifndef _SIM7020_NB_H
#define _SIM7020_NB_H

#include "sim7020_modem.h"
#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>

class SIM7020NB : public SIM7020Modem {
  private:
    /* sim7020 nbiot connect setting */
    bool setAPN(const char *apn);
    bool setBand(uint8_t band);
    // bool connectBS(uint16_t bs_code);
    bool chkSimCard(void);

    void ParseSubMsg(void) {}

  public:
    SIM7020NB(Stream &SerialAT, uint8_t reset_pin);
    ~SIM7020NB();
    bool init(void);
    bool chkNet(void);
    bool nbiotConnect(const char *apn, uint8_t band, uint16_t bs_code = 0);
    bool waitForNetwork(void);
};

#endif /* _SIM7020_NB_H */