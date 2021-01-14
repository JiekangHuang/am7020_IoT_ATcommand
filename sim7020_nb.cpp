/*
sim7020_nb.cpp
Author : Zack Huang
Link   : zack@atticedu.com
Date   : 2020/10/29 下午2:45:40
*/
#include "sim7020_nb.h"

SIM7020NB::SIM7020NB(Stream &SerialAT, uint8_t reset_pin) : SIM7020Modem(SerialAT, reset_pin)
{
}

SIM7020NB::~SIM7020NB()
{
}

/* sim7020 nbiot connect setting */
bool SIM7020NB::setAPN(const char *apn)
{
    /* Set Default PSD Connection Settings. refer AT CMD 3.2.46 */
    this->sendAT(GF("*MCGDEFCONT="), GF("\"IP\",\""), apn, GF("\""));
    return (waitResponse() == 1);
}

bool SIM7020NB::setBand(uint8_t band)
{
    /* Get and Set Mobile Operation Band. refer AT CMD 5.2.4 */
    this->sendAT(GF("+CBAND="), band);
    return (waitResponse() == 1);
}

bool SIM7020NB::chkNet(void)
{
    int status;
    /* EPS Network Registration Status. refer AT CMD 3.2.47 */
    this->sendAT(GF("+CEREG?"));
    this->waitResponse(GF("+CEREG: 0,"));
    status = this->streamGetIntBefore('\n');
    this->waitResponse();
    return (status == 1);
}

bool SIM7020NB::chkSimCard(void)
{
    int8_t status;
    /* Enter PIN. refer AT CMD 3.2.11 */
    this->sendAT(GF("+CPIN?"));
    status = this->waitResponse(GF("READY"), GF("SIM PIN"), GF("SIM PUK"), GF("NOT INSERTED"), GF("NOT READY"));
    this->waitResponse();
    return (status == 1);
}

bool SIM7020NB::init(void)
{
    randomSeed(analogRead(A0));
    this->restart();
    if (!this->testAT()) {
        return false;
    }
    /* Echo Off. refer AT CMD 2.2.1 */
    this->sendAT(GF("E0"));
    this->waitResponse();

    /* Control the Data Output Format. refer AT CMD 5.2.18 */
    this->sendAT(GF("+CREVHEX=0"));
    return (this->waitResponse() == 1);
}

bool SIM7020NB::nbiotConnect(const char *apn, uint8_t band, uint16_t bs_code)
{
    if (!this->chkSimCard() || !this->setAPN(apn) || !this->setBand(band)) {
        return false;
    }
    if (bs_code > 0) {
        /* Operator Selection. refer AT CMD 3.2.10 */
        this->sendAT(GF("+COPS=1,2,\""), bs_code, GF("\""));
        return (waitResponse((long)(120 * 1000L)) == 1);
    }
    return true;
}

bool SIM7020NB::waitForNetwork(void)
{
    return this->chkNet();
}
