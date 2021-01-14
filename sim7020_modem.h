/*
sim7020_modem.h
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/10/29 下午2:45:34
*/

#ifndef _SIM7020_MODEM_H
#define _SIM7020_MODEM_H

#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef SIM7020_RX_BUFF
#define SIM7020_RX_BUFF 64
#endif

#if defined(__AVR__)
#define SIM7020E_PROGMEM PROGMEM
typedef const __FlashStringHelper *GsmConstStr;
#define GFP(x) (reinterpret_cast<GsmConstStr>(x))
#define GF(x) F(x)
#else
#define SIM7020E_PROGMEM
typedef const char *GsmConstStr;
#define GFP(x) x
#define GF(x) x
#endif

#if defined(SIM7020_DEBUG)
template <typename T> static void DBG_PLAIN(T last)
{
    SIM7020_DEBUG.println(last);
}

template <typename T, typename... Args> static void DBG_PLAIN(T head, Args... tail)
{
    SIM7020_DEBUG.print(head);
    SIM7020_DEBUG.print(' ');
    DBG_PLAIN(tail...);
}

template <typename... Args> static void DBG(Args... args)
{
    SIM7020_DEBUG.print(GF("["));
    SIM7020_DEBUG.print(millis());
    SIM7020_DEBUG.print(GF("] "));
    DBG_PLAIN(args...);
}
#else
#define DBG_PLAIN(...)
#define DBG(...)
#endif

#ifndef SIM7020E_YIELD_MS
#define SIM7020E_YIELD_MS 0
#endif

#ifndef SIM7020E_YIELD
#define SIM7020E_YIELD()                                                                                               \
    {                                                                                                                  \
        delay(SIM7020E_YIELD_MS);                                                                                      \
    }
#endif

#define GSM_NL "\r\n"
static const char GSM_OK[] SIM7020E_PROGMEM    = "OK" GSM_NL;
static const char GSM_ERROR[] SIM7020E_PROGMEM = "ERROR" GSM_NL;

class SIM7020Modem {
  private:
    Stream *_at;
    uint8_t _reset_pin;

  public:
    virtual void ParseSubMsg(void) = 0;
    /* sim7020 nbiot hardware setting */
    void restart(void);
    bool testAT(uint32_t timeout_ms = 10000L);

    /* stream util */
    template <typename T> void streamWrite(T last) { this->_at->print(last); }

    template <typename T, typename... Args> void streamWrite(T head, Args... tail)
    {
        this->_at->print(head);
        this->streamWrite(tail...);
    }

    template <typename... Args> void sendAT(Args... cmd)
    {
        this->streamWrite("AT", cmd..., GF(GSM_NL));
        this->_at->flush();
        SIM7020E_YIELD();
    }

    int     streamRead(void);
    int     streamAvailable(void);
    bool    streamGetLength(char *buf, int8_t numChars, const uint32_t timeout_ms = 1000L);
    int16_t streamGetIntLength(int8_t numChars, const uint32_t timeout_ms = 1000L);
    int16_t streamGetIntBefore(char lastChar);
    float   streamGetFloatLength(int8_t numChars, const uint32_t timeout_ms = 1000L);
    float   streamGetFloatBefore(char lastChar);
    String  streamGetStringBefore(char lastChar);
    bool    streamSkipUntil(const char c, const uint32_t timeout_ms = 1000L);
    int8_t  waitResponse(uint32_t timeout_ms, String &data, GsmConstStr r1 = GFP(GSM_OK),
                         GsmConstStr r2 = GFP(GSM_ERROR), GsmConstStr r3 = NULL, GsmConstStr r4 = NULL,
                         GsmConstStr r5 = NULL);
    int8_t  waitResponse(uint32_t timeout_ms, GsmConstStr r1 = GFP(GSM_OK), GsmConstStr r2 = GFP(GSM_ERROR),
                         GsmConstStr r3 = NULL, GsmConstStr r4 = NULL, GsmConstStr r5 = NULL);
    int8_t  waitResponse(GsmConstStr r1 = GFP(GSM_OK), GsmConstStr r2 = GFP(GSM_ERROR), GsmConstStr r3 = NULL,
                         GsmConstStr r4 = NULL, GsmConstStr r5 = NULL);

  public:
    SIM7020Modem(Stream &SerialAT, uint8_t reset_pin);
    SIM7020Modem(Stream &SerialAT);
    ~SIM7020Modem();
};

#endif /* _SIM7020_MODEM_H */