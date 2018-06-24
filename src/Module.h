#ifndef _LORALIB_MODULE_H
#define _LORALIB_MODULE_H

#include <SPI.h>

#include "TypeDef.h"

#define SPI_READ  0b00000000
#define SPI_WRITE 0b10000000

class Module {
  public:
    virtual uint8_t begin() = 0;
    
    virtual uint8_t tx(char* data, uint8_t length) = 0;
    virtual uint8_t rxSingle(char* data, uint8_t* length) = 0;
    virtual uint8_t runCAD() = 0;
    
    virtual uint8_t setBandwidth(Bandwidth bw) = 0;
    virtual uint8_t setSpreadingFactor(SpreadingFactor sf) = 0;
    virtual uint8_t setCodingRate(CodingRate cr) = 0;
    virtual uint8_t setFrequency(float freq) = 0;
    virtual uint8_t setSyncWord(uint8_t syncword) = 0;
    
    virtual uint8_t setMode(uint8_t mode) = 0;
    virtual uint8_t config(Bandwidth bw, SpreadingFactor sf, CodingRate cr, float freq, uint8_t syncWord) = 0;
    virtual int8_t getLastPacketRSSI() = 0;
    virtual float getLastPacketSNR() = 0;
    
    void initModule(int nss, int dio0, int dio1);
    
    uint8_t getRegValue(uint8_t reg, uint8_t msb = 7, uint8_t lsb = 0);
    uint8_t readRegisterBurst(uint8_t reg, uint8_t numBytes, uint8_t* inBytes);
    uint8_t readRegisterBurstStr(uint8_t reg, uint8_t numBytes, char* str);
    uint8_t readRegister(uint8_t reg);
    
    uint8_t setRegValue(uint8_t reg, uint8_t value, uint8_t msb = 7, uint8_t lsb = 0);
    void writeRegisterBurst(uint8_t reg, uint8_t* data, uint8_t numBytes);
    void writeRegisterBurstStr(uint8_t reg, const char* data, uint8_t numBytes);
    void writeRegister(uint8_t reg, uint8_t data);
  
  private:
    int _nss;
    int _dio0;
    int _dio1;

};

#endif
