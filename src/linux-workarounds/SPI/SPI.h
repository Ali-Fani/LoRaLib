#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include <inttypes.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#ifndef NOWIRINGIPI
  #include <wiringPi.h>
  #include <wiringPiSPI.h>
#else
  #define wiringPiSetup(...) {}
  #define wiringPiSPISetupMode(...) (-1)
  #define wiringPiSPIDataRW(...) (-1)
  #define millis(...) (0)
  #define micros(...) (0)
  #define delay(...) (0)
  #define delayMicroseconds(...) (0)
  #define INPUT 1
  #define OUTPUT 0
  #define LOW 0
  #define HIGH 1

  void digitalWrite(uint32_t pin, uint32_t value);
  uint32_t digitalRead(uint32_t pin);
  void pinMode(uint32_t pin, uint32_t mode);
#endif


// SPI_HAS_TRANSACTION means SPI has beginTransaction(), endTransaction(),
// usingInterrupt(), and SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 0

// SPI_HAS_NOTUSINGINTERRUPT means that SPI has notUsingInterrupt() method
#define SPI_HAS_NOTUSINGINTERRUPT 0
#define SPI_HAS_USINGINTERRUPT 0

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

class SPISettings {
  int speed;
  int mode;
  bool isLSBmode;
  int channel;

  static const uint8_t bitReverseTable256[256];

  friend class SPIClass;

public:
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode, int spiChannel) //nonstandard, but full ctor
  {
    speed = clock;
    isLSBmode = (bitOrder == LSBFIRST);
    mode = dataMode;
    channel = spiChannel;
  }

  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
  {
    speed = clock;
    isLSBmode = (bitOrder == LSBFIRST);
    mode = dataMode;
    channel = 0;
  }

  SPISettings()
  {
    speed = 500000;
    mode = SPI_MODE0;
    isLSBmode = false;
    channel = 0;
  }

  inline uint8_t prepareByte(uint8_t b) {
    return (!isLSBmode ? b : bitReverseTable256[b]);
  }
};


class SPIClass {
  static uint8_t initialized;
  static int spiDeviceFp;
  static SPISettings settings;

public:
  SPIClass() {
    wiringPiSetup(); // USING wiringPi numbers scheme!!!! exec command "gpio readall" to check them
  }

  // Initialize the SPI library
  static void begin();

  // Before using SPI.transfer() or asserting chip select pins,
  // this function is used to gain exclusive access to the SPI bus
  // and configure the correct settings.
  inline static void beginTransaction(SPISettings settings) {
    if (!initialized++) {
      SPIClass::settings = settings;
      if (SPIClass::spiDeviceFp != -1) {
        close(SPIClass::spiDeviceFp);
      }
      SPIClass::spiDeviceFp = wiringPiSPISetupMode(settings.channel, settings.speed, settings.mode);
      if (SPIClass::spiDeviceFp == -1) {
        initialized = 0;
      }
    }
  }

  // Write to the SPI bus (MOSI pin) and also receive (MISO pin)
  inline static uint8_t transfer(uint8_t data) {
    data = settings.prepareByte(data);
    if (wiringPiSPIDataRW(settings.channel, (unsigned char*)&data, 1) == -1) {
       int err = errno;
       printf("error writing/reading to SPI: %d - %s\n", err, strerror(err));
    }
    return data;
  }

  inline static void transfer(void *buf, size_t count) {
    unsigned char* dataBuf = new unsigned char[count];
    memcpy(dataBuf, buf, count);

    for (size_t i = 0; i < count; ++i) {
      dataBuf[i] = settings.prepareByte(dataBuf[i]);
    }

    if (wiringPiSPIDataRW(settings.channel, dataBuf, count) == -1) {
      int err = errno;
      printf("error writing/reading to SPI: %d\n", err);
    }
    delete[] dataBuf;
  }

  // After performing a group of transfers and releasing the chip select
  // signal, this function allows others to access the SPI bus
  inline static void endTransaction(void) {
    if (initialized) {
      if (--initialized <= 0) {
        if (SPIClass::spiDeviceFp != -1) {
          close(SPIClass::spiDeviceFp);
          SPIClass::spiDeviceFp = -1;
        }
        initialized = 0;
      }
    }
  }

  // Disable the SPI bus
  static void end();

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setBitOrder(uint8_t bitOrder) {
    settings.isLSBmode = (bitOrder == LSBFIRST);
  }

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setDataMode(uint8_t dataMode) {
    settings.mode = dataMode;
  }

  // This function is deprecated.  New applications should use
  // beginTransaction() to configure SPI settings.
  inline static void setClockDivider(uint8_t clockDiv) {
  }
  // These undocumented functions should not be used.  SPI.transfer()
  // polls the hardware flag which is automatically cleared as the
  // AVR responds to SPI's interrupt
  inline static void attachInterrupt() {  }
  inline static void detachInterrupt() {  }

};

extern SPIClass SPI;

#endif
