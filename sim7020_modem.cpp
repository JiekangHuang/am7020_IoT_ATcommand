/*
sim7020_modem.cpp
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/10/29 下午2:45:40
*/
#include "sim7020_modem.h"

SIM7020Modem::SIM7020Modem(Stream &SerialAT, uint8_t reset_pin)
{
    this->_at        = &SerialAT;
    this->_reset_pin = reset_pin;
}

SIM7020Modem::SIM7020Modem(Stream &SerialAT)
{
    this->_at = &SerialAT;
}

SIM7020Modem::~SIM7020Modem()
{
}

/* sim7020 Modemiot hardware setting */
void SIM7020Modem::restart(void)
{
    pinMode(this->_reset_pin, OUTPUT);
    digitalWrite(this->_reset_pin, LOW);
    delay(500);
    digitalWrite(this->_reset_pin, HIGH);
    delay(5000);
}

bool SIM7020Modem::testAT(uint32_t timeout_ms)
{
    for (uint32_t start = millis(); millis() - start < timeout_ms;) {
        /* AT Command syntax. refer AT CMD 1.4 */
        this->sendAT(GF(""));
        if (this->waitResponse(200) == 1) {
            return true;
        }
        delay(100);
    }
    return false;
}

int SIM7020Modem::streamRead(void)
{
    return this->_at->read();
}

int SIM7020Modem::streamAvailable(void)
{
    return this->_at->available();
}

bool SIM7020Modem::streamGetLength(char *buf, int8_t numChars, const uint32_t timeout_ms)
{
    if (!buf) {
        return false;
    }

    int8_t   numCharsReady = -1;
    uint32_t startMillis   = millis();
    while (millis() - startMillis < timeout_ms && (numCharsReady = this->_at->available()) < numChars) {
        SIM7020E_YIELD();
    }

    if (numCharsReady >= numChars) {
        this->_at->readBytes(buf, numChars);
        return true;
    }

    return false;
}

int16_t SIM7020Modem::streamGetIntLength(int8_t numChars, const uint32_t timeout_ms)
{
    char buf[numChars + 1];
    if (streamGetLength(buf, numChars, timeout_ms)) {
        buf[numChars] = '\0';
        return atoi(buf);
    }

    return -9999;
}

int16_t SIM7020Modem::streamGetIntBefore(char lastChar)
{
    char   buf[7];
    size_t bytesRead = this->_at->readBytesUntil(lastChar, buf, static_cast<size_t>(7));
    // if we read 7 or more bytes, it's an overflow
    if (bytesRead && bytesRead < 7) {
        buf[bytesRead] = '\0';
        int16_t res    = atoi(buf);
        return res;
    }

    return -9999;
}

float SIM7020Modem::streamGetFloatLength(int8_t numChars, const uint32_t timeout_ms)
{
    char buf[numChars + 1];
    if (streamGetLength(buf, numChars, timeout_ms)) {
        buf[numChars] = '\0';
        return atof(buf);
    }

    return -9999.0F;
}

float SIM7020Modem::streamGetFloatBefore(char lastChar)
{
    char   buf[16];
    size_t bytesRead = this->_at->readBytesUntil(lastChar, buf, static_cast<size_t>(16));
    // if we read 16 or more bytes, it's an overflow
    if (bytesRead && bytesRead < 16) {
        buf[bytesRead] = '\0';
        float res      = atof(buf);
        return res;
    }

    return -9999.0F;
}

String SIM7020Modem::streamGetStringBefore(char lastChar)
{
    return this->_at->readStringUntil(lastChar);
}

bool SIM7020Modem::streamSkipUntil(const char c, const uint32_t timeout_ms)
{
    uint32_t startMillis = millis();
    while (millis() - startMillis < timeout_ms) {
        while (millis() - startMillis < timeout_ms && !this->_at->available()) {
            SIM7020E_YIELD();
        }
        if (this->_at->read() == c) {
            return true;
        }
    }
    return false;
}

int8_t SIM7020Modem::waitResponse(uint32_t timeout_ms, String &data, GsmConstStr r1, GsmConstStr r2, GsmConstStr r3,
                                  GsmConstStr r4, GsmConstStr r5)
{
    data.reserve(SIM7020_RX_BUFF);
    uint8_t  index       = 0;
    uint32_t startMillis = millis();
    bool     flag        = false;
    do {
        SIM7020E_YIELD();
        while (this->_at->available() > 0) {
            SIM7020E_YIELD();
            int8_t a = this->_at->read();
            if (a <= 0)
                continue;     // Skip 0x00 bytes, just in case
            data += static_cast<char>(a);
            if (r1 && data.endsWith(r1)) {
                index = 1;
                flag  = true;
                break;
            } else if (r2 && data.endsWith(r2)) {
                index = 2;
                flag  = true;
                break;
            } else if (r3 && data.endsWith(r3)) {
                index = 3;
                flag  = true;
                break;
            } else if (r4 && data.endsWith(r4)) {
                index = 4;
                flag  = true;
                break;
            } else if (r5 && data.endsWith(r5)) {
                index = 5;
                flag  = true;
                break;
            } else if (data.endsWith(GF("+CMQPUB: "))) {
                // Process URC msg
                this->ParseSubMsg();
            }
        }

        if (flag) {
            break;
        }
    } while (millis() - startMillis < timeout_ms);

    if (!index) {
        data.trim();
        if (data.length()) {
            DBG(GF("### Unhandled:"), data);
        }
        data = "";
    }
    return index;
}

int8_t SIM7020Modem::waitResponse(uint32_t timeout_ms, GsmConstStr r1, GsmConstStr r2, GsmConstStr r3, GsmConstStr r4,
                                  GsmConstStr r5)
{
    String data;
    return waitResponse(timeout_ms, data, r1, r2, r3, r4, r5);
}

int8_t SIM7020Modem::waitResponse(GsmConstStr r1, GsmConstStr r2, GsmConstStr r3, GsmConstStr r4, GsmConstStr r5)
{
    return waitResponse(1000, r1, r2, r3, r4, r5);
}
